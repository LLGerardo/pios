#include page.h

struct ppage physical_page_array[128]; // 128 pages, each 2mb in length covers 256 megs of memory

void init_pfa_list(void) {
    //todo create a function that initializes the 
    //list of of free physical page structures:
}

//todo  create the following functions to allocate and free physical pages:
struct ppage *allocate_physical_pages(unsigned int npages) {

}

void free_physical_pages(struct ppage *ppage_list){

}