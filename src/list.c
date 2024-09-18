#include "list.h"

//add elements from a linked list
void list_add(struct list_element **list_head, struct list_element *new_element) {
    if (*list_head == NULL) {
        *list_head = new_element;
        new_element->next = NULL;
    } else {
        new_element->next = *list_head;
        *list_head = new_element;
    }
}

/remove elements from a linked list
void list_remove(struct list_element *element) {
    if (element == NULL || element->next == NULL) {
        return;
    }

    struct list_element *next = element->next;
    *element = *next;
}