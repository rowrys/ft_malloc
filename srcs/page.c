
#include "ft_malloc_intern.h"

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>

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
free_all_page(void) {
	free_page_from_map(TINY);
	free_page_from_map(SMALL);
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
