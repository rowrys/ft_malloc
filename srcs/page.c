
#include "ft_malloc_intern.h"

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>

static void*
alloc_one_page(uint8_t map_type) {
	void*		new_page;
	t_chunk*	current_chunk;

	new_page = mmap(NULL, PAGE_SIZE, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	current_chunk = ptr_add(new_page, sizeof(t_page));
	write_page_header(new_page, NULL, PAGE_SIZE);
	write_chunk_header(current_chunk,  MAPTYPE(map_type) | FREE(1), PAGE_SIZE - sizeof(t_page) - sizeof(t_chunk));
	return (new_page);
}

void*
alloc_large_page(size_t size) {
	t_chunk*	current_chunk;
	size_t		alloc_size;
	void*		new_page;

	alloc_size = align_as(sizeof(t_page) + sizeof(t_chunk) + size, PAGE_SIZE);
	new_page = mmap(NULL, alloc_size, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	current_chunk = ptr_add(new_page, sizeof(t_page));
	write_page_header(new_page, NULL, alloc_size);
	write_chunk_header(current_chunk, MAPTYPE(LARGE), alloc_size - sizeof(t_page) - sizeof(t_chunk));
	if (!g_malloc.map[LARGE])
		g_malloc.map[LARGE] = new_page;
	else
		g_malloc.map[LARGE]->next = new_page;
	return (ptr_add(current_chunk, sizeof(t_chunk)));
}

void*
make_pages(uint8_t const map_type, uint8_t nb_pages, t_page* last_page) {
	void*	new_page;

	if (!nb_pages)
		return (NULL);
	if (!last_page) {
		assert(g_malloc.map[map_type] == NULL);			// si last_page == NULL, alors, g_malloc.map[map_type] doit etre NULL
		new_page = alloc_one_page(map_type);
		if (!new_page)
			return (NULL);
		g_malloc.map[map_type] = new_page;
		last_page = new_page;
		--nb_pages;
	}
	while (nb_pages--) {
		new_page = alloc_one_page(map_type);
		if (!new_page)
			return (NULL);
		last_page->next = new_page;
		last_page = new_page;
	}
	return (g_malloc.map[map_type]);
}
