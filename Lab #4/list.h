// list/list.h
// Interface for a simple singly linked list.

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

/* element type (change if you need a different type) */
typedef int elem;

/* node and list definitions */
typedef struct node {
  elem value;
  struct node *next;
} node_t;

typedef struct list {
  node_t *head;
} list_t;

/* allocation / free */
list_t *list_alloc(void);
void    list_free(list_t *l);

/* printing / string */
void    list_print(list_t *l);
char   *listToString(list_t *l);

/* convenience: heap-alloc a node with value */
node_t *getNode(elem value);

/* length & membership */
int     list_length(list_t *l);
bool    list_is_in(list_t *l, elem value);

/* getters */
elem    list_get_elem_at(list_t *l, int index);   // 1-based; -1 on error
int     list_get_index_of(list_t *l, elem value); // -1 if not found

/* add */
void    list_add_to_back(list_t *l, elem value);
void    list_add_to_front(list_t *l, elem value);
void    list_add_at_index(list_t *l, elem value, int index); // 1-based

/* remove: return removed value or -1 on error */
elem    list_remove_from_back(list_t *l);
elem    list_remove_from_front(list_t *l);
elem    list_remove_at_index(list_t *l, int index); // 1-based

#endif // LIST_H
