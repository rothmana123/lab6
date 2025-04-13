/**
 * @file allocator.c
 * @author Your name here
 *
 * Implements a C memory allocator with a FIFO free list cache. Users can set
 * ALLOC_THRESH to determine how many free blocks are cached in the free list.
 */

#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "logger.h"
#include "trace.h"

// TODO: add static instance variables here. Since you are maintaining a
//       doubly-linked list, you'll want to store the head, tail, etc.
//       You will also want to create functions that modify your linked
//       list so your code is easier to test.
static struct mem_block *free_list_head = NULL;
static struct mem_block *free_list_tail = NULL;
static size_t free_list_size = 0;
static size_t alloc_threshold = 100;


//include pointers for double linked list
struct mem_block {
    size_t size;
    struct mem_block *prev;
    struct mem_block *next;
};

/**
 * The malloc() function allocates size bytes and returns a pointer to the
 * allocated memory. The memory is not initialized.
 */
void *malloc(size_t size)
{
    size_t block_size = size + sizeof(struct mem_block);
    LOG("malloc request. size: %zu, block size: %zu\n", size, block_size);

    // TODO: scan through your doubly-linked free list, starting at the tail,
    //       and return a viable block if you find one. If no viable blocks are
    //       in the list, you can mmap a new block.

    struct mem_block *block = mmap(
        NULL,
        block_size, //system call that does a lot...creates a chunk of memory for ourselves
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0);

    if (block == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    block->size = block_size;
    TRACE("Allocated block [%p]: %zu bytes", block, block_size);
    return block + 1;
}

/**
 * The free() function frees the memory space pointed to by ptr, which must
 * have been returned by a previous call to malloc() or related functions.
 * Otherwise, or if ptr has already been freed, undefined behavior occurs.
 * If ptr is NULL, no operation is performed.
 */
void free(void *ptr)
{
    if (ptr == NULL) {
        /* Freeing a NULL pointer does nothing. */
        return;
    }

    struct mem_block *block = (struct mem_block *) ptr - 1;
    LOG("free request. ptr: %p, size: %zu\n", block, block->size);
    size_t block_size = block->size;

    // TODO: find out what the size of the free list should be by checking the
    //       ALLOC_THRESH environment variable (note that getenv returns a
    //       string, not a number). You can store the size so you don't need to
    //       look it up every time.

    // TODO: if there is space in our free list, add the block to the head of
    //       the list instead of unmapping it.

    // TODO: if the list has run out of space, unmap the oldest block in the
    //       list to make space for the block that was just freed. Take note
    //       that the code below is unmapping the block that was just freed, so
    //       you will need to change it.

    if (munmap(block, block->size) == -1) {
        perror("munmap");
    } else {
        TRACE("Unmapped block -- [%p]: %zu bytes", block, block_size);
    }
}

/**
 * The  calloc()  function  allocates  memory  for an array of nmemb elements
 * of size bytes each and returns a pointer to the allocated memory.  The
 * memory is set to zero.
 */
void *calloc(size_t nmemb, size_t size)
{
    LOG("calloc request. size: %zu memb, %zu bytes each\n", nmemb, size);
    void *ptr = malloc(nmemb * size);
    memset(ptr, 0, nmemb * size);
    return ptr;
}

/**
 * The realloc() function changes the size of the memory block pointed to by
 * ptr to size bytes.  The contents of the memory will be unchanged in the
 * range from the start of the region up to the minimum of the old and new
 * sizes.  If the new size is larger than the old size, the added memory will
 * not be initialized.
 */
void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return malloc(size);
    }

    LOG("realloc request. ptr: %p, new size: %zu\n", ptr, size);
    // TODO: check if the block can already accommodate the requested size.
    //       if it can, there's no need to do anything

    // TODO: if the block can't accommodate the requested size, then
    //       we should allocate a new block, copy the old data there,
    //       and then free the old block.
 
    return ptr;
}
  
