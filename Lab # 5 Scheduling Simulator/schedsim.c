// schedsim.c — Self-contained scheduler with robust input parser and main()
// Build flags (Makefile): -Wall -Werror -pedantic -std=c99 -g

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/* ---------- Data type ---------- */
typedef struct Process {
    int pid;  /* Process ID */
    int bt;   /* Burst Time */
    int art;  /* Arrival Time (assume 0 for lab) */
    int wt;   /* Waiting Time */
    int tat;  /* Turnaround Time */
    int pri;  /* Priority (lower value = higher priority) */
} ProcessType;

/* ---------- Common helpers ---------- */
static void compute_tat(ProcessType *plist, int n) {
    for (int i = 0; i < n; i++) plist[i].tat = plist[i].wt + plist[i].bt;
}

static void findWaitingTimeFCFS(ProcessType *plist, int n) {
    int elapsed = 0;
    for (int i = 0; i < n; i++) {
        if (elapsed < plist[i].art) elapsed = plist[i].art;   /* future-proof */
        plist[i].wt = elapsed - plist[i].art;
        elapsed += plist[i].bt;
    }
}

/* ---------- Printing helpers (match reference style) ---------- */
static void print_table_header(void) {
    printf("    Processes    Burst time    Waiting time    Turn around time\n");
}
static void print_table_row(const ProcessType *p) {
    printf("%12d%14d%15d%18d\n", p->pid, p->bt, p->wt, p->tat);
}
static void print_avgs(const ProcessType *plist, int n) {
    double aw = 0, at = 0;
    for (int i = 0; i < n; i++) { aw += plist[i].wt; at += plist[i].tat; }
    aw /= n; at /= n;
    printf("\nAverage waiting time = %.2f\n", aw);
    printf("Average turn around time = %.2f\n\n", at);
}
static void print_section(const char *title, ProcessType *p, int n) {
    printf("**********\n%s\n\n", title);
    print_table_header();
    for (int i = 0; i < n; i++) print_table_row(&p[i]);
    print_avgs(p, n);
}

/* ---------- Priority (sort + FCFS among equals) ---------- */
static int cmp_priority_then_fcfs(const void *a, const void *b) {
    const ProcessType *pa = (const ProcessType *)a;
    const ProcessType *pb = (const ProcessType *)b;
    /* Lower pri value = higher priority. Flip sign if your course uses opposite. */
    if (pa->pri != pb->pri) return pa->pri - pb->pri;
    /* FCFS among equals */
    if (pa->art != pb->art) return pa->art - pb->art;
    return pa->pid - pb->pid;
}
static void schedulePriority(ProcessType *plist, int n) {
    qsort(plist, n, sizeof(ProcessType), cmp_priority_then_fcfs);
    findWaitingTimeFCFS(plist, n);
    compute_tat(plist, n);
}

/* ---------- SJF (preemptive / SRTF) ---------- */
static void scheduleSJF_Preemptive(ProcessType *plist, int n) {
    int *rem = (int *)malloc(sizeof(int) * n);
    int *fin = (int *)malloc(sizeof(int) * n);
    if (!rem || !fin) { free(rem); free(fin); exit(2); }

    for (int i = 0; i < n; i++) { rem[i] = plist[i].bt; fin[i] = -1; }

    /* Start at earliest arrival (0 in this lab) */
    int earliest = INT_MAX;
    for (int i = 0; i < n; i++) if (plist[i].art < earliest) earliest = plist[i].art;
    int t = (earliest == INT_MAX ? 0 : earliest);
    int complete = 0;

    while (complete != n) {
        int shortest = -1, minrem = INT_MAX;
        for (int j = 0; j < n; j++) {
            if (plist[j].art <= t && rem[j] > 0 && rem[j] < minrem) {
                minrem = rem[j];
                shortest = j;
            }
        }
        if (shortest == -1) {
            int next_arr = INT_MAX;
            for (int j = 0; j < n; j++) if (rem[j] > 0 && plist[j].art < next_arr) next_arr = plist[j].art;
            if (next_arr == INT_MAX) break;   /* safety */
            t = next_arr;
            continue;
        }
        rem[shortest]--; t++;
        if (rem[shortest] == 0) { fin[shortest] = t; complete++; }
    }

    for (int i = 0; i < n; i++) {
        plist[i].wt = fin[i] - plist[i].art - plist[i].bt;
        if (plist[i].wt < 0) plist[i].wt = 0;
        plist[i].tat = plist[i].wt + plist[i].bt;
    }
    free(rem); free(fin);
}

