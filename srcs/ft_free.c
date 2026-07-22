
#include "ft_malloc_intern.h"

#include <sys/mman.h>
#include <stdio.h>

static void
free_large(t_page* page) {
	t_page*	current_page;
	t_page*	prev_page;

	prev_page = NULL;
	current_page = g_malloc.map[LARGE];
	while (current_page) {
		if (current_page == page) {
			if (!prev_page)
				g_malloc.map[LARGE] = current_page->next;
			else
				prev_page->next = current_page->next;
			munmap(page, page->size);
			--g_malloc.total_alloc;
			return ;
		}
		prev_page = current_page;
		current_page = current_page->next;
	}
	panic(STDERR_FILENO, "free(): invalid pointer\n");
}

static void
merge_free_chunk(uint8_t map_type) {
	t_page*		current_page;
	t_chunk*	current_chunk;
	t_chunk*	next_chunk;

	current_page = g_malloc.map[map_type];
	while (current_page) {
		current_chunk = ptr_add(current_page, sizeof(t_page));
		while (get_free_space_after_chunk(current_page, current_chunk)) {
			next_chunk = ptr_add(current_chunk, sizeof(t_chunk) + current_chunk->size);
			current_chunk->size = (get_chunk_free_flag(current_chunk) && get_chunk_free_flag(next_chunk)) ? current_chunk->size + sizeof(t_chunk) + next_chunk->size : current_chunk->size;
			current_chunk = (get_chunk_free_flag(current_chunk) && get_chunk_free_flag(next_chunk)) ? current_chunk : next_chunk;
		}
		current_page = current_page->next;
	}

}

static void
check_for_munmap(uint8_t map_type) {
	t_page*		prev_page;
	t_page*		current_page;
	t_chunk*	current_chunk;
	size_t		nb_empty_page;

	nb_empty_page = 0;
	prev_page = NULL;
	current_page = g_malloc.map[map_type];
	while (current_page) {
		current_chunk = ptr_add(current_page, sizeof(t_page));
		if (current_chunk->size == (current_page->size - sizeof(t_page) - sizeof(t_chunk)) && get_chunk_free_flag(current_chunk)) {
			++nb_empty_page;
			if (nb_empty_page > MAX_PAGE_BEFOR_FREE) {
				if (!prev_page)
					g_malloc.map[map_type] = current_page->next;
				else
					prev_page->next = current_page->next;
				munmap(current_page, current_page->size);
				return ;
			}
		}
		current_page = current_page->next;
	}
}

static bool
check_pointeur(void* ptr, t_page** page, t_chunk** chunk) {
	if (!ptr)
		return (1);
	if (!check_cheksum(ptr)) {
		panic(STDERR_FILENO, "free(): invalid pointer\n");
		return (1);
	}
	*chunk = ptr_sub(ptr, sizeof(t_chunk));
	if (get_chunk_free_flag(*chunk)) {
		panic(STDERR_FILENO, "free(): double free detected\n");
		return (1);
	}
	*page = get_page_from_chunk(*chunk, get_map_type_flag(*chunk));
	if (!*page) {
		panic(STDERR_FILENO, "ERROR: internal error\n");
		return (1);
	}
	return (0);
}

void
free(void* ptr) {
	t_chunk*	chunk;
	t_page*		page;
	uint8_t		map_type;

	if (check_pointeur(ptr, &page, &chunk))
		return ;
	map_type = get_map_type_flag(chunk);
	if (map_type == LARGE) {
		free_large(page);
		if (!g_malloc.total_alloc)
			free_all_page();
		return ;
	}
	--g_malloc.total_alloc;
	set_chunk_free_flag(chunk, 1);
	merge_free_chunk(map_type);
	check_for_munmap(map_type);
	if (!g_malloc.total_alloc) {
		free_all_page();
		return ;
	}
}
