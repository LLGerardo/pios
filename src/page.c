#include page.h
#define NUM_Pages 128

struct ppage physical_page_array[NUM_Pages]; // 128 pages, each 2mb in length covers 256 megs of memory

struct ppage *free_list_head = NULL; 

void init_pfa_list(void) {
    //todo create a function that initializes the 
    //list of of free physical page structures:
    for (int i = 0; i < NUM_PAGES; i++) {
        physical_page_array[i].physical_addr = (void *)(i * 2 * 1024 * 1024); // 2MB per page
        physical_page_array[i].prev = NULL;
        physical_page_array[i].next = free_list_head;
        if (free_list_head) {
            free_list_head->prev = &physical_page_array[i];
        }
        free_list_head = &physical_page_array[i];
    }
}

//todo  create the following functions to allocate and free physical pages:
struct ppage *allocate_physical_pages(unsigned int npages) {
    if (npages == 0 || free_list_head == NULL) {
        return NULL;
    }

    struct ppage *allocd_list = free_list_head;
    struct ppage *current = free_list_head;

    for (unsigned int i = 0; i < npages - 1; i++) {
        if (current->next == NULL) {
            return NULL; // Not enough free pages
        }
        current = current->next;
    }

    // Update free list
    free_list_head = current->next;
    if (free_list_head) {
        free_list_head->prev = NULL;
    }

    // Prepare allocated list
    current->next = NULL;
    allocd_list->prev = NULL;

    return allocd_list;
}

void free_physical_pages(struct ppage *ppage_list){
    if (ppage_list == NULL) {
        return;
    }

    // Find the last page in the list to be freed
    struct ppage *last = ppage_list;
    while (last->next != NULL) {
        last = last->next;
    }

    // Connect the list to be freed with the existing free list
    last->next = free_list_head;
    if (free_list_head) {
        free_list_head->prev = last;
    }

    // Update free list head
    free_list_head = ppage_list;
    ppage_list->prev = NULL;
}