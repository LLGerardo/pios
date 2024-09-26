#include <stdio.h>
#include <stdlib.h>
#include "rprintf.c"
#include "rprintf.h"

void putc(int data){
	uint32_t* mu_io = (uint32_t*) //address; //set pointer to register 
	*mu_io = (uint32_t)data; //write char to mu_io register
}

