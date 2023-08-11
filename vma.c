// Copyright Margheanu Cristina-Andreea 313CA
#include <inttypes.h>
#include "vma.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

// Functie care trebuie apelata pentru alocarea si initializarea unei liste.
list_t*
dll_create(unsigned int data_size)
{
	list_t *list = malloc(sizeof(*list));
	DIE(!list, "malloc() failed\n");
	// initializari necesare definirii listei
	list->size = 0;
	list->data_size = data_size;
	list->head = NULL;
	return list;
}

dll_node_t*
dll_get_nth_node(list_t *list, unsigned int n)
{   int index = n % list->size;
	dll_node_t *curr = list->head;
    // daca lista nu a fost creata sau e goala
	if (!list || !list->head) {
		printf("Not created or empty list!\n");
		return NULL; }
	for (int i = 0; i < index; i++)
		curr = curr->next;
	return curr;
}

dll_node_t*
dll_remove_nth_node(list_t *list, unsigned int n)
{
	if (!list || !list->head) {
		printf("Not created or empty list!\n");
		return NULL;
		}
	// Verificam daca am depasit dimensiunea listei
	if (n >= list->size)
		n = list->size - 1;
	if (list->size == 1) {
		// Daca stergem unicul element
		dll_node_t *node = list->head;
		list->head = NULL;
		list->tail = NULL;
		list->size = 0;
		return node;
	}
		dll_node_t *curr = dll_get_nth_node(list, n);
	// Analizam fiecare pozitie in care ne putem afla in lista
	if (n == 0)
		list->head = list->head->next;
	if (n == list->size - 1)
		list->tail = list->tail->prev;
	if (curr->prev)
		curr->prev->next = curr->next;
	if (curr->next)
		curr->next->prev = curr->prev;
	list->size--;
	return curr;
}

void
dll_add_nth_node(list_t *list, unsigned int n, const void *data)
{	if (!list) {
		printf("Not created list.\n");
		return;
	}
	// cream noul nod
	dll_node_t *new_node = malloc(sizeof(*new_node));
	DIE(!new_node, "malloc() failed!\n");
	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "malloc() failed!\n");
	memcpy(new_node->data, data, list->data_size);
	if (list->size == 0) {
		//adaugam primul element
		list->head = new_node;
		list->tail = new_node;
		new_node->next = NULL;
		new_node->prev = NULL;
	} else {
		dll_node_t *curr, *prev;
		if (n >= list->size) {
			prev = dll_get_nth_node(list, list->size - 1);
			curr = NULL;
	} else {
		curr = dll_get_nth_node(list, n);
		prev = curr->prev;
	}
	new_node->prev = prev;
	new_node->next = curr;
	if (prev)
		prev->next = new_node;
	if (curr)
		curr->prev = new_node;
	if (n == 0)
		list->head = new_node;
	if (n >= list->size)
		list->tail = new_node;
	}
	list->size++;
}

unsigned int
dll_get_size(list_t *list)
{
	if (!list)
		return 0;
	return list->size;
}

void dll_free(list_t **pp_list)
{
	if (!(*pp_list))
		return;
	if (!(*pp_list)->head)
		return;
	dll_node_t *curr;
	while ((*pp_list)->size != 0) {
		// Parcurgem lista si eliberam memoria pentru fiecare nod
		curr = dll_remove_nth_node(*pp_list, 0);
		free(curr->data);
		free(curr);
	}
	free(*pp_list);
	(*pp_list) = NULL;
}

arena_t *alloc_arena(const uint64_t size)
{	arena_t *arena;
	arena = malloc(sizeof(arena_t));
	DIE(!arena, "malloc() failed!\n");
	arena->arena_size = size;
	// initializarea listei de blocuri
	arena->alloc_list = dll_create(sizeof(block_t));
	return arena;
}

