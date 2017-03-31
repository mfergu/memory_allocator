#include <stdio.h>
#include <stdlib.h>

int main( void ) {


	unsigned long ** temp;
	for( int i = 0; i < 7; i++) {
	temp[i]= (unsigned long*) calloc(sizeof(unsigned long), 128);
	printf("callocing temp%u: %p\n",i, temp[i]);
	*temp[i] = temp[i];


}

	for( int i = 0; i < 3; i++) {
		int j = 6-i;
		printf("freeing temp%d:", j);
		printf("	ptr: %p\n",temp[j]);
		fflush(stdout);
		free(temp[j]);
	}

	for( int i = 4; i < 7; i++) {
		temp[i]= (unsigned long*) malloc(sizeof(unsigned long)* 128);
		printf("mallocing temp%u: %p\n",i, temp[i]);
		*temp[i] = temp[i];
	}



/*
	printf(" sizeof(unsigned long) * 256: %p\n", sizeof(unsigned long) * 128);
	printf(" sizeof(unsigned long)		: %p\n", sizeof(unsigned long));
	printf(" malloc : %p\t", temp[i]);
	printf(" storing: %p\n", *temp[i]);

	size_t* temp4 = malloc(512);
	printf(" malloc: %p\n", temp4);

	printf("creating a calloc\n");
	size_t* temp2 = calloc(1, sizeof(size_t));

	printf("creating a realloc\n");
	temp1 = realloc( temp1, sizeof(size_t));
*/
} 