/* ---------- Round Robin ---------- */
static void scheduleRR(ProcessType *plist, int n, int quantum) {
    int *rem = (int *)malloc(sizeof(int) * n);
    if (!rem) exit(2);
    for (int i = 0; i < n; i++) { rem[i] = plist[i].bt; plist[i].wt = 0; }
    int t = 0;  /* all arrivals are 0 */

    for (;;) {
        int done = 1;
        for (int i = 0; i < n; i++) if (rem[i] > 0) {
            done = 0;
            if (rem[i] > quantum) { t += quantum; rem[i] -= quantum; }
            else { t += rem[i]; plist[i].wt = t - plist[i].bt; rem[i] = 0; }
        }
        if (done) break;
    }
    compute_tat(plist, n);
}

/* ---------- Robust input parser & driver ---------- */

static int is_blank_or_comment(const char *s) {
    for (; *s; ++s) {
        if (*s == '#') return 1;        /* treat as comment line */
        if (!isspace((unsigned char)*s)) return 0;
    }
    return 1; /* blank */
}

static ProcessType *read_input(const char *path, int *out_n) {
    FILE *fp = fopen(path, "r");
    if (!fp) { perror("fopen"); fprintf(stderr, "Usage: ./schedsim <input-file-path>\n"); exit(1); }

    int cap = 16, n = 0;
    ProcessType *p = (ProcessType *)malloc(sizeof(ProcessType) * cap);
    if (!p) exit(2);

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (is_blank_or_comment(line)) continue;

        int pid = 0, bt = 0, pri = 0;
        int got = sscanf(line, "%d %d %d", &pid, &bt, &pri);
        if (got < 2) continue;          /* ignore malformed rows */
        if (got == 2) pri = 0;          /* default priority if not present */

        /* Skip ghost/idle rows that caused your extra lines */
        if (pid == 0 && bt == 0) continue;

        if (n == cap) {
            cap *= 2;
            p = (ProcessType *)realloc(p, sizeof(ProcessType) * cap);
            if (!p) exit(2);
        }
        p[n].pid = pid; p[n].bt = bt; p[n].pri = pri;
        p[n].art = 0; p[n].wt = 0; p[n].tat = 0;
        n++;
    }
    fclose(fp);
    if (n == 0) { fprintf(stderr, "No valid processes found in %s\n", path); exit(1); }
    *out_n = n;
    return p;
}

static void copy_procs(ProcessType *dst, const ProcessType *src, int n) {
    memcpy(dst, src, sizeof(ProcessType) * n);
}

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "Usage: ./schedsim <input-file-path>\n"); return 1; }

    int n = 0;
    ProcessType *orig = read_input(argv[1], &n);
    ProcessType *buf  = (ProcessType *)malloc(sizeof(ProcessType) * n);
    if (!buf) { free(orig); return 2; }

    /* FCFS (given) */
    copy_procs(buf, orig, n);
    findWaitingTimeFCFS(buf, n); compute_tat(buf, n);
    print_section("FCFS", buf, n);

    /* SJF (preemptive / SRTF) */
    copy_procs(buf, orig, n);
    scheduleSJF_Preemptive(buf, n);
    print_section("SJF", buf, n);

    /* Priority */
    copy_procs(buf, orig, n);
    schedulePriority(buf, n);
    print_section("Priority", buf, n);

    /* Round Robin with quantum = 2 (per guide) */
    int quantum = 2;
    printf("**********\nRR Quantum = %d\n\n", quantum);
    copy_procs(buf, orig, n);
    scheduleRR(buf, n, quantum);
    print_table_header();
    for (int i = 0; i < n; i++) print_table_row(&buf[i]);
    print_avgs(buf, n);

    free(buf);
    free(orig);
    return 0;
}
