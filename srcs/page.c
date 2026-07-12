
#include "ft_malloc_intern.h"

#include <sys/mman.h>
#include <assert.h>

static void*
alloc_one_page(void) {
	void*		new_page;
	t_chunk*	current_chunk;

	new_page = mmap(NULL, PAGE_SIZE, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	current_chunk = ptr_add(new_page, sizeof(t_page));
	write_page_header(new_page, NULL, PAGE_SIZE);
	write_chunk_header(current_chunk, CRNT_CHUNK_FREE, PAGE_SIZE - sizeof(t_page) - sizeof(t_chunk));
	return (new_page);
}

void*
make_pages(uint8_t const map_type, uint8_t nb_pages, t_page* last_page) {
	void*	new_page;
	// void*	first_alloc_page;

	if (!nb_pages)
		return (NULL);
	if (!last_page) {
		assert(g_malloc.map[map_type] == NULL);			// si last_page == NULL, alors, g_malloc.map[map_type] doit etre NULL
		new_page = alloc_one_page();
		if (!new_page)
			return (NULL);
		g_malloc.map[map_type] = new_page;
		last_page = new_page;
		--nb_pages;
	}
	while (nb_pages--) {
		new_page = alloc_one_page();
		if (!new_page)
			return (NULL);
		last_page->next = new_page;
		last_page = new_page;
	}
	return (g_malloc.map[map_type]);
}
