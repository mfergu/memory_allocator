// https://www.usenix.org/legacy/publications/library/proceedings/bos94/full_papers/bonwick.a 
// https://www.kernel.org/doc/gorman/html/understand/understand011.html 
// linux kernel development 3rd ed. 

#pragma once

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#define PAGESIZE 4096
#define MAX_OBJ_SIZE 1024
#define NUM_OF_CACHES 10

void myalloc_init(void);
void myalloc_cleanup(void);


typedef struct header_t{                                                                                              
	uint_fast32_t *free_list;
    struct header_t *previous, *next;                                                                                
    uint16_t obj_size;                                                                                     
    ssize_t available;                                                                                   
}header_t;                                                                                                     
                                                                                                              
/*
To allocate an object:

	  if ( request smaller than or equal to max_obj_size)
		  take it ( from the list);
	  else {
		  allocate memory;
		  construct the object;
	  }
*/
void* malloc( size_t);
void* calloc( size_t count, size_t size);
void* realloc( void* ptr, size_t size);

/*
To free an object:

      return it to the list (no destruction required);

To reclaim memory from the cache:

  take some objects from the list;
  destroy the objects;
  free the underlying memory;
*/
void free( void* ptr);

void *Mmap(void* , size_t , int, int, int, off_t);
void Memcpy( void*, const void*, size_t);
