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

#define BLOCK_NAME_LEN 32


// TODO: add static instance variables here.
static struct mem_block *free_list_head = NULL;
static struct mem_block *free_list_tail = NULL;
static size_t free_list_size = 0;
static size_t alloc_threshold = 100;

// include pointers for double linked list
struct mem_block
{
    size_t size;
    struct mem_block *prev;
    struct mem_block *next;
    char name[BLOCK_NAME_LEN]; // name for debugging
};

//be clear on what this does
struct mem_block *find_suitable_block(size_t size);

// TODO: find out what the size of the free list should be by checking the
//       ALLOC_THRESH environment variable (note that getenv returns a
//       string, not a number). You can store the size so you don't need to
//       look it up every time.
// Moving this here so it can be accessed elsewhere...but should it be insid free() ???
static int initialized = 0;


//*be clear on what this does and it its needed...
void initialize_threshold_if_needed()
{
    if (!initialized)
    {
        const char *env = getenv("ALLOC_THRESH");
        if (env != NULL)
        {
            alloc_threshold = atoi(env); // convert to int
        }
        initialized = 1;
    }
}

// Helper Function to Add to Free List Head
void add_to_free_list(struct mem_block *block)
{
    block->prev = NULL;
    block->next = free_list_head;

    if (free_list_head != NULL)
    {
        free_list_head->prev = block;
    }
    free_list_head = block;

    // If list was empty, set tail too
    if (free_list_tail == NULL)
    {
        free_list_tail = block;
    }

    free_list_size++;
    TRACE("Block added to free list head [%p], new size: %zu", block, free_list_size);
}

//this also could be in free, moving outside...
// TODO: if the list has run out of space, unmap the oldest block in the
//       list to make space for the block that was just freed. Take note
//       that the code below is unmapping the block that was just freed, so
//       you will need to change it.
void remove_tail_block()
{
    if (free_list_tail == NULL)
        return;

    struct mem_block *oldest = free_list_tail;

    if (oldest->prev != NULL)
    {
        oldest->prev->next = NULL;
        free_list_tail = oldest->prev;
    }
    else
    {
        // List had only one item
        free_list_head = NULL;
        free_list_tail = NULL;
    }

    free_list_size--;

    if (munmap(oldest, oldest->size) == -1)
    {
        perror("munmap");
    }
    else
    {
        TRACE("Unmapped oldest block from tail [%p]: %zu bytes", oldest, oldest->size);
    }
}

struct mem_block *find_suitable_block(size_t size)
{
    struct mem_block *current = free_list_tail;

    while (current != NULL)
    {
        size_t usable_size = current->size - sizeof(struct mem_block);
        if (usable_size >= size)
        {
            // Found a usable block — unlink it from the list
            if (current->prev)
            {
                current->prev->next = current->next;
            }
            else
            {
                // current was head
                free_list_head = current->next;
            }

            if (current->next)
            {
                current->next->prev = current->prev;
            }
            else
            {
                // current was tail
                free_list_tail = current->prev;
            }

            current->prev = current->next = NULL;
            free_list_size--;

            TRACE("Reusing block from free list [%p], usable: %zu, requested: %zu",
                  current, usable_size, size);
            return current;
        }

        current = current->prev; // keep moving toward head
    }

    // No suitable block found
    return NULL;
}

//unclear on what this method does - maybe delete
// void *malloc_name(size_t size, const char *name) {
//     void *ptr = malloc(size);
//     if (ptr == NULL)
//         return NULL;

