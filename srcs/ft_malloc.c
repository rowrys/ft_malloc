
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

t_malloc	g_malloc;

static void*
split_chunk(t_page* current_page, t_chunk* current_chunk, size_t size, size_t chunk_size) {
	set_crnt_chunk_free(current_chunk, 0);
	current_chunk->size = size;
	if (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk))
		write_chunk_header(ptr_add(current_chunk, current_chunk->size + sizeof(t_chunk)), CRNT_CHUNK_FREE, chunk_size - sizeof(t_chunk) - size);
	return (ptr_add(current_chunk, sizeof(t_chunk)));
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
			if (get_crnt_chunk_free_flag(current_chunk) && chunk_size >= size)
				return (split_chunk(current_page, current_chunk, size, chunk_size));
		} while (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk));
		prev_page = current_page;
		current_page = current_page->next;
	}
	if (!make_pages(map_type, INIT_ZONE_NB_PAGE_DEFAULT, prev_page))
		return (NULL);
	current_page = prev_page->next;
	return (split_chunk(current_page, ptr_add(current_page, sizeof(t_page)), size, PAGE_SIZE - sizeof(t_page) - sizeof(t_chunk)));
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
		result = alloc_large_page(size);
	return (result);
}

int
main(void) {
	show_alloc_mem();
	for (int i = 0; i <= 127; i++) {		// i <= 127 to make chunk on other page
		ft_malloc(16);
	}
	ft_malloc(5445544545);
	show_alloc_mem();
	return (0);
}
