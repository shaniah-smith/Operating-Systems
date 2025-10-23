// list/list.c
// Implementation for singly linked list.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

list_t *list_alloc(void) {
  list_t *new_list = (list_t *)malloc(sizeof(list_t));
  if (!new_list) return NULL;
  new_list->head = NULL;
  return new_list;
}

node_t *getNode(elem value) {
  node_t *n = (node_t *)malloc(sizeof(node_t));
  if (!n) return NULL;
  n->value = value;
  n->next  = NULL;
  return n;
}

void list_add_to_front(list_t *l, elem value) {
  node_t *node = getNode(value);
  node->next = l->head;
  l->head = node;
}

void list_add_to_back(list_t *l, elem value) {
  if (l->head == NULL) { list_add_to_front(l, value); return; }
  node_t *ptr = l->head;
  while (ptr->next) ptr = ptr->next;
  ptr->next = getNode(value);
}

void list_add_at_index(list_t *l, elem value, int index) {
  if (index <= 1 || l->head == NULL) { list_add_to_front(l, value); return; }
  int pos = 1;
  node_t *walker = l->head, *prev = NULL;
  while (walker) {
    if (pos == index) {
      node_t *node = getNode(value);
      node->next = walker;
      prev->next = node;
      return;
    }
    prev = walker;
    walker = walker->next;
    pos++;
  }
  /* append if index is just after end */
  if (pos == index) list_add_to_back(l, value);
}

elem list_remove_from_front(list_t *l) {
  if (!l->head) return -1;
  node_t *tmp = l->head;
  l->head = tmp->next;
  elem val = tmp->value;
  free(tmp);
  return val;
}

elem list_remove_from_back(list_t *l) {
  if (!l->head) return -1;
  node_t *ptr = l->head;
  if (!ptr->next) {
    elem v = ptr->value;
    l->head = NULL;
    free(ptr);
    return v;
  }
  node_t *prev = NULL;
  while (ptr->next) { prev = ptr; ptr = ptr->next; }
  elem v = ptr->value;
  prev->next = NULL;
  free(ptr);
  return v;
}

elem list_remove_at_index(list_t *l, int index) {
  if (index <= 1) return list_remove_from_front(l);
  int pos = 1;
  node_t *ptr = l->head, *prev = NULL;
  while (ptr) {
    if (pos == index) {
      elem v = ptr->value;
      prev->next = ptr->next;
      free(ptr);
      return v;
    }
    prev = ptr;
    ptr = ptr->next;
    pos++;
  }
  return -1;
}

int list_length(list_t *l) {
  int count = 0;
  for (node_t *p = l->head; p; p = p->next) count++;
  return count;
}

bool list_is_in(list_t *l, elem value) {
  for (node_t *p = l->head; p; p = p->next)
    if (p->value == value) return true;
  return false;
}

elem list_get_elem_at(list_t *l, int index) {
  int pos = 1;
  for (node_t *p = l->head; p; p = p->next, pos++)
    if (pos == index) return p->value;
  return -1;
}

int list_get_index_of(list_t *l, elem value) {
  int pos = 1;
  for (node_t *p = l->head; p; p = p->next, pos++)
    if (p->value == value) return pos;
  return -1;
}

void list_print(list_t *l) {
  if (!l->head) { printf("NULL\n"); return; }
  node_t *p = l->head;
  printf("%d", p->value); p = p->next;
  while (p) { printf("->%d", p->value); p = p->next; }
  printf("->NULL\n");
}

char *listToString(list_t *l) {
  char *buf = (char *)malloc(1024);
  buf[0] = '\0';
  char tbuf[20];
  node_t *p = l->head;

  if (!p) { strcat(buf, "NULL"); return buf; }

  sprintf(tbuf, "%d", p->value);
  strcat(buf, tbuf);
  p = p->next;

  while (p) {
    sprintf(tbuf, "->%d", p->value);
    strcat(buf, tbuf);
    p = p->next;
  }
  strcat(buf, "->NULL");
  return buf;
}

void list_free(list_t *l) {
  node_t *p = l->head;
  while (p) {
    node_t *nxt = p->next;
    free(p);
    p = nxt;
  }
  l->head = NULL;
  /* If you own the list object itself and it's not reused:
     free(l);  // optional */
}
