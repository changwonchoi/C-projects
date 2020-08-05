////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Semester:         CS 354 Fall 2017
//
// Author:           Chang Won Choi
// Email:            cchoi25@wisc.edu
// CS Login:         cchoi
////////////////////////////////////////////////////////////////////////////////

//Global variable last_block was added

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct blk_hdr {   	    

    int size_status;
  
    /*
    * Size of the block is always a multiple of 8
    * => last two bits are always zero - can be used to store other information
    *
    * LSB -> Least Significant Bit (Last Bit)
    * SLB -> Second Last Bit 
    * LSB = 0 => free block
    * LSB = 1 => allocated/busy block
    * SLB = 0 => previous block is free
    * SLB = 1 => previous block is allocated/busy
    * 
    * When used as the footer the last two bits should be zero
    */

    /*
    * Examples:
    * 
    * For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an additional 4 bytes for header)
    * Header:
    * If the previous block is allocated, size_status should be set to 27
    * If the previous block is free, size_status should be set to 25
    * 
    * For a free block of size 24 bytes (including 4 bytes for header + 4 bytes for footer)
    * Header:
    * If the previous block is allocated, size_status should be set to 26
    * If the previous block is free, size_status should be set to 24
    * Footer:
    * size_status should be 24
    * 
    */

} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_block = NULL;

// Global variable - Total available memory 
int total_mem_size = 0;

// Global variable - Pointer pointing to the last possible header block(based only on memory size) in the heap
blk_hdr *last_block = NULL;

