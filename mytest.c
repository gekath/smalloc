#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

#define SIZE 128 * 64


/* Interesting test case. 

Divides SIZE into 4 blocks and allocates smalloc each  SIZE / 4 block of memory
Tests 
 - ability for smalloc to handle adding to an empty allocated_list
 - ability for smalloc to handle adding to an allocated_list with existing blocks
 - ability for smalloc to handle when smalloc is called for num_bytes > available size in freelist
 - ability for freelist to handle when freelist has no memory blocks available
 - ability for freelist to handle when the size in freelist is equal to the number of bytes called by smalloc (when the last memory block at ptrs[3] is called), and freelist should free this memory block from the list rather than break it into chunks

 */
int main(void) {

    mem_init(SIZE);

    char *ptrs[10];
    int i;

    //Call smalloc 4 times 

    ptrs[0] = smalloc(SIZE * 2);

    for(i = 0; i <4; i++) {
        int num_bytes = SIZE / 4;

        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
    }

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
    printf("freeing %p result = %d\n", ptrs[0], sfree(ptrs[0]));
    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));
    printf("freeing %p result = %d\n", ptrs[2], sfree(ptrs[2]));
    printf("freeing %p result = %d\n", ptrs[3], sfree(ptrs[3]));
 
    // sfree already called for address at ptrs[1]. Case should be handled 
    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));    
    
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    mem_clean();
    return 0;
}
