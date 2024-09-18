#ifndef LIST_H
#define LIST_H

struct list_element { //define list_element struct
    struct list_element *next;
    int data;
};

struct list_element c = {NULL, 0}; //next ptr is NULL, end of list
struct list_element b = {&c, 0}; //next ptr points to c
struct list_element a = {&b, 0}; //next ptr points to a
struct list_element *head = &a; 

//declare function prototype
list_add(struct list_element **list_head, struct list_element *new_element);
list_remove(struct list_element *element);

#endif // LIST_H