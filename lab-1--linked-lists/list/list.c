// list/list.c
// 
// Implementation for linked list.
//
// <Author>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* new_list = (list_t *) malloc(sizeof(list_t)); 
  new_list->head = NULL;
  return new_list;
}

void list_add_to_front(list_t *l, elem value) {
  node_t* node = (node_t *) malloc(sizeof(node_t));
  node->value = value;
  node->next = l->head;
  l->head = node;
}

void list_add_to_back(list_t *l, elem value) {
  if (l->head == NULL) { 
    list_add_to_front(l, value);
    return;
  }
  node_t *ptr = l->head;
  while (ptr->next != NULL) {
    ptr = ptr->next;
  }
  node_t* node = (node_t *) malloc(sizeof(node_t));
  node->value = value;
  node->next = NULL;
  ptr->next = node;
}

void list_add_at_index(list_t *l, elem value, int index) {
  if (index == 1) {
    list_add_to_front(l, value);
    return;
  }
  int pos = 1;
  node_t *walker = l->head;
  node_t *prev = NULL;

  while (walker != NULL) {
    if (pos == index) {
      node_t* node = (node_t *) malloc(sizeof(node_t));
      node->value = value;
      node->next = walker;
      prev->next = node;
      return;
    }
    prev = walker;
    walker = walker->next;
    pos++;
  }

  if (pos == index) {
    list_add_to_back(l, value);
  }
}

elem list_remove_from_front(list_t *l) {
  if (l->head == NULL) return -1;

  node_t *temp = l->head;
  l->head = temp->next;
  elem val = temp->value;
  free(temp);
  return val;
}

elem list_remove_from_back(list_t *l) {
  if (l->head == NULL) return -1;

  node_t *ptr = l->head;
  if (ptr->next == NULL) {
    elem val = ptr->value;
    l->head = NULL;
    free(ptr);
    return val;
  }

  node_t *prev = NULL;
  while (ptr->next != NULL) {
    prev = ptr;
    ptr = ptr->next;
  }
  elem val = ptr->value;
  prev->next = NULL;
  free(ptr);
  return val;
}

elem list_remove_at_index(list_t *l, int index) {
  if (index == 1) return list_remove_from_front(l);

  int pos = 1;
  node_t *ptr = l->head;
  node_t *prev = NULL;

  while (ptr != NULL) {
    if (pos == index) {
      elem val = ptr->value;
      prev->next = ptr->next;
      free(ptr);
      return val;
    }
    prev = ptr;
    ptr = ptr->next;
    pos++;
  }
  return -1;
}

int list_length(list_t *l) { 
  int count = 0;
  node_t* ptr = l->head;
  while (ptr != NULL) {
    ptr = ptr->next;
    count++;
  }
  return count; 
}

bool list_is_in(list_t *l, elem value) {
  node_t *runner = l->head;
  while (runner != NULL) {
    if (runner->value == value) return true;
    runner = runner->next;
  }
  return false; 
}

elem list_get_elem_at(list_t *l, int index) {
  int pos = 1;
  node_t *ptr = l->head;
  while (ptr != NULL) {
    if (pos == index) return ptr->value;
    ptr = ptr->next;
    pos++;
  }
  return -1; 
}

int list_get_index_of(list_t *l, elem value) {
  int pos = 1;
  node_t *ptr = l->head;
  while (ptr != NULL) {
    if (ptr->value == value) return pos;
    ptr = ptr->next;
    pos++;
  }
  return -1; 
}

void list_print(list_t *l) {
  if (l->head == NULL) {
    printf("NULL\n");
    return;
  }
  node_t *ptr = l->head;
  printf("%d", ptr->value);
  ptr = ptr->next;
  while (ptr != NULL) {
    printf("->%d", ptr->value);
    ptr = ptr->next;
  }
  printf("->NULL\n");
}

char * listToString(list_t *l) {
  char* buf = (char *) malloc(sizeof(char) * 1024);
  buf[0] = '\0';
  char tbuf[20];
  node_t* ptr = l->head;

  if (l->head != NULL) {
    sprintf(tbuf, "%d", ptr->value);
    strcat(buf, tbuf);
    ptr = ptr->next;
  } else {
    strcat(buf, "NULL");
    return buf;
  }

  while (ptr != NULL) {
    sprintf(tbuf, "->%d", ptr->value);
    strcat(buf, tbuf);
    ptr = ptr->next;
  }
  strcat(buf, "->NULL");
  return buf;
}

void list_free(list_t *l) {
  node_t *ptr = l->head;
  while (ptr != NULL) {
    node_t *next_node = ptr->next;
    free(ptr);
    ptr = next_node;
  }
  l->head = NULL;
}
