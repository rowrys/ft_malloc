
#include "ft_malloc_intern.h"

#include <stdio.h>

static size_t
show_map(uint8_t map_type) {
	t_page*		current_page;
	t_chunk*	current_chunk;
	size_t		used_mem;
	size_t		chunk_size;
	size_t		total_used_mem;

	total_used_mem = 0;
	current_page = g_malloc.map[map_type];
	while (current_page) {
		current_chunk = ptr_add(current_page, sizeof(t_page) - sizeof(t_chunk));
		chunk_size = 0;
		do {
			current_chunk = ptr_add(current_chunk, sizeof(t_chunk) + chunk_size);
			chunk_size = current_chunk->size;
			if (!get_crnt_chunk_free_flag(current_chunk)) {
				used_mem = chunk_size;
				total_used_mem += used_mem;
				printf("%p - %p : %lu bytes\n", ptr_add(current_chunk, sizeof(t_chunk)), ptr_add(current_chunk, sizeof(t_chunk) + used_mem), used_mem);
			}
		} while (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk));
		current_page = current_page->next;
	}
	return (total_used_mem);
}

void
show_alloc_mem(void) {
	size_t	total_used_mem;

	total_used_mem = 0;
	printf("TINY : %p\n", (void*)g_malloc.map[TINY]);
	total_used_mem += show_map(TINY);
	printf("SMALL : %p\n", (void*)g_malloc.map[SMALL]);
	total_used_mem += show_map(SMALL);
	printf("LARGE : %p\n", (void*)g_malloc.map[LARGE]);
	total_used_mem += show_map(LARGE);
	printf("Total : %lu bytes\n", total_used_mem);
}
