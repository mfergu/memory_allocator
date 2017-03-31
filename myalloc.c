/*
                                   /$$ /$$                                  
                                  | $$| $$                                  
 /$$$$$$/$$$$  /$$   /$$  /$$$$$$ | $$| $$  /$$$$$$   /$$$$$$$      /$$$$$$$
| $$_  $$_  $$| $$  | $$ |____  $$| $$| $$ /$$__  $$ /$$_____/     /$$_____/
| $$ \ $$ \ $$| $$  | $$  /$$$$$$$| $$| $$| $$  \ $$| $$          | $$      
| $$ | $$ | $$| $$  | $$ /$$__  $$| $$| $$| $$  | $$| $$          | $$      
| $$ | $$ | $$|  $$$$$$$|  $$$$$$$| $$| $$|  $$$$$$/|  $$$$$$$ /$$|  $$$$$$$
|__/ |__/ |__/ \____  $$ \_______/|__/|__/ \______/  \_______/|__/ \_______/
               /$$  | $$                                                    
              |  $$$$$$/                                                    
               \______/                   
*/

#define _GNU_SOURCE

#include "myalloc.h"
#define T header_t

T *partial_pages[NUM_OF_CACHES];
int fd;

void __attribute__ ((constructor)) myalloc_init(void) {

	for(uint_fast8_t i = 0; i < NUM_OF_CACHES; i++) {
		partial_pages[i] = NULL;
	}
		
	fd = open( "/dev/zero", O_RDWR) ;
}

void __attribute__ ((destructor)) myalloc_cleanup(void) {

}

T create_header( size_t size) {

	T temp = { NULL, NULL, NULL, size, 0};
	return temp;
}


//finds the index of the segregated list to place the data
uint_fast16_t find_slot( uint_fast16_t x) {

	uint_fast16_t i = 0, j = 2;
	while( x > j ) {
		j = j*2;
		++i;
	}
	return i;
}

//finds the ceiling size required to place the data in the segregated list
uint_fast16_t find_cache_size( uint_fast16_t x) {

	uint_fast16_t i = 2;
	while(x > i) {
		i = i * 2;
	}
	return i;
}

