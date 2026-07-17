
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <stdio.h>
#include <sys/mman.h>

t_malloc	g_malloc;

static void*
split_chunk(t_page* current_page, t_chunk* current_chunk, size_t size, uint8_t map_type) {
	size_t const chunk_size = current_chunk->size;

	set_chunk_free_flag(current_chunk, 0);
	current_chunk->size = size;
	if (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk))
		write_chunk_header(ptr_add(current_chunk, size + sizeof(t_chunk)), MAPTYPE(map_type) | FREE(1), chunk_size - sizeof(t_chunk) - size);
	return (ptr_add(current_chunk, sizeof(t_chunk)));
}

static void*
alloc_large(size_t size) {
	t_chunk*	chunk;
	t_chunk*	result;
	t_page*		new_page;
	size_t		alloc_size;

	alloc_size = align_as(sizeof(t_page) + (sizeof(t_chunk) * 2) + size, PAGE_SIZE);
	new_page = mmap(NULL, alloc_size, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	chunk = ptr_add(new_page, sizeof(t_page));
	write_page_header(new_page, NULL, alloc_size);
	write_chunk_header(chunk, MAPTYPE(LARGE), size);
	result = ptr_add(chunk, sizeof(t_chunk));
	chunk = ptr_add(chunk, size + sizeof(t_chunk));
	write_chunk_header(chunk, MAPTYPE(LARGE) | FREE(1), alloc_size - size - (sizeof(t_chunk) * 2) - sizeof(t_page));
	new_page->next = g_malloc.map[LARGE];
	g_malloc.map[LARGE] = new_page;
	return (result);
}

static void*
alloc(uint8_t map_type, uint8_t nb_page, size_t size) {
	t_page*		prev_page;
	t_page*		current_page;
	t_chunk*	current_chunk;
	size_t		chunk_size;

	prev_page = NULL;
	current_page = g_malloc.map[map_type];
	if (!current_page)
		current_page = make_pages(map_type, nb_page, NULL);
	if (!current_page)
		return (NULL);
	while (current_page) {
		current_chunk = ptr_add(current_page, sizeof(t_page) - sizeof(t_chunk));
		chunk_size = 0;
		do {
			current_chunk = ptr_add(current_chunk, sizeof(t_chunk) + chunk_size);
			chunk_size = current_chunk->size;
			if (get_chunk_free_flag(current_chunk) && chunk_size >= size)
				return (split_chunk(current_page, current_chunk, size, map_type));
		} while (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk));
		prev_page = current_page;
		current_page = current_page->next;
	}
	if (!make_pages(map_type, nb_page, prev_page))
		return (NULL);
	current_page = prev_page->next;
	return (split_chunk(current_page, ptr_add(current_page, sizeof(t_page)), size, map_type));
}

void*
malloc(size_t size) {
	void*	result;
	size_t	align_size;

	align_size = align_as(size, _Alignof(void*));
	if (!size || align_size < size)
		return (NULL);
	if (align_size <= max_tiny)
		result = alloc(TINY, N, align_size);
	else if (align_size <= max_small)
		result = alloc(SMALL, M, align_size);
	else
		result = alloc_large(align_size);
	g_malloc.total_alloc = (result) ? g_malloc.total_alloc + 1 : g_malloc.total_alloc;
	return (result);
}
