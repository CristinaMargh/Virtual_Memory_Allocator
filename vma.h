#pragma once
#include <inttypes.h>
#include <stddef.h>

typedef struct dll_node_t {
	void *data;
	struct dll_node_t *prev, *next;
} dll_node_t;

typedef struct list_t {
	dll_node_t *head, *tail;
	unsigned int data_size;
	unsigned int size;
} list_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm;
	void *rw_buffer;

} miniblock_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	list_t *miniblock_list;
} block_t;

typedef struct {
	uint64_t arena_size;
	list_t *alloc_list;
} arena_t;

arena_t *alloc_arena(const uint64_t size);
void dealloc_arena(arena_t *arena);
int verify(arena_t *arena, const uint64_t address, const uint64_t size);
void alloc_block(arena_t *arena, const uint64_t address,
				 const uint64_t size);
void free_block(arena_t *arena, const uint64_t address);
void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address,
		   const uint64_t size, int8_t *data);
int count(dll_node_t *node);
void pmap(const arena_t *arena);
int mini(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);