//returns the location of the new mapped memory
uint_fast32_t* create_page( size_t size) {

	uint_fast32_t* actual_addr = Mmap(NULL, size + sizeof(T), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	T *temp = (T*)actual_addr;	
	*temp = create_header( size);

	return actual_addr;
} 

uint_fast32_t* create_cache_page( size_t size) {

	uint_fast32_t* actual_addr = Mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	uint_fast32_t* user_space;
	T *temp = (T*)actual_addr;	
	uint_fast16_t cache_id = find_cache_size(size);
	*temp = create_header( cache_id);
	temp->available = ( (PAGESIZE - sizeof(T)) / cache_id);			

	//set the free_list ptr to the user_space 
	user_space = actual_addr + (sizeof(T) >> 3);
	temp->free_list = user_space;

	uint_fast32_t *temp_ptr = user_space;
	for(uint_fast8_t i = 0; i < temp->available - 1; i++) {
		*temp_ptr = (temp_ptr + (temp->obj_size >> 3));
		temp_ptr += ((temp->obj_size) >> 3);
	}

	return actual_addr;
}
	
void remove_page_from_cache( T* header, T** cache) {

	size_t size = header->obj_size;
	uint_fast16_t cache_id = find_slot( size);
		
	if( header->previous != NULL) {
		header->previous->next = header->next;
		cache[cache_id] = header->previous;
	}

	if( header->next != NULL) {
		header->next->previous = header->previous;
		cache[cache_id] = header->next;
	}

	header->previous = header->next = NULL;
}

void add_page_to_cache( T* header, T** cache) {

	size_t size = header->obj_size;
	uint_fast16_t cache_id = find_slot( size);
	
	header->previous = NULL;
	if( cache[cache_id] != NULL) {
		header->next = cache[cache_id];
	}

	if(header->next != NULL) {
		header->next->previous = header;
	}

	cache[cache_id] = header;
}

void* malloc( size_t size){

	T *temp;
	uint_fast32_t *actual_addr, *user_space;
	
	//if size > 1024 create a new page 
	if( size > 1024) {
		
		actual_addr = create_page( size);
		
		user_space = actual_addr + (sizeof(T) >> 3);
		
	} else {
	//size is < 1024
	 
		// find a cache to request an open memory slot	
		uint_fast16_t cache_id = find_slot( size);	
		
		if( partial_pages[cache_id] == NULL) {

			//create the new page and add it to the empty segregated list
			actual_addr = create_cache_page( size);
			temp = (T*) actual_addr; 

			add_page_to_cache(temp, partial_pages);
			temp->available--;

			user_space = temp->free_list;
			temp->free_list = *temp->free_list;
			
		}
		else {
		//there exists a page with that size object 
			actual_addr = partial_pages[cache_id];
			temp = ( (T*) actual_addr);
			user_space = temp->free_list;

			if( temp->available == 1) {
				//if the front page of partial_pages is full add it to the full_pages 
				
				temp->available--;
				user_space = temp->free_list;

//				remove_page_from_cache( temp, partial_pages);
//				add_page_to_cache( temp, full_pages);
				
				//copy of null page creation	
				actual_addr = create_cache_page( size);
				temp = (T*) actual_addr; 
				add_page_to_cache(temp, partial_pages);

			}
			else {
				//if the partial_pages front isn't full 

				temp->available--;
				user_space = temp->free_list;
				temp->free_list = *temp->free_list;
			}
			
		}		
	}
	return user_space;
}


void* calloc( size_t count, size_t size) {

	size_t total = count*size;
	void* temp = malloc( total);
	memset( temp, 0, total);
	return temp;
}

void* realloc( void* ptr, size_t size) {

	void* temp = malloc(size);
	T* header = (T*) ptr;
	if( size > header->obj_size) {
		Memcpy( temp, ptr, header->obj_size);
	} else {
		Memcpy( temp, ptr, size);
	}
		
	free(ptr);	
		
	return temp;
}

void free( void* ptr) {
	
	T *header =  ( T*) ( (uint_fast32_t) ptr & 0xFFFFFFFFF000);
	size_t size = header->obj_size;

	if( size > 1024) {
		munmap( header, size + sizeof(T));
	}
	else {

		header->available++;
		uint_fast16_t cache_id = find_slot( size);
		if( header->available == 1) {

//			remove_page_from_cache( header, full_pages[cache_id]);
			add_page_to_cache( header, partial_pages[cache_id]);		
			header->free_list = ptr;
		}
		else if( header->available == (PAGESIZE - sizeof(T)) / header->obj_size) { 
/*
			printf("partial[id]   : %p \n", partial_pages[cache_id]);
			printf(" cache_avail  : %p\n", partial_pages[cache_id]->available);
			printf("unmapping page: %p\n", header); 
*/
			remove_page_from_cache(header, partial_pages);
			munmap( header, PAGESIZE);
		}
		else {
	/*
			printf("  hdr avai: %p \n", header->available);
			printf("  hdr_fr_l: %p \n", header->free_list);
	*/
			uint_fast32_t* temp = (uint32_t*) ptr;
			*temp = header->free_list;
			header->free_list = temp;
	/*
			printf(" *hdr_fr_l: %p \n", *header->free_list);
			printf("  hdr_fr_l: %p \n", header->free_list);
	*/
		}
	}	
}

void *Mmap(void *addr, size_t length, int prot, int flags,
    int fd, off_t offset) {
        void *temp;
        if( ( temp = mmap( addr, length, prot, flags,
                fd, offset)) == MAP_FAILED) {
            perror(" mmap error\n");
        }
        else {
            return temp;
        }
}

void Munmap( void* addr, size_t length) {
	if( munmap(addr, length) == -1) {
		perror(" munmap error\n");
	}
}

void Memcpy( void* dest, const void *src, size_t n) {
    void *temp;
        if( (temp = memcpy(dest, src, n)) == NULL) {
            perror(" memcpy error\n");
        }
}