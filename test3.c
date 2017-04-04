#include <stdio.h>
#include <stdlib.h>

#define NAME_SIZE  16
#define MAX_CALLOCS 5

typedef struct 
{
	int a;
	int b;
	int c;
	char name[NAME_SIZE];
} test_structure_t;



int main(int argc, char **argv)
{

	for (int i=1; i < MAX_CALLOCS; i++)
	{
		test_structure_t *p= calloc(i, sizeof(test_structure_t));
		p = realloc(p, (i%3) * sizeof(test_structure_t));		
		free(p);
		p = NULL;
	}

}