/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */
void* Mem_Alloc(int size) {   	
    last_block = (blk_hdr*) ((char*)first_block + total_mem_size - 9);
   
    if (size <= 0) {
        return NULL;
    }
	//size must be positive

    blk_hdr *current;
    blk_hdr *footer;
    blk_hdr *update;
    
	current = first_block;
    
	//update size to meet the requirement
    int tot_size = size + 4;
    if (tot_size < 8) {
        tot_size = 8;
    }
    else if (tot_size % 8 != 0) {
        tot_size = ((tot_size / 8) + 1) * 8;
    }

    if (tot_size >= total_mem_size) {
        return NULL;
    }
	// tot_size should be less then the total available memory
	
    int status = current->size_status;
    int prev = 2; //initially prev is 2 because this is the first block
    int busy = status & 1;
    int memory = status - prev - busy;
    int min = -1; //used for best fit
    blk_hdr *address; //used for best fit

    while ((int)current <= (int)last_block) {        
        if (memory == tot_size && (status & 1) == 0) {
            current->size_status = tot_size + 3;
			//if the memory fits nicely, just allocate it now.
            update = (blk_hdr*) ((char*)current + memory);
			//get next block
            if ((int)update < (int)last_block) {
                update->size_status += 2;
            }//if next block is not the end update its value

        
            return (blk_hdr*) ((char*)current + 4);
        }
        
		//search for best fit block
        if ((memory - tot_size) >= 8 && min == -1 && (status & 1) == 0) {
            min = memory - tot_size;
            address = current;
        }
        else if ((memory - tot_size) >= 8 && (memory - tot_size) < min && (status & 1) == 0) {
            min = memory - tot_size;
            address = current;
        }

		//update current to next block
        current = (blk_hdr*) ((char*)current + memory);
        status = current->size_status;
        prev = busy * 2;
        busy = status & 1;
        memory = status - prev - busy;
    }
    
    if (min == -1) {
        return NULL;
    }
    else {
        status = address->size_status;
        memory = status - (status & 1) - (status & 2);

        address->size_status = tot_size + 3; // because we know that the prev block must be assgined and this block is busy
        update = (blk_hdr*) ((char*)address + tot_size);
		//move to next block created because free block was splitted.
        if ((int)update <= (int)last_block) {
            update->size_status = memory - tot_size + 2;
            footer = (blk_hdr*) ((char*)update + (memory - tot_size) - 4);
            footer->size_status = memory - tot_size;
        }//update the newly created free block if it is not the end of the heap
        
        return (blk_hdr*) ((char*)address + 4);
    }
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL or not within the range of memory allocated by Mem_Init()
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */
int Mem_Free(void *ptr) {   
    last_block = (blk_hdr*) ((char*)first_block + total_mem_size - 9);
	
    if (ptr == NULL || ((int)ptr % 8) != 0) {
        return -1;
    }//ptr should align and not be NULL

    blk_hdr *header;
    blk_hdr *footer;
    blk_hdr *prev;
    blk_hdr *post;
    int status;
    int memory;
    int prev_memory;
    int post_memory;
    int prev_busy;
    int post_busy;
     
    header = (blk_hdr*) ((char*)ptr - 4);
    status = header->size_status;
    memory = status - (status & 1) - (status & 2);
    prev_busy = (status & 2);
    post = (blk_hdr*) ((char*)header + memory);
    post_busy = ((post->size_status) & 1);
    
    if ((int)header > (int)last_block || (status & 1) == 0) {
        return -1;
    }//ptr should be within the scope of heap and the block should not be free

    if ((int)header == (int)first_block) {
        if (post_busy == 1) {
            header->size_status -= 1;
            footer = (blk_hdr*) ((char*)header + memory - 4);
            footer->size_status = memory;
            if ((int)post <= (int)last_block) {
                post->size_status -= 2;
            }//update post block if it is in the scope of the heap
        
            return 0;
        }//if next block is busy, just update header and create footer
        else {
            post_memory = (post->size_status) - ((post->size_status) & 1) - ((post->size_status) & 2);
            memory += post_memory;
            header->size_status = memory + 2;
            footer = (blk_hdr*) ((char*)header + memory - 4);
            footer->size_status = memory;
        
            return 0;
        }//combine next free block and update header and footer.
    }//Case for when ptr is pointing to first block
    else if (prev_busy == 2 && post_busy == 1) {
        header->size_status = memory + 2;
        footer = (blk_hdr*) ((char*)header + memory - 4);
        footer->size_status = memory;
        post->size_status -= 2;
        
        return 0;
    }//Case when both prev and post blocks are busy
    else if (prev_busy == 2) {
        post_memory = (post->size_status) - ((post->size_status) & 1) - ((post->size_status) & 2);
        memory += post_memory;
        header->size_status = memory + 2;
        footer = (blk_hdr*) ((char*)header + memory - 4);
        footer->size_status = memory;
        
        return 0;
    }//Case for when only prev block is busy
    else if (post_busy == 1) {
        prev = (blk_hdr*) ((char*)header - 4);
        prev_memory = prev->size_status;
        header = (blk_hdr*) ((char*)header - prev_memory);//go to prev block
        memory += prev_memory;
        header->size_status = memory + 2;
        footer = (blk_hdr*) ((char*)header + memory - 4);
        footer->size_status = memory;
        post->size_status -= 2;//update post block
        
        return 0;
    }//Case for when only post block is busy
    else {
        prev = (blk_hdr*) ((char*)header - 4);
        prev_memory = prev->size_status;
        post_memory = (post->size_status) - ((post->size_status) & 1) - ((post->size_status) & 2);
        header = (blk_hdr*) ((char*)header - prev_memory);
        memory = memory + prev_memory + post_memory;
        header->size_status = memory + 2;
        footer = (blk_hdr*) ((char*)header + memory - 4);
        footer->size_status = memory;
		
        return 0;
    }//Case for when both prev and post are free
}

/*
 * For testing purpose
 * To verify whether a block is double word aligned
 */
void *start_pointer;

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion) {   	   
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    static int allocated_once = 0;
  
    if(0 != allocated_once) {
        fprintf(stderr,"Error:mem.c: Mem_Init has allocated space during a previous call\n");
        return -1;
    }
    if(sizeOfRegion <= 0) {
        fprintf(stderr,"Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if(-1 == fd){
        fprintf(stderr,"Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr,"Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
    allocated_once = 1;
 
    // for double word alignement
    alloc_size -= 4;
 
    // Intialising total available memory size
    total_mem_size = alloc_size;

    // To begin with there is only one big free block
    // initialize heap so that first block meets double word alignement requirement
    first_block = (blk_hdr*) space_ptr + 1;
    start_pointer = space_ptr;
  
    // Setting up the header
    first_block->size_status = alloc_size;

    // Marking the previous block as busy
    first_block->size_status += 2;

    // Setting up the footer
    blk_hdr *footer = (blk_hdr*) ((char*)first_block + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header)(including the header/footer)
 */ 
void Mem_Dump() {   	   
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blk_hdr *current = first_block;
    counter = 1;

    int busy_size = 0;
    int free_size = 0;
    int is_busy = -1;

    fprintf(stdout,"************************************Block list***********************************\n");
    fprintf(stdout,"No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout,"---------------------------------------------------------------------------------\n");
  
    while (current < (blk_hdr*) ((char*)first_block + total_mem_size)) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => busy block
            strcpy(status,"Busy");
            is_busy = 1;
            t_size = t_size - 1;
        }
        else {
            strcpy(status,"Free");
            is_busy = 0;
        }

        if (t_size & 2) {
            strcpy(p_status,"Busy");
            t_size = t_size - 2;
        }
        else 
            strcpy(p_status,"Free");

        if (is_busy) 
            busy_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout,"%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, p_status, 
                    (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blk_hdr*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout,"---------------------------------------------------------------------------------\n");
    fprintf(stdout,"*********************************************************************************\n");
    fprintf(stdout,"Total busy size = %d\n", busy_size);
    fprintf(stdout,"Total free size = %d\n", free_size);
    fprintf(stdout,"Total size = %d\n", busy_size + free_size);
    fprintf(stdout,"*********************************************************************************\n");
    fflush(stdout);

    return;
}
