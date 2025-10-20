// shell2.c — Quash: simple UNIX-like shell for Tasks 1–6
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

extern char **environ;

static volatile pid_t fg_child_pid = 0;      // Foreground child PID (if any)
static const int TIMEOUT_SECONDS = 10;       // Task 5 timeout

static const char *DELIMS = " \t\r\n";

// --------------------------- helpers ---------------------------------

static void print_prompt(void) {
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) strcpy(cwd, "?");
    printf("%s> ", cwd);                    // e.g., /home/codio>
    fflush(stdout);
}

static char *trim(char *s) {
    if (!s) return s;
    while (*s && isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n-1])) s[--n] = '\0';
    return s;
}

// Expand $VARNAME (no quotes/escapes) into out
static void expand_var(const char *in, char *out, size_t outsz) {
    size_t oi = 0;
    for (size_t i = 0; in[i] && oi + 1 < outsz; ) {
        if (in[i] == '$') {
            i++;
            if (isalpha((unsigned char)in[i]) || in[i] == '_') {
                char name[128]; size_t ni = 0;
                while (in[i] && (isalnum((unsigned char)in[i]) || in[i]=='_') && ni+1 < sizeof(name)) {
                    name[ni++] = in[i++];
                }
                name[ni] = '\0';
                const char *val = getenv(name);
                if (!val) val = "";
                for (size_t k = 0; val[k] && oi + 1 < outsz; k++) out[oi++] = val[k];
            } else {
                out[oi++] = '$';            // literal $
            }
        } else {
            out[oi++] = in[i++];
        }
    }
    out[oi] = '\0';
}

// Tokenize by whitespace; apply $VAR expansion to each token.
// NOTE: for simplicity we expect operators like ">" and "&" to be separated by spaces.
static int tokenize_and_expand(char *line, char *argv[], int maxv) {
    int argc = 0;
    for (char *tok = strtok(line, DELIMS); tok && argc < maxv-1; tok = strtok(NULL, DELIMS)) {
        static char buf[PATH_MAX*2];
        expand_var(tok, buf, sizeof(buf));
        argv[argc++] = strdup(buf);         // dup so caller can free
    }
    argv[argc] = NULL;
    return argc;
}

static void free_argv(char *argv[], int argc) {
    for (int i = 0; i < argc; i++) if (argv[i]) free(argv[i]);
}

// --------------------------- signals ---------------------------------

static void on_sigint(int sig) {
    (void)sig;
    if (fg_child_pid > 0) {
        kill(fg_child_pid, SIGINT);         // forward Ctrl-C to child
    } else {
        write(STDOUT_FILENO, "\n", 1);
        print_prompt();
    }
}

static void on_sigalrm(int sig) {
    (void)sig;
    if (fg_child_pid > 0) kill(fg_child_pid, SIGKILL);  // timeout kill
}

// --------------------------- built-ins -------------------------------

static bool is_builtin(const char *cmd) {
    return cmd &&
           (!strcmp(cmd, "cd") || !strcmp(cmd, "pwd") || !strcmp(cmd, "echo") ||
            !strcmp(cmd, "env") || !strcmp(cmd, "setenv") || !strcmp(cmd, "exit"));
}

static int bi_cd(char *argv[]) {
    const char *path = argv[1] ? argv[1] : getenv("HOME");
    if (!path) path = ".";
    if (chdir(path) != 0) perror("cd");
    return 0;
}

static int bi_pwd(void) {
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) { perror("pwd"); return -1; }
    puts(cwd);
    return 0;
}

static int bi_echo(char *argv[]) {
    for (int i = 1; argv[i]; i++) {
        if (i > 1) putchar(' ');
        fputs(argv[i], stdout);
    }
    putchar('\n');
    return 0;
}

static int bi_env(char *name_opt) {
    if (name_opt) {
        const char *v = getenv(name_opt);
        if (v) puts(v);
        return 0;
    }
    for (char **e = environ; *e; ++e) puts(*e);
    return 0;
}

static int bi_setenv(char *name, char *val) {
    if (!name || !val) { fprintf(stderr, "usage: setenv NAME VALUE\n"); return -1; }
    if (setenv(name, val, 1) != 0) perror("setenv");
    return 0;
}

static void run_builtin(char *argv[]) {
    if (!strcmp(argv[0], "cd")) { bi_cd(argv); return; }
    if (!strcmp(argv[0], "pwd")) { bi_pwd(); return; }
    if (!strcmp(argv[0], "echo")) { bi_echo(argv); return; }
    if (!strcmp(argv[0], "env")) { bi_env(argv[1]); return; }
    if (!strcmp(argv[0], "setenv")) { bi_setenv(argv[1], argv[2]); return; }
    if (!strcmp(argv[0], "exit")) { exit(0); }
}

// --------------------------- exec plan (>, &) ------------------------

typedef struct {
    char *args[MAX_COMMAND_LINE_ARGS];
    int   argc;
    int   background;
    char *redirect_out;                       // filename after >
} exec_plan_t;

static void build_plan(char *argv[], int argc, exec_plan_t *p) {
    memset(p, 0, sizeof(*p));
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "&") && i == argc-1) {
            p->background = 1;
        } else if (!strcmp(argv[i], ">") && i+1 < argc) {
            p->redirect_out = argv[i+1];
            i++; // skip filename
        } else {
            p->args[p->argc++] = argv[i];
        }
    }
    p->args[p->argc] = NULL;
}

// --------------------------- main loop --------------------------------

int main(void) {
    // install signal handlers
    struct sigaction sa_int = {0}, sa_alrm = {0};
    sa_int.sa_handler = on_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    sa_alrm.sa_handler = on_sigalrm;
    sigemptyset(&sa_alrm.sa_mask);
    sigaction(SIGALRM, &sa_alrm, NULL);

    char line_buf[MAX_COMMAND_LINE_LEN];

    while (true) {
        print_prompt();

        if (!fgets(line_buf, sizeof(line_buf), stdin)) {
            if (feof(stdin)) { putchar('\n'); break; }
            perror("fgets"); continue;
        }

        char *line = trim(line_buf);
        if (*line == '\0') continue;

        // Build argv with $VAR expansion
        char *argv[MAX_COMMAND_LINE_ARGS] = {0};
        int argc = tokenize_and_expand(line, argv, MAX_COMMAND_LINE_ARGS);
        if (argc == 0) continue;

        // Built-ins
        if (is_builtin(argv[0])) {
            run_builtin(argv);
            free_argv(argv, argc);
            continue;
        }

        // External command: parse > and &
        exec_plan_t plan; build_plan(argv, argc, &plan);
        if (!plan.args[0]) { free_argv(argv, argc); continue; }

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); free_argv(argv, argc); continue; }

        if (pid == 0) {
            // child process: set up redirection if any
            if (plan.redirect_out) {
                int fd = open(plan.redirect_out, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                if (fd < 0) { perror("open"); _exit(127); }
                if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2"); _exit(127); }
                close(fd);
            }
            execvp(plan.args[0], plan.args);
            perror("execvp");
            _exit(127);
        }

        // parent process
        if (plan.background) {
            printf("[bg] pid %d\n", pid);
            free_argv(argv, argc);
            continue;                         // no waiting
        }

        fg_child_pid = pid;
        alarm(TIMEOUT_SECONDS);               // Task 5 timeout
        int status = 0;
        if (waitpid(pid, &status, 0) < 0 && errno != EINTR) perror("waitpid");
        alarm(0);
        fg_child_pid = 0;

        free_argv(argv, argc);
    }
    return 0;
}
