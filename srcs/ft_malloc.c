
#include "ft_malloc_intern.h"

#include <assert.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

t_malloc	g_malloc;

static void*
make_pages(uint8_t const map_type, uint8_t nb_pages, t_page* last_page) {
	void*		new_page;
	t_chunk*	current_chunk;

	if (!last_page)
		last_page = g_malloc.map[map_type];
	while (nb_pages--) {
		new_page = mmap(NULL, PAGE_SIZE, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
		if (new_page == MAP_FAILED)
			return (new_page);
		current_chunk = ptr_add(new_page, sizeof(t_page));
		write_page_header(new_page, NULL, PAGE_SIZE);
		write_chunk_header(current_chunk, CRNT_CHUNK_FREE, PAGE_SIZE - sizeof(t_page) - sizeof(t_chunk));
		last_page->next = new_page;
		last_page = new_page;
	}
	return (new_page);
}

void*
ft_malloc(size_t size) {
	void*			result;

	if (!size)
		return (NULL);
	if (size <= N)
		result = NULL;
	else if (size <= M)
		result = NULL;
	else
		result = NULL;
	if (result == MAP_FAILED)
		result = NULL;
	return (result);
}
