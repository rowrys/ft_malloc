
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

t_malloc	g_malloc;

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

static void*
make_pages(uint8_t const map_type, uint8_t nb_pages, t_page* last_page) {
	void*	new_page;

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

static void*
alloc(uint8_t map_type, size_t size) {
	t_page*		prev_page;
	t_page*		current_page;
	t_chunk*	current_chunk;
	size_t		chunk_size;

	prev_page = NULL;
	current_page = g_malloc.map[map_type];
	if (!current_page)
		current_page = make_pages(map_type, INIT_ZONE_NB_PAGE_DEFAULT, NULL);
	if (!current_page)
		return (NULL);
	while (current_page) {
		current_chunk = ptr_add(current_page, sizeof(t_page) - sizeof(t_chunk));
		chunk_size = 0;
		do {
			current_chunk = ptr_add(current_chunk, sizeof(t_chunk) + chunk_size);
			chunk_size = current_chunk->size;
			if (get_crnt_chunk_free_flag(current_chunk) && chunk_size >= size) {
				set_crnt_chunk_free(current_chunk, 0);
				current_chunk->size = size;
				if (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk))
					write_chunk_header(ptr_add(current_chunk, current_chunk->size + sizeof(t_chunk)), CRNT_CHUNK_FREE, chunk_size - sizeof(t_chunk) * 2 - size);
				return (ptr_add(current_chunk, sizeof(t_chunk)));
			}
		} while (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk));
		prev_page = current_page;
		current_page = current_page->next;
	}
	if (!make_pages(map_type, INIT_ZONE_NB_PAGE_DEFAULT, prev_page))
		return (NULL);
	return (NULL);
}

void*
ft_malloc(size_t size) {
	void*	result;

	if (!size)
		return (NULL);
	size = align_as(size, _Alignof(void*));
	if (size <= N)
		result = alloc(TINY, size);
	else if (size <= M)
		result = alloc(SMALL, size);
	else
		result = NULL;
	return (result);
}

int
main(void) {
	show_alloc_mem();
	void* ptr = ft_malloc(14);
	(void)ptr;
	show_alloc_mem();
	return (0);
}
