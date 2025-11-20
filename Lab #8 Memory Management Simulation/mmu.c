#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "list.h"
#include "util.h"

void TOUPPER(char * arr){
    for (int i = 0; i < (int)strlen(arr); i++) {
        arr[i] = toupper(arr[i]);
    }
}

void get_input(char *args[], int input[][2], int *n, int *size, int *policy) 
{
    FILE *input_file = fopen(args[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    parse_file(input_file, input, n, size);
    fclose(input_file);
  
    TOUPPER(args[2]);
  
    if ((strcmp(args[2],"-F") == 0) || (strcmp(args[2],"-FIFO") == 0))
        *policy = 1;
    else if ((strcmp(args[2],"-B") == 0) || (strcmp(args[2],"-BESTFIT") == 0))
        *policy = 2;
    else if ((strcmp(args[2],"-W") == 0) || (strcmp(args[2],"-WORSTFIT") == 0))
        *policy = 3;
    else {
        printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
        exit(1);
    }
}

/*
 * Allocate memory for PID using a block from freelist, and insert into alloclist.
 * Respects policy:
 *   1 -> FIFO
 *   2 -> BESTFIT
 *   3 -> WORSTFIT
 */
void allocate_memory(list_t * freelist, list_t * alloclist, int pid, int blocksize, int policy) {
    // Find index of first block of size >= blocksize
    int idx = list_get_index_of_by_Size(freelist, blocksize);
    if (idx < 0) {
        printf("Error: Memory Allocation %d blocks\n", blocksize);
        return;
    }

    // Remove that block from freelist
    block_t *blk = list_remove_at_index(freelist, idx);
    if (blk == NULL) {
        printf("Error: Memory Allocation %d blocks\n", blocksize);
        return;
    }

    // Keep original end for fragment creation
    int original_end = blk->end;

    // Assign PID
    blk->pid = pid;

    // Resize allocated block
    blk->end = blk->start + blocksize - 1;

    // Add allocated block to ALLOC_LIST sorted by address
    list_add_ascending_by_address(alloclist, blk);

    // Handle leftover (fragment) if there is any space after blk->end
    if (blk->end < original_end) {
        block_t *fragment = (block_t *)malloc(sizeof(block_t));
        if (fragment == NULL) {
            fprintf(stderr, "Error: malloc failed for fragment\n");
            return;
        }

        fragment->pid   = 0;
        fragment->start = blk->end + 1;
        fragment->end   = original_end;

        // Insert fragment into FREE_LIST based on policy
        if (policy == 1) {            // FIFO
            list_add_to_back(freelist, fragment);
        } else if (policy == 2) {     // BESTFIT
            list_add_ascending_by_blocksize(freelist, fragment);
        } else if (policy == 3) {     // WORSTFIT
            list_add_descending_by_blocksize(freelist, fragment);
        }
    }
}

/*
 * Deallocate block belonging to PID from alloclist and put it back in freelist.
 */
void deallocate_memory(list_t * alloclist, list_t * freelist, int pid, int policy) { 
    // Find block by PID in ALLOC_LIST
    int idx = list_get_index_of_by_Pid(alloclist, pid);
    if (idx < 0) {
        printf("Error: Can't locate Memory Used by PID: %d\n", pid);
        return;
    }

    block_t *blk = list_remove_at_index(alloclist, idx);
    if (blk == NULL) {
        printf("Error: Can't locate Memory Used by PID: %d\n", pid);
        return;
    }

    // Mark as free
    blk->pid = 0;

    // Insert back into FREE_LIST based on policy
    if (policy == 1) {            // FIFO
        list_add_to_back(freelist, blk);
    } else if (policy == 2) {     // BESTFIT
        list_add_ascending_by_blocksize(freelist, blk);
    } else if (policy == 3) {     // WORSTFIT
        list_add_descending_by_blocksize(freelist, blk);
    }
}

list_t* coalese_memory(list_t * list){
    list_t *temp_list = list_alloc();
    block_t *blk;
  
    // sort the list in ascending order by address
    while ((blk = list_remove_from_front(list)) != NULL) {
        list_add_ascending_by_address(temp_list, blk);
    }
  
    // combine physically adjacent blocks
    list_coalese_nodes(temp_list);
        
    return temp_list;
}

void print_list(list_t * list, char * message){
    node_t *current = list->head;
    block_t *blk;
    int i = 0;
  
    printf("%s:\n", message);
  
    while (current != NULL){
        blk = current->blk;
        printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);
      
        if (blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else  
            printf("\n");
      
        current = current->next;
        i += 1;
    }
}

/* DO NOT MODIFY */
int main(int argc, char *argv[]) 
{
   int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;
  
   list_t *FREE_LIST = list_alloc();   // list that holds all free blocks (PID is always zero)
   list_t *ALLOC_LIST = list_alloc();  // list that holds all allocated blocks
   int i;
  
   if (argc != 3) {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
   }
  
   get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);
  
   // Allocate the initial partition of size PARTITION_SIZE
   block_t * partition = malloc(sizeof(block_t));   // create the partition meta data
   if (partition == NULL) {
       fprintf(stderr, "Error: malloc failed for initial partition\n");
       exit(1);
   }

   partition->start = 0;
   partition->end   = PARTITION_SIZE + partition->start - 1;
                                   
   list_add_to_front(FREE_LIST, partition);          // add partition to free list
                                   
   // Loop through all the input data and simulate a memory management policy
   for (i = 0; i < N; i++) {
       printf("************************\n");
       if (inputdata[i][0] != -99999 && inputdata[i][0] > 0) {
             printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
             allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
       }
       else if (inputdata[i][0] != -99999 && inputdata[i][0] < 0) {
             printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
             deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
       }
       else {
             printf("COALESCE/COMPACT\n");
             FREE_LIST = coalese_memory(FREE_LIST);
       }   
     
       printf("************************\n");
       print_list(FREE_LIST, "Free Memory");
       print_list(ALLOC_LIST,"\nAllocated Memory");
       printf("\n\n");
   }
  
   list_free(FREE_LIST);
   list_free(ALLOC_LIST);
  
   return 0;
}
