#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


void *mem;
struct block *freelist;
struct block *allocated_list;
void push(struct block ** memlist, void * addr, int size);


/* Creates a struct block with address addr, of size size and pushes to the head of the given memlist. The memlist is a pointer of a pointer of a struct block.
*/
void add_to_head(struct block ** memlist, void *addr, int size) {
    struct block *new = (struct block *)malloc(sizeof(struct block));
    if (new == NULL) {
	perror("Could not allocate memory for list");
	exit(1);
    }
    new->addr = addr;
    new->size = size;
    new->next = *memlist;
    * memlist = new;
}


void *smalloc(unsigned int nbytes) {
   
    // Iterate through linked list freelist until block of sufficient size is found
    struct block *prev = NULL;
    struct block *current = freelist;
    while (current != NULL && current->size < nbytes) {
        prev = current;
	current = current->next;
    } 

    // No block of sufficient size could be found
    if (current == NULL) {
	return NULL;
    
    // current is a block of sufficient size, found
    } else {
	    
    	// Two cases for size of bytes: 
	// (1) if current->size > nbytes
	if (current->size > nbytes) { 
	   
	    // Add block to head of allocated_list using helper function add_to_head
	    add_to_head(&allocated_list, current->addr, nbytes);
	
	    // Modify freelist since memory of nbytes has been allocated
	    current->addr = (current->addr + nbytes);
            current->size = current->size - nbytes;

	    return allocated_list->addr;
	}
	// 2) current->size == nbytes 
	else {
            
	    // Add block to head of allocated_list using helper function add_to_head
	    add_to_head(&allocated_list, current->addr, nbytes);
            
            // Modify freelist since memory of nbytes has been allocated
            
	    // first block in freelist was allocated
	    if (prev == NULL) { 
		freelist = current->next;
		free(current);
	    } 

	    // block of memory allocated was found somewhere in freelist other than in the
	    // head position
	    else { 
		prev->next = current->next;
	    }
	    return allocated_list->addr;
	} 	    
    }
}


int sfree(void *addr) {
    
    // Iterate through linked list allocated_list until block with pointer addr is found
    struct block * prev_alloc = NULL;
    struct block * current_alloc = allocated_list;
    while (current_alloc != NULL && current_alloc->addr != addr) {
        prev_alloc = current_alloc;
	current_alloc = current_alloc->next;
    }
    
    // If current_alloc->next not NULL, then block it is pointing to is correct addr
    if (current_alloc != NULL) {
       
	// Remove block with pointer addr from allocated_list
	// if prev_alloc == NULL, then allocated_list moved to next block
        if (prev_alloc == NULL) {
            allocated_list = current_alloc->next;
        // Else, remove the current_alloc block from allocated_list
        } else {
            prev_alloc->next = current_alloc->next;
        }
	
        // Iterate through freelist until block with largest memory address smaller than addr is found
        struct block * prev_free = NULL;
        struct block * current_free = freelist; // freelist is not empty
        while (current_free != NULL && current_free->addr < addr) {
            prev_free = current_free;
	    current_free = current_free->next;
        }
           
	if (current_free != NULL) {
	    //if (current_free->addr > addr) {
	    if (prev_free == NULL) {
	        current_alloc->next = current_free;
	        freelist = current_alloc;
	    } else {
	    // Insert freed block before this memory address in freelist
	        prev_free->next = current_alloc;
	        current_alloc->next = current_free;	
	    }
	
	// Either freelist is empty, or every block of memory in freelist
	// has address smaller than addr  
	} else {
	    // freelist is empty
	    if (prev_free == NULL) {
		freelist = current_alloc;
	    } else {
	    // all addresses in freelist are smaller than addr
		prev_free->next = current_alloc;
	    }
	}    
        return 0;
    }
    return -1;
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }
    
    // freelist head is a pointer to allocated memory returned to mmap
    freelist = (struct block *) malloc(sizeof(struct block));
    if (freelist == NULL) {
	perror("Could not allocate memory for freelist");
	exit(1);
    }

    freelist->addr = mem;
    freelist->size = size;
    freelist->next = NULL;

    // allocated_list at mem_init does not have existing pointers to allocated memory
    allocated_list = NULL; 
}


void mem_clean(){
    
    // free each block in allocated_list
    struct block * current = allocated_list;
    while (current != NULL) {
    	struct block * temp = current->next;    
    	free(current);
	current = temp;
    }    
    
    // free each block in freelist
    current = freelist;
    while (current != NULL) {
	struct block * temp = current->next;
	free(current);
	current = temp;
    }
}


