
#include "ft_malloc_intern.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

static void
free_page_from_map(uint8_t map_type) {
	t_page*	current_page;
	t_page*	next_page;

	current_page = g_malloc.map[map_type];
	while (current_page) {
		next_page = current_page->next;
		munmap(current_page, current_page->size);
		current_page = next_page;
	}
	g_malloc.map[map_type] = NULL;
}

void
check_page_integrity(void) {
	t_page*		current_page;
	t_chunk*	current_chunk;
	size_t		chunk_size;

	for (uint8_t i = TINY; i <= LARGE; ++i) {
		current_page = g_malloc.map[i];
		while (current_page) {
			current_chunk = ptr_add(current_page, sizeof(t_page) - sizeof(t_chunk));
			chunk_size = 0;
			do {
				current_chunk = ptr_add(current_chunk, sizeof(t_chunk) + chunk_size);
				chunk_size = current_chunk->size;
				if (!check_cheksum(ptr_add(current_chunk, sizeof(t_chunk))))
					panic(2, "ERROR: page error, test for page_integrity failed.\n");
			} while (get_free_space_after_chunk(current_page, current_chunk) >= sizeof(t_chunk));
			current_page = current_page->next;
		}
	}
}

void
free_all_page(void) {
	free_page_from_map(TINY);
	free_page_from_map(SMALL);
	free_page_from_map(LARGE);
	pthread_mutex_destroy(&g_malloc.lock);
}

t_page*
get_page_from_chunk(t_chunk* chunk, uint8_t map_type) {
	t_page*	current_page;

	current_page = g_malloc.map[map_type];
	while (current_page) {
		if ((size_t)current_page < (size_t)chunk && (size_t)ptr_sub(chunk, (size_t)current_page) < current_page->size)
			return (current_page);
		current_page = current_page->next;
	}
	return (NULL);
}

void*
make_pages(uint8_t map_type, uint8_t nb_pages, t_page* last_page) {
	void*			new_page;
	size_t	const	alloc_size = PAGE_SIZE * nb_pages;

	if (!nb_pages)
		return (NULL);
	new_page = mmap(NULL, alloc_size, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	write_page_header(new_page, NULL, alloc_size);
	write_chunk_header(ptr_add(new_page, sizeof(t_page)), MAPTYPE(map_type) | FREE(1), alloc_size - sizeof(t_page) - sizeof(t_chunk));
	if (!last_page)
		g_malloc.map[map_type] = new_page;
	else
		last_page->next = new_page;
	return (g_malloc.map[map_type]);
}
