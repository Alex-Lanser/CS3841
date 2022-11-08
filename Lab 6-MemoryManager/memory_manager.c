#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "memory_manager.h"

/*
 * Using static causes the compiler to
 * limit visibility of the variables to this file only
 * This can be used to simulate 'private' variables in c
 */
static int allocation_count;
static int fragment_count;

pthread_mutex_t malloc_mutex;

struct block
{
	void *start;
	int size;
	int type; // 0 is free, 1 is allocated
	struct block *next;
} block;

struct block *top;

/* TODO Define additional structure definitions here */

/* mmInit()
 *     Initialize the memory manager to "manage" the given location
 *     in memory with the specified size.
 *         Parameters: start - the start of the memory to manage
 *                     size - the size of the memory to manage
 *         Returns: void
 */
void mmInit(void *start, int size)
{
	struct block *b = malloc(sizeof(block));
	b->start = start;
	b->size = size;
	b->type = 0;
	b->next = NULL;
	top = b;
	pthread_mutex_init(&malloc_mutex, NULL);
	allocation_count = 0;
	fragment_count = 1;
}

/* mmDestroy()
 *     Cleans up any storage used by the memory manager
 *     After a call to mmDestroy:
 *         all allocated spaces become invalid
 *         future allocation attempts fail
 *         future frees result in segmentation faults
 *     NOTE: after a call to mmDestroy a call to mmInit
 *           reinitializes the memory manager to allow allocations
 *           and frees
 *         Parameters: None
 *         Returns: void
 */
void mmDestroy()
{
	pthread_mutex_lock(&malloc_mutex);
	pthread_mutex_unlock(&malloc_mutex);
}

/* mymalloc_ff()
 *     Requests a block of memory be allocated using
 *         first fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytes - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void *mymalloc_ff(int nbytes)
{
	pthread_mutex_lock(&malloc_mutex);
	struct block *p = top;
	while (p != NULL)
	{
		if (p->size >= nbytes && p->type == 0) // There is room for block and it is free space
		{
			break;
		}
		p = p->next;
	}
	if (p == NULL) // There are no open blocks
	{
		pthread_mutex_unlock(&malloc_mutex);
		return NULL;
	}
	else if (p->size == nbytes) // Open space is the exact size of bytes needed
	{
		p->type = 1;
		pthread_mutex_unlock(&malloc_mutex);
		allocation_count++;
		return p->start;
	}
	else // Place bytes into open space
	{
		struct block *b = malloc(sizeof(block));
		b->size = nbytes;
		b->start = p->start;
		b->type = 1;
		b->next = p;

		p->start = p->start + nbytes;
		p->size = p->size - nbytes;
		if (p == top)
		{
			top = b;
		}
		pthread_mutex_unlock(&malloc_mutex);
		allocation_count++;
		return b->start;
	}
	pthread_mutex_unlock(&malloc_mutex);
	return NULL;
}

/* mymalloc_wf()
 *     Requests a block of memory be allocated using
 *         worst fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytes - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void *mymalloc_wf(int nbytes)
{
	pthread_mutex_lock(&malloc_mutex);
	struct block *p = top;
	while (p != NULL)
	{
		if (p->size >= nbytes && p->type == 0) // There is space and it is free
		{
			break;
		}
		p = p->next;
	}
	if (p == NULL) // There are no open blocks
	{
		pthread_mutex_unlock(&malloc_mutex);
		return NULL;
	}

	else if (p->size > nbytes) // Larger than the space needed (Needed for worst fit)
	{
		struct block *b = malloc(sizeof(block));
		b->size = nbytes;
		b->start = p->start;
		b->type = 1;
		b->next = p;

		p->start = p->start + nbytes;
		p->size = p->size - nbytes;
		if (p == top)
		{
			top = b;
		}
		pthread_mutex_unlock(&malloc_mutex);
		allocation_count++;
		return b->start;
	}
	else // The space needed is the same size as the space available
	{
		p->type = 1;
		pthread_mutex_unlock(&malloc_mutex);
		allocation_count++;
		return p->start;
	}
	pthread_mutex_unlock(&malloc_mutex);
	return NULL;
}

/* mymalloc_bf()
 *     Requests a block of memory be allocated using
 *         best fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytes - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void *mymalloc_bf(int nbytes)
{
	pthread_mutex_lock(&malloc_mutex);
	struct block *p = top;
	while (p != NULL)
	{
		if (p->size >= nbytes && p->type == 0) // There is space and it is free
		{
			break;
		}
		p = p->next;
	}
	if (p == NULL) // There are no open blocks
	{
		pthread_mutex_unlock(&malloc_mutex);
		return NULL;
	}

	else if (p->size == nbytes) // The free space is the same size as the space needed
	{
		p->type = 1;
		pthread_mutex_unlock(&malloc_mutex);
		allocation_count++;
		return p->start;
	}
	else // The space free is greater than the needed space
	{
		struct block *b = malloc(sizeof(block));
		b->size = nbytes;
		b->start = p->start;
		b->type = 1;
		b->next = p;

		p->start = p->start + nbytes;
		p->size = p->size - nbytes;
		if (p == top)
		{
			top = b;
		}
		pthread_mutex_unlock(&malloc_mutex);
		allocation_count++;
		return b->start;
	}
	pthread_mutex_unlock(&malloc_mutex);
	return NULL;
}

/* myfree()
 *     Requests a block of memory be freed and the storage made
 *         available for future allocations
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: ptr - a pointer to the start of the space to be freed
 *         Returns: void
 *         Signals a SIGSEGV if a free is not valid
 *             - memory manager is not initialized
 *             - memory manager has been destroyed
 *             - ptr is not allocated (e.g. double free)
 */
void myfree(void *ptr)
{
	struct block *b = malloc(sizeof(block));
	b->start = ptr;
	if (b->type == 0) // Already free
	{
		raise(SIGSEGV);
	}
	else
	{
		b->type = 0;
	}
}

/* get_allocated_space()
 *     Retrieve the current amount of space allocated by the memory manager (in bytes)
 *         Parameters: None
 *         Returns: int - the current number of allocated bytes
 */
int get_allocated_space()
{
	struct block *b = malloc(sizeof(block));
	b = top;
	int allocatedSpace = 0;
	while (b->type == 1)
	{
		allocatedSpace += b->size;
		b = b->next;
	}
	return allocatedSpace;
}

/* get_remaining_space()
 *     Retrieve the current amount of available space in the memory manager (in bytes)
 *         (e.g. sum of all free blocks)
 *         Parameters: None
 *         Returns: int - the current number of free bytes
 */
int get_remaining_space()
{
	struct block *b = malloc(sizeof(block));
	b = top;
	int freeSpace = 0;
	while (b->next != NULL)
	{
		if (b->type == 0)
		{
			freeSpace += b->size;
			b = b->next;
		}
		else
		{
		}
		b = b->next;
	}
	return freeSpace;
}

/* get_fragment_count()
 *     Retrieve the current amount of free blocks (i.e. the count of all the block, not the size)
 *         Parameters: None
 *         Returns: int - the current number of free blocks
 */
int get_fragment_count()
{
	return fragment_count;
}

/* get_mymalloc_count()
 *     Retrieve the number of successful malloc calls (for all placement types)
 *         Parameters: None
 *         Returns: int - the total number of successful mallocs
 */
int get_mymalloc_count()
{
	return allocation_count;
}
