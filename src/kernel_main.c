
extern char __bss_start, __bss_end; //initialize variables globally

void clear_bss(void) { 
    char *begin = &__bss_start; //find starting address
    char *end = &__bss_end; //find ending address
    
    for (char *p = begin; p < end; p++) { //loop through the addresses in memory 
        *p = 0; //set values in bss to o
    }
}

void kernel_main() {
    clear_bss(); // clear bss before starting kernel
    while(1){
    }
}
