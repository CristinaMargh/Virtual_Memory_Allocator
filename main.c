#include "vma.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COMM_SIZE 30
int main(void)
{   arena_t *arena = NULL;
	char command[MAX_COMM_SIZE];
	do { scanf("%s", command);
	if (strcmp(command, "ALLOC_ARENA") == 0) {
		uint64_t dim_arena;
		scanf("%ld", &dim_arena);
	   arena = alloc_arena(dim_arena);
	} else if (strcmp(command, "ALLOC_BLOCK") == 0) {
		uint64_t address, size;
		scanf("%ld%ld", &address, &size);
		alloc_block(arena, address, size);
	} else if (strcmp(command, "FREE_BLOCK") == 0) {
		uint64_t address;
	    scanf("%ld", &address);
		free_block(arena, address);
	} else if (strcmp(command, "READ") == 0) {
		uint64_t address, size;
		scanf("%ld%ld", &address, &size);
		read(arena, address, size);
	} else if (strcmp(command, "WRITE") == 0) {
		uint64_t address, size;
		scanf("%ld%ld", &address, &size);
		int8_t *data = malloc((size + 1) * sizeof(char));
		scanf("%c", &data[0]);
		for (uint64_t i = 0; i < size; i++)
			scanf("%c", &data[i]);
		data[size] = '\0';
		write(arena, address, size, data);
		free(data);
	} else if (strcmp(command, "MPROTECT") == 0) {
		uint64_t address;
		scanf("%ld", &address);
		int8_t *permission = malloc(50 * sizeof(int8_t));
		fgets((char *)permission, 50, stdin);
		mprotect(arena, address, permission);
		free(permission);
	} else if (strcmp(command, "PMAP") == 0) {   
		pmap(arena);
	} else if (strcmp(command, "DEALLOC_ARENA") != 0) {
		printf("Invalid command. Please try again.\n");
		}
	} while (strcmp(command, "DEALLOC_ARENA") != 0);
	dealloc_arena(arena);
	return 0;
}
