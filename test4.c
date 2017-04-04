#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define MAX_CALLOCS 10

 test_structure_t;

uint8_t*  temp[10];
unsigned long pow( unsigned a , unsigned b) {
	
	if(b == 0) {
		return 1;
	}
	int temp = a;
	while( b-- > 0) {
		temp = temp * a;
	}
//	printf("temp %u\n", temp);
	return temp;
}

int main(int argc, char **argv)
{
//srand(time(NULL));

	for (int i=0; i < MAX_CALLOCS; i++) {

		int k = 1000;
		do {
			int j = rand() %1130 ;
			if( k%3 == 0 || k%3 == 2) {
				temp[i] = malloc(j);
//				printf(" temp[%u] = malloc %u\n",i, j);
			} else {
				temp[i] = calloc(1, j);
//				printf(" temp[%u] = calloc %u\n",i, j);
			}
		} while( k--);
		
		if( i == 9) {
			int p = i;
			do {
				k = 10000;
//				printf("made it to second loop\n");
				do{ 
					int j = rand() %1130;
//					printf(" temp[%u] =realloc %u\n", p, j);
					temp[p] = realloc(temp[p],  j);		
					for( int l = 0; l < j; l++) {
						*(temp[p]+l) = 'a';
					}
				} while( k--);
					free(temp[p]);
					temp[p] = NULL;
			} while( --p >= 0);
		}
	}
}

