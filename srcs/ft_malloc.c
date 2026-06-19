
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <assert.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stdio.h>

t_malloc	g_malloc;

static void*
allocate_large(size_t size) {
	void*			new_memory;
	t_page			page_header;
	int const		page_size = sysconf(_SC_PAGE_SIZE);
	size_t const	allocated_memory = size + page_size - (size & (page_size - 1));

	new_memory = mmap(NULL, allocated_memory, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (new_memory == MAP_FAILED)
		return (new_memory);
	page_header.chunk = new_memory + sizeof(t_page);
	page_header.next = g_malloc.large;
	g_malloc.large = new_memory;
	*(t_page *)new_memory = page_header;
	new_memory += sizeof(t_page);
	*(t_chunk *)new_memory = (t_chunk) {.size = allocated_memory, .next = NULL};
	return (new_memory + sizeof(t_chunk));
}


static void*
insert_new_chunk(t_chunk* current_chunk, t_chunk* next_chunk, size_t size) {
	void*	new_chunk;

	new_chunk = (void *)current_chunk + sizeof(t_chunk) + current_chunk->size;
	current_chunk->next = new_chunk;
	*(t_chunk *)new_chunk = (t_chunk) {.size = size, .next = next_chunk};
	return (new_chunk + sizeof(t_chunk));
}

void*
alloc_tiny(size_t size) {
	void*		result;
	t_page*		prev_page;
	t_page*		current_page;
	t_chunk*	current_chunk;
	t_chunk*	next_chunk;

	result = NULL;
	current_page = g_malloc.tiny;
	if (!g_malloc.tiny)
		return (init_zone(TINY, size));
	current_page = g_malloc.tiny;
	while (current_page) {
		current_chunk = current_page->chunk;
		if (!current_chunk || ((void*)current_page + sizeof(t_page)) - (void*)current_chunk >= size)
			return (insert_new_chunk_after_page_header(current_page, size));
		while (current_chunk) {
			next_chunk = current_chunk->next;
			if (get_free_space_between(current_page, current_chunk, next_chunk) >= size)
				return (insert_new_chunk(current_chunk, next_chunk, size));
			current_chunk = current_chunk->next;
		}
		prev_page = current_page;
		current_page = current_page->next;
	}
	// result = allocat_new_page(prev_page, TINY);
	return (NULL);
}


void*
ft_malloc(size_t size) {
	void*	result;
	uint8_t	rest;

	if (!size)
		return (NULL);

	rest = size & (_Alignof(void*) - 1);
	if (rest)
		size = size + _Alignof(void*) - rest;
	// if (size <= N)
	result = alloc_tiny(size);
	// else if (size <= M)
	// 	resutl = allocate_small(size);
	// else
		// resutl = allocate_large(size);
	if (result == MAP_FAILED)
		result = NULL;
	return (result);
}

int main(void) {
	t_page*		page;
	t_chunk*	chunk;
	t_chunk*	chunk2;
	void* ptr = ft_malloc(5);
	void* ptr2 = ft_malloc(7);

	page = ptr - sizeof(t_chunk) - sizeof(t_page);
	chunk = ptr - sizeof(t_chunk);
	printf("%p\n", page->chunk);
	printf("%p\n", chunk);

	chunk2 = ptr2 - sizeof(t_chunk);
	printf("%lu\n", chunk2->size);
	return (0);
}
