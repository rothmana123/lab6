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
static int initialized = 0;

// include pointers for double linked list
struct mem_block
{
    size_t size;
    struct mem_block *prev;
    struct mem_block *next;
    char name[BLOCK_NAME_LEN]; // name for debugging
    // #ifdef DEBUG
    // char name[BLOCK_NAME_LEN];
    // #endif
};

//be clear on what this does
struct mem_block *find_suitable_block(size_t size);

// TODO: find out what the size of the free list should be by checking the
//       ALLOC_THRESH environment variable (note that getenv returns a
//       string, not a number). You can store the size so you don't need to
//       look it up every time.

//*be clear on what this does and it its needed...
/**
 * Initialize the allocator's threshold from environment variable if not already done
 */
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

/**
 * Remove and unmap the oldest block (tail) from the free list
 */
void remove_tail_block()
{
    if (free_list_tail == NULL)
        return;

    struct mem_block *oldest = free_list_tail;

    // Update the tail pointer
    free_list_tail = oldest->prev;

    // If we just removed the last element, update head too
    if (free_list_tail == NULL) {
        free_list_head = NULL;
    } else {
        // Otherwise update the new tail's next pointer
        free_list_tail->next = NULL;
    }

    free_list_size--;

    // TRACE("remove_tail_block(): Unmapping oldest block [%p]: %zu bytes", 
    //       oldest, oldest->size);

    // Now unmap the block
    if (munmap(oldest, oldest->size) == -1) {
        perror("munmap");
    }

}
/**
 * Add a block to the head of the free list
 */
void add_to_free_list(struct mem_block *block)
{
    //size_t block_size = block->size + sizeof(struct mem_block);
    block->prev = NULL;
    block->next = free_list_head;

    if (free_list_head != NULL)
    {
        free_list_head->prev = block;
    }

    //update the head to point to this new block
    free_list_head = block;

    // If list was empty, set tail too
    if (free_list_tail == NULL)
    {
        free_list_tail = block;
    }

    free_list_size++;
    TRACE("free(): Cached free block -- [%p]: %zu bytes", block, block->size);
    //LOG("malloc request. size: %zu, block size: %zu\n", size, block_size);
    //TRACE("Allocated block [%p]: %zu bytes", block, block_size);

    // Check if we need to remove a block because we've reached the threshold
    if (free_list_size > alloc_threshold) {
        remove_tail_block();
    }
}


/**
 * Find a suitable block in the free list that can accommodate the requested size
 * The search starts from the tail to maintain FIFO behavior
 */
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

            // TRACE("Reusing block from free list [%p], usable: %zu, requested: %zu",
            //       current, usable_size, size);
            return current;
        }

        current = current->prev; // keep moving toward head
    }

    // No suitable block found
    return NULL;
}

/**
 * Helper function to allocate memory with a name (for debugging)
 */
void *malloc_name(size_t size, const char *name) {
    // First allocate the memory using our regular malloc
    void *ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    // Get access to the memory block header
    struct mem_block *block = (struct mem_block *)ptr - 1;
    
    // Copy the name into the block's name field if provided
    if (name) {
        // Make sure not to write beyond the name buffer
        strncpy(block->name, name, BLOCK_NAME_LEN - 1);
        block->name[BLOCK_NAME_LEN - 1] = '\0'; // Ensure null termination
    } 
    
    // else {
    //     // Initialize to empty string if no name provided
    //     block->name[0] = '\0';
    // }
    
    return ptr;
}

/**
 * The malloc() function allocates size bytes and returns a pointer to the
 * allocated memory. The memory is not initialized.
 */