void dealloc_arena(arena_t *arena)
{	dll_node_t *block_node = arena->alloc_list->head;
	// parcurgem intreaga lista de blocuri
	while (block_node) {
		block_t *block = (block_t *)block_node->data;
		dll_node_t *mini_node = block->miniblock_list->head;
		// parcurgem lista de miniblocuri
		while (mini_node) {
			miniblock_t *miniblock = (miniblock_t *)mini_node->data;
			if (miniblock->rw_buffer)
				free(miniblock->rw_buffer);
			free(miniblock);
			// mergem la urmatoarele noduri
			dll_node_t *new_mini = (dll_node_t *)mini_node->next;
			free(mini_node);
			mini_node = new_mini;
		}
		free(block->miniblock_list);
		free(block);
		dll_node_t *new_block = (dll_node_t *)block_node->next;
		free(block_node);
		block_node = new_block;
	}
	// eliberam la final si ultimele resurse folosite
	free(arena->alloc_list);
	free(arena);
}

// Functie pentru verificarea posibilitatii alocarii unui bloc
// la o anumita adresa

int verify(arena_t *arena, const uint64_t address, const uint64_t size)
{
	int ok = 1;
	if (address >= arena->arena_size) {
		printf("The allocated address is outside the size of arena\n");
		ok = 0;
		return 0;
	}
	if (address + size > arena->arena_size) {
		printf("The end address is past the size of the arena\n");
		ok = 0;
		return 0;
	}
	dll_node_t *node = arena->alloc_list->head;
	while (node) {
		block_t *block = (block_t *)node->data;
		if ((block->start_address <= address &&
		     (block->start_address + block->size) > address) ||
		   (address <= block->start_address &&
		   block->start_address < (address + size))) {
			printf("This zone was already allocated.\n");
			ok = 0;
			return 0;
			}
			node = node->next;
		}
	if (ok == 0)
		return 0;
	return 1;
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{	 if (verify(arena, address, size) == 0)
		return;
	int index = 0;
	int last_index = -1;
	dll_node_t *node = arena->alloc_list->head;
	while (node) {
		block_t *block = (block_t *)node->data;
		// finalul unui bloc
		if ((block->start_address + block->size) == address) {
			miniblock_t *miniblock = malloc(sizeof(*miniblock));
	DIE(!miniblock, "malloc() failed.\n");
	// initializari necesare miniblock-ului
	miniblock->size = size;
	miniblock->start_address = address;
	miniblock->rw_buffer = NULL;
	miniblock->perm = 6;
	dll_add_nth_node(block->miniblock_list,
					 block->miniblock_list->size, miniblock);
	// crestem dimensiunea blocului
	block->size = block->size + miniblock->size;
	// verificam nodul urmator
	dll_node_t *new_node = (dll_node_t *)node->next;
	if (!new_node) {
		free(miniblock);
		return;
	}
	block_t *new_block = (block_t *)new_node->data;
	if (new_block->start_address == address + size) {
		// restabilim legaturile si dimensiunile
		block->miniblock_list->tail->next = new_block->miniblock_list->head;
		new_block->miniblock_list->head->prev = block->miniblock_list->tail;
		block->miniblock_list->tail = new_block->miniblock_list->tail;
		block->miniblock_list->size += new_block->miniblock_list->size;
		block->size += new_block->size;
		new_node = dll_remove_nth_node(arena->alloc_list, index + 1);
		free(new_block->miniblock_list);
		free(new_block);
		free(new_node);
		free(miniblock);
		return;
		} free(miniblock);
	}
	if (address + size == block->start_address) {
		miniblock_t *miniblock = malloc(sizeof(*miniblock));
		DIE(!miniblock, "malloc() failed!\n");
		miniblock->size = size;
		miniblock->start_address = address;
		miniblock->rw_buffer = NULL;
		miniblock->perm = 6;
		dll_add_nth_node(block->miniblock_list, 0, miniblock);
		block->size += miniblock->size;
		block->start_address = miniblock->start_address;
		free(miniblock);
		return;
	}
	if (block->start_address + block->size - 1 < address)
		last_index = index;
	node = node->next; //mergem mai departe in arena
	index++;
	}
	miniblock_t *miniblock = malloc(sizeof(*miniblock));
	DIE(!miniblock, "malloc() failed\n");
	miniblock->size = size;
	miniblock->start_address = address;
	miniblock->rw_buffer = NULL;
	miniblock->perm = 6;
	block_t *block = malloc(sizeof(*block));
	DIE(!block, "malloc() failed\n");
	block->miniblock_list = dll_create(sizeof(miniblock_t));
	dll_add_nth_node(block->miniblock_list, 0, miniblock);
	block->start_address = address;
	block->size = size;
	dll_add_nth_node(arena->alloc_list, last_index + 1, block);
	free(miniblock);
	block->miniblock_list = NULL;
	free(block);
}

int count(dll_node_t *node)
{
	int rez = 0;
	while (node) {
		miniblock_t *miniblock = (miniblock_t *)node->data;
		rez += miniblock->size;
		node = node->next;
	}
	return rez;
}

void free_block(arena_t *arena, const uint64_t address)
{	dll_node_t *block_node = arena->alloc_list->head;
	miniblock_t *miniblock = NULL;
	block_t *block = NULL;
	dll_node_t *mini_node = NULL;
	int ok = 0;
	int index = 0, sw = 0;
	// Verificam daca putem aplica free
	while (block_node) {
		block = (block_t *)block_node->data;
		mini_node = block->miniblock_list->head;
		index = 0;
		while (mini_node) {
			miniblock = (miniblock_t *)mini_node->data;
			if (miniblock->start_address == address) {
				sw = 1;
				break; }
			index++;
			mini_node = mini_node->next;
		}
		if (sw == 1)
			break;
		ok++;
		block_node = block_node->next;
	}
	if (sw == 0) {
		printf("Invalid address for free.\n");
		return; }
	if (mini_node == block->miniblock_list->head) {
		mini_node = dll_remove_nth_node(block->miniblock_list, 0);
		block->size = block->size - miniblock->size;
		block->start_address = miniblock->start_address + miniblock->size;
		if (miniblock->rw_buffer)
			free(miniblock->rw_buffer);
		free(miniblock);
		free(mini_node);
		if (block->miniblock_list->size == 0) {
			block_node = dll_remove_nth_node(arena->alloc_list, ok);
			free(block->miniblock_list);
			free(block);
			free(block_node); }
		return;
	}
	if (mini_node == block->miniblock_list->tail) {
		mini_node = dll_remove_nth_node(block->miniblock_list,
										block->miniblock_list->size - 1);
		block->size = block->size - miniblock->size;
		if (miniblock->rw_buffer)
			free(miniblock->rw_buffer);
		free(miniblock);
		free(mini_node);
		if (block->miniblock_list->size == 0) {
			arena->arena_size -= block->size;
			block_node = dll_remove_nth_node(arena->alloc_list, ok);
			free(block->miniblock_list);
			free(block);
			free(block_node);
		}
		return;
	}
	block_t *new_block = malloc(sizeof(*new_block));
	new_block->miniblock_list = dll_create(sizeof(miniblock_t));
	new_block->miniblock_list->head = mini_node->next;
	new_block->miniblock_list->tail = block->miniblock_list->tail;
	new_block->miniblock_list->size = block->miniblock_list->size - index - 1;
	miniblock_t *new_mini = (miniblock_t *)mini_node->next->data;
	new_block->start_address = new_mini->start_address;
	new_block->size = count(mini_node->next);

	mini_node->prev->next = NULL;
	block->miniblock_list->tail = mini_node->prev;
	block->miniblock_list->tail->next = NULL;
	block->miniblock_list->size = index;
	block->size = count(block->miniblock_list->head);
	dll_add_nth_node(arena->alloc_list, ok + 1, new_block);
	free(new_block);
	if (miniblock->rw_buffer)
		free(miniblock->rw_buffer);
	free(miniblock);
	free(mini_node);
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	if (arena->arena_size <= address) {
		printf("Invalid address for read.\n");
		return;
	}
	dll_node_t *curr = arena->alloc_list->head;
	int ok = 1;
	block_t *block = NULL;
	while (curr) {
		block = (block_t *)curr->data;
		uint64_t adr = block->start_address + block->size;
		if (adr > address && address >= block->start_address) {
			ok = 0;
			break;
		}
		curr = curr->next;
	}
	if (ok == 1) {
		printf("Invalid address for read.\n");
		return;
	}
		block = (block_t *)curr->data;
		dll_node_t *node = block->miniblock_list->head;
		int size2 = dll_get_size(block->miniblock_list);
		for (int j = 0; j < size2; j++) {
			miniblock_t *miniblock = (miniblock_t *)node->data;
			if (miniblock->perm == 0 || miniblock->perm == 2) {
				printf("Invalid permissions for read.\n");
				return;
			}
			node = node->next;
		}
	uint64_t rest = size;
	if (((block_t *)curr->data)->size < size) {
		printf("Warning: size was bigger than the block size. ");
		printf("Reading %lu characters.\n", ((block_t *)curr->data)->size);
	rest = block->start_address + block->size - address;
	}
	node = block->miniblock_list->head;
	while (node) {
		miniblock_t *miniblock = (miniblock_t *)node->data;
		if (miniblock->start_address == address)
			break;
		node = node->next;
	}
	// ma poizitionez la inceputul miniblocului de unde incepe citirea
	uint64_t len = 0;
	while (len < rest && node) {
		miniblock_t *miniblock = (miniblock_t *)node->data;
		char *text = (char *)miniblock->rw_buffer;
		if (miniblock->size + len <= rest) {
			printf("%s", text);
		len = len + miniblock->size;
		} else {
			uint64_t left = rest - len;
		char *left_text = malloc((left + 1) * sizeof(char));
		DIE(!left_text, "malloc() failed\n");
		memcpy(left_text, text, left + 1);
		printf("%s", left_text);
		free(left_text);
		len = len + left;
			}
			node = node->next;
		}
	 printf("\n");
}

void
write(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data)
{	if (arena->arena_size <= address) {
		printf("Invalid address for write.\n");
		return;
	}
	dll_node_t *curr = arena->alloc_list->head;
	int ok = 1;
	block_t *block = NULL;
	while (curr) {
		block = (block_t *)curr->data;
		uint64_t adr = block->start_address + block->size;
		if (adr > address && address >= block->start_address) {
			ok = 0;
			break; }
		curr = curr->next;
	}
	if (ok == 1) {
		printf("Invalid address for write.\n");
		return;
	}
		block = (block_t *)curr->data;
		dll_node_t *node = block->miniblock_list->head;
		int size2 = dll_get_size(block->miniblock_list);
		for (int j = 0; j < size2; j++) {
			miniblock_t *miniblock = (miniblock_t *)node->data;
			if (miniblock->perm == 0 || miniblock->perm == 4) {
				printf("Invalid permissions for write.\n");
				return;
			}
			node = node->next;
		}
	uint64_t rest = size;
	if (((block_t *)curr->data)->size +
	      ((block_t *)curr->data)->start_address < size + address) {
		printf("Warning: size was bigger than the block size. ");
		printf("Writing %lu characters.\n", ((block_t *)curr->data)->size
		       + ((block_t *)curr->data)->start_address - address);
		rest = block->start_address + block->size - address;
	}
	node = block->miniblock_list->head;
	while (node) {
		miniblock_t *miniblock = (miniblock_t *)node->data;
		if (miniblock->start_address == address)
			break;
		node = node->next;
	}
	uint64_t len = 0;
	while (len < rest && node) {
		miniblock_t *miniblock = (miniblock_t *)node->data;
		if (miniblock->size + len <= rest) {
			char *copy = malloc((miniblock->size + 1) * sizeof(char));
			memcpy(copy, data + len, miniblock->size);
			copy[miniblock->size] = '\0';
			len = len + miniblock->size;
			miniblock->rw_buffer = copy;
		} else {
			uint64_t left = rest - len;
			char *copy = malloc((left + 1) * sizeof(char));
			memcpy(copy, data + len, (left + 1));
			len = len + left;
			miniblock->rw_buffer = copy;
			}
			node = node->next;
		}
}

// Functie folosita pentru aflarea numarului de miniblocuri din arena.

int mini(const arena_t *arena)
{
	dll_node_t *node = arena->alloc_list->head;
	int nr_mini = 0;
	while (node) {
		block_t *block = (block_t *)node->data;
		nr_mini = nr_mini + block->miniblock_list->size;
		node = node->next;
	}
	return nr_mini;
}

void pmap(const arena_t *arena)
{	uint64_t occup = 0;
	block_t *block = NULL;
	dll_node_t *node = arena->alloc_list->head;
	while (node) {
		block = (block_t *)node->data;
		occup += block->size;
		node = node->next;
	}
	printf("Total memory: 0x%lX bytes\n", arena->arena_size);
	uint64_t free_mem = arena->arena_size - occup;
	printf("Free memory: 0x%lX bytes\n", free_mem);
	printf("Number of allocated blocks: %d\n", arena->alloc_list->size);
	printf("Number of allocated miniblocks: %d\n", mini(arena));

	int size1 = dll_get_size(arena->alloc_list);
	if (arena->alloc_list->head == 0)
		return;
	printf("\n");
	node = arena->alloc_list->head;
	for (int i = 0; i < size1; i++) {
		block = (block_t *)node->data;
	printf("Block %d begin\n", i + 1);
	printf("Zone: 0x%lX - 0x%lX\n", block->start_address,
	       block->start_address + block->size);
	int size2 = dll_get_size(block->miniblock_list);
	dll_node_t *mininode = block->miniblock_list->head;
	for (int j = 0; j < size2; j++) {
		miniblock_t *miniblock = (miniblock_t *)mininode->data;
		uint64_t mini1 = miniblock->start_address;
		uint64_t mini2 = mini1 + miniblock->size;
		printf("Miniblock %d:\t\t0x%lX\t\t-\t\t0x%lX\t\t| ",
			   j + 1, mini1, mini2);
		if (miniblock->perm == 6)
			printf("RW-");
		if (miniblock->perm == 7)
			printf("RWX");
		if (miniblock->perm == 4)
			printf("R--");
		if (miniblock->perm == 0)
			printf("---");
		if (miniblock->perm == 2)
			printf("-W-");
		printf("\n");
		mininode = mininode->next;
	}
	printf("Block %d end\n", i + 1);
	if (i + 1 < size1)
		printf("\n");
	node = node->next;
	}
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	dll_node_t *curr = arena->alloc_list->head;
	block_t *block = NULL;
	int size1 = dll_get_size(arena->alloc_list);
	for (int i = 0; i < size1; i++) {
		block = (block_t *)curr->data;
		dll_node_t *node = block->miniblock_list->head;
		int size2 = dll_get_size(block->miniblock_list);
		for (int j = 0; j < size2; j++) {
			miniblock_t *miniblock = (miniblock_t *)node->data;
			if (miniblock->start_address == address) {
				miniblock->perm = 0;
				if (strstr((const char *)permission, "PROT_NONE"))
					miniblock->perm = 0;
				if (strstr((const char *)permission, "PROT_EXEC"))
					miniblock->perm = miniblock->perm + 1;
				if (strstr((const char *)permission, "PROT_WRITE"))
					miniblock->perm = miniblock->perm + 2;
				if (strstr((const char *)permission, "PROT_READ"))
					miniblock->perm = miniblock->perm + 4;
				return;
			}
			node = node->next;
		}
		curr = curr->next;
	}
	printf("Invalid address for mprotect.\n");
}
