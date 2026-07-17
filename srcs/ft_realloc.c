
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

static void*
try_reduce(t_chunk* chunk, size_t size) {
	size_t const	chunk_size = chunk->size;
	uint8_t	const	map_type = get_map_type_flag(chunk);

	if (chunk->size == size)
		return (ptr_add(chunk, sizeof(t_chunk)));
	else if (chunk->size > size) {
		chunk->size = size;
		write_chunk_header(ptr_add(chunk, size + sizeof(t_chunk)), MAPTYPE(map_type) | FREE(1), chunk_size - sizeof(t_chunk) - size);
		return (ptr_add(chunk, sizeof(t_chunk)));
	}
	return (NULL);
}

static void*
expand_failed(t_chunk* current_chunk, size_t size) {
	void* new_ptr;

	new_ptr = malloc(size);
	if (!new_ptr)
		return (NULL);
	memcpy(new_ptr, ptr_add(current_chunk, sizeof(t_chunk)), current_chunk->size);
	free(ptr_add(current_chunk, sizeof(t_chunk)));
	return (new_ptr);
}

static void*
expand(t_chunk* current_chunk, size_t size) {
	t_page*			page;
	t_chunk*		next_chunk;
	uint8_t const	map_type = get_map_type_flag(current_chunk);
	size_t const	extra_size = size - current_chunk->size;

	page = get_page_from_chunk(current_chunk, map_type);
	if (!page)
		return (NULL);
	if (get_free_space_after_chunk(page, current_chunk) > sizeof(t_chunk)) {
		next_chunk = ptr_add(current_chunk, sizeof(t_chunk) + current_chunk->size);
		if (get_chunk_free_flag(next_chunk) && next_chunk->size >= extra_size) {
			current_chunk->size += extra_size;
			next_chunk->size -= extra_size;
			write_chunk_header(ptr_add(current_chunk, sizeof(t_chunk) + current_chunk->size), MAPTYPE(map_type) | FREE(1), next_chunk->size);
			return (ptr_add(current_chunk, sizeof(t_chunk)));
		}
	}
	return (expand_failed(current_chunk, size));
}

void*
realloc(void* ptr, size_t size) {
	void*		new_ptr;
	t_chunk*	chunk;

	if (!ptr)
		return (malloc(size));
	else if (!size) {
		free(ptr);
		return (NULL);
	}
	chunk = ptr_sub(ptr, sizeof(t_chunk));
	if (!check_cheksum(ptr) || get_chunk_free_flag(chunk)) {
		panic(STDERR_FILENO, "realloc(): invalid pointer\n");
		return (NULL);
	}
	size = align_as(size, _Alignof(void*));
	new_ptr = try_reduce(chunk, size);
	if (new_ptr)
		return (new_ptr);
	return (expand(chunk, size));
}