void *malloc(size_t size)
{
    initialize_threshold_if_needed();  // doing this in `free`, but it’s needed here too.

    size_t block_size = size + sizeof(struct mem_block);
    LOG("malloc request. size: %zu, block size: %zu\n", size, block_size);

    struct mem_block *block = find_suitable_block(size);
    if (block != NULL) {
        return block + 1;  // skip over metadata
    }

    //if no suitbale block found
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
    block->prev = NULL;
    block->next = NULL;
    block->name[0] = '\0';

    TRACE("Allocated block [%p]: %zu bytes", block, block_size);
    return block + 1;
}

/**
 * The free() function frees the memory space pointed to by ptr, which must
 * have been returned by a previous call to malloc() or related functions.
 */
void free(void *ptr)
{
    if (ptr == NULL) {
        /* Freeing a NULL pointer does nothing. */
        return;
    }

    initialize_threshold_if_needed(); //

    struct mem_block *block = (struct mem_block *) ptr - 1;
    LOG("free request. ptr: %p, size: %zu\n", block, block->size);
   

     // If the free list is at capacity, remove the oldest block before adding a new one
    if (free_list_size >= alloc_threshold)
    {
        remove_tail_block(); // FIFO: free up a slot
    }

    // Add the newly freed block to the head of the list
    add_to_free_list(block); 
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
 * ptr to size bytes.
 */
void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return malloc(size);
    }

    LOG("realloc request. ptr: %p, new size: %zu\n", ptr, size);

    struct mem_block *old_block = (struct mem_block *)ptr - 1;
    size_t old_size = old_block->size - sizeof(struct mem_block);

    // Check if the block can already accommodate the requested size
    if (old_size >= size)
    {
        return ptr;
    }

    // Allocate a new, larger block
    void *new_ptr = malloc(size);
    if (new_ptr == NULL)
    {
        return NULL; // malloc failed
    }
    // Copy old contents to the new block (only up to the old size)
    memcpy(new_ptr, ptr, old_size);

    // TRACE("Block resized: old [%p], new [%p], copied: %zu bytes", 
    //       old_block, ((struct mem_block *)new_ptr - 1), old_size);
    struct mem_block *new_block = (struct mem_block *)new_ptr - 1;
    TRACE("Resized block -- [%p]: %zu bytes -> %zu bytes",
        old_block, old_block->size, new_block->size);

    // Free the old block
    free(ptr);
 
    return new_ptr;

    //return ptr;
}

/**
 * Debug function to print the current state of the free list
 */
void print_memory()
{
    printf("=== Free List State ===\n");
    struct mem_block *cur = free_list_head;
    int index = 0;
    while (cur != NULL)
    {
        printf("Block %d at %p | size: %-6zu | name: %s\n",
               index++, (void *)cur, cur->size,
               cur->name[0] ? cur->name : "(unnamed)");
        cur = cur->next;
    }
    printf("=======================\n");
}

int main()
{
    setenv("ALLOC_THRESH", "3", 1); // Keep the free list short for testing

    ///code before implementing malloc_name
    // void *a = malloc(100);
    // strncpy(((struct mem_block *)a - 1)->name, "alpha", BLOCK_NAME_LEN);
    // void *b = malloc(200);
    // strncpy(((struct mem_block *)b - 1)->name, "beta", BLOCK_NAME_LEN);
    // void *c = malloc(300);
    // strncpy(((struct mem_block *)c - 1)->name, "gamma", BLOCK_NAME_LEN);

    //implementation with malloc_name:
    void *a = malloc_name(100, "alpha");
    void *b = malloc_name(200, "beta");
    void *c = malloc_name(300, "gamma");

    // Free the blocks in the order they were allocated
    free(a);
    free(b);
    free(c);

    // This should show gamma -> beta -> alpha in the free list (head to tail)
    print_memory();

    // Allocate another block to reuse from the list
    // void *d = malloc(150);
    // strncpy(((struct mem_block *)d - 1)->name, "gamma", BLOCK_NAME_LEN);
    void *d = malloc_name(150, "delta");
    printf("Reused or new block: %p (delta)\n", d);

    print_memory(); // Updated free list

    return 0;
}
