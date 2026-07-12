
#include "ft_malloc_intern.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static void
show_map(uint8_t map_type, size_t* total_used_mem) {
	t_page*		current_page;
	t_chunk*	current_chunk;
	size_t		used_mem;
	size_t		chunk_size;

	current_page = g_malloc.map[map_type];
	while (current_page) {
		current_chunk = ptr_add(current_page, sizeof(t_page) - sizeof(t_chunk));
		chunk_size = 0;
		do {
			current_chunk = ptr_add(current_chunk, sizeof(t_chunk) + chunk_size);
			chunk_size = current_chunk->size;
			if (!get_crnt_chunk_free_flag(current_chunk)) {
				used_mem = chunk_size;
				*total_used_mem += used_mem;
				printf("%p - %p : %lu Bytes\n", ptr_add(current_chunk, sizeof(t_chunk)), ptr_add(current_chunk, sizeof(t_chunk) + used_mem), used_mem);
			}
		} while (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk));
		current_page = current_page->next;
	}
}

void
show_alloc_mem(void) {
	size_t	total_used_mem;

	total_used_mem = 0;
	printf("TINY : %p\n", (void*)g_malloc.map[TINY]);
	show_map(TINY, &total_used_mem);
	printf("SMALL : %p\n", (void*)g_malloc.map[SMALL]);
	show_map(SMALL, &total_used_mem);
	printf("LARGE : %p\n", (void*)g_malloc.map[LARGE]);
	show_map(LARGE, &total_used_mem);
	printf("Total : %lu\n", total_used_mem);
}