//     struct mem_block *block = (struct mem_block *)ptr - 1;
//     strncpy(block->name, name, BLOCK_NAME_LEN - 1);
//     block->name[BLOCK_NAME_LEN - 1] = '\0';
//     return ptr;
// }

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
    initialize_threshold_if_needed();  // You were doing this in `free`, but it’s needed here too.

    struct mem_block *block = find_suitable_block(size);
    if (block != NULL) {
        return block + 1;  // skip over metadata
    }

     block = mmap(
        NULL,
        block_size,
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
    initialize_threshold_if_needed(); //

    

    if (ptr == NULL) {
        /* Freeing a NULL pointer does nothing. */
        return;
    }

    struct mem_block *block = (struct mem_block *) ptr - 1;
    LOG("free request. ptr: %p, size: %zu\n", block, block->size);
    // size_t block_size = block->size;
    // LOG("free request. ptr: %p, size: %zu\n", block, block_size);


    // TODO: find out what the size of the free list should be by checking the
    //       ALLOC_THRESH environment variable (note that getenv returns a
    //       string, not a number). You can store the size so you don't need to
    //       look it up every time.
    //*this was moved out of this function, below find suitable block

    // TODO: if there is space in our free list, add the block to the head of
    //       the list instead of unmapping it.
    //wait, why would the free list run out of space???
    if (free_list_size >= alloc_threshold)
    {
        remove_tail_block(); // FIFO: free up a slot
    }

    add_to_free_list(block); // insert the newly freed block...insert to where tho?

    //*tHis one also moved out, now under add to free
    // TODO: if the list has run out of space, unmap the oldest block in the
    //       list to make space for the block that was just freed. Take note
    //       that the code below is unmapping the block that was just freed, so
    //       you will need to change it.

     // always try to add the new block to the list
    // if the list is full, remove and unmpa the oldest block first to stay under threshold
    // do not munmap() the block we are adding
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

    struct mem_block *old_block = (struct mem_block *)ptr - 1;
    size_t old_size = old_block->size - sizeof(struct mem_block);
    // TODO: check if the block can already accommodate the requested size.
    //       if it can, there's no need to do anything
    if (old_size >= size)
    {
        return ptr;
    }

    // TODO: if the block can't accommodate the requested size, then
    //       we should allocate a new block, copy the old data there,
    //       and then free the old block.
    void *new_ptr = malloc(size);
    if (new_ptr == NULL)
    {
        return NULL; // malloc failed
    }
    // Copy old contents to the new block (only up to the old size)
    memcpy(new_ptr, ptr, old_size);

    // Free the old block
    free(ptr);
 
    return new_ptr;

    //return ptr;
}

// If ptr == NULL, treat like malloc()
// If current block is big enough, return it unchanged.
// If it’s not big enough:
// Allocate a new block
// Copy over the existing data
// Free the old block
// Return new block

void print_memory()
{
    printf("=== Free List State ===\n");
    struct mem_block *cur = free_list_head;
    while (cur != NULL)
    {
        printf("Block at %p | size: %zu | name: %s\n",
               (void *)cur, cur->size, cur->name[0] ? cur->name : "(unnamed)");
        cur = cur->next;
    }
    printf("=======================\n");
}

int main()
{
    setenv("ALLOC_THRESH", "3", 1); // Keep the free list short for testing

    
    void *a = malloc(100);
    strncpy(((struct mem_block *)a - 1)->name, "alpha", BLOCK_NAME_LEN);
    void *b = malloc(200);
    strncpy(((struct mem_block *)b - 1)->name, "beta", BLOCK_NAME_LEN);
    void *c = malloc(300);
    strncpy(((struct mem_block *)c - 1)->name, "gamma", BLOCK_NAME_LEN);

    // Free the blocks in the order they were allocated
    free(a);
    free(b);
    free(c);

    // This should show gamma -> beta -> alpha in the free list (head to tail)
    print_memory();

    // Allocate another block to reuse from the list
    void *d = malloc(150);
    strncpy(((struct mem_block *)d - 1)->name, "gamma", BLOCK_NAME_LEN);
    printf("Reused or new block: %p (delta)\n", d);

    print_memory(); // Updated free list

    return 0;
}
 // if (munmap(block, block->size) == -1) {
    //     perror("munmap");
    // } else {
    //     TRACE("Unmapped block -- [%p]: %zu bytes", block, block_size);
    // }