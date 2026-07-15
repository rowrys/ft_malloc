
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

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
	write(2, "ft_free(): invalid pointer\n", 27);
	kill(0, SIGABRT);
}

static void
merge_free_chunk(t_page* page, t_chunk* chunk) {
	t_chunk*	next_chunk;

	if (!(get_free_space_after_chunk(page, chunk) > sizeof(t_chunk)))
		return ;
	next_chunk = ptr_add(chunk, sizeof(t_chunk) + chunk->size);
	chunk->size = (get_crnt_chunk_free_flag(next_chunk)) ? chunk->size + sizeof(t_chunk) + next_chunk->size : chunk->size;
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
		if (current_chunk->size == (current_page->size - sizeof(t_page) - sizeof(t_chunk))) {
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

void
ft_free(void* ptr) {
	t_chunk*	chunk;
	t_page*		page;
	uint8_t		map_type;

	if (!ptr)
		return ;
	if (!check_cheksum(ptr)) {
		write(2, "ft_free(): invalid pointer\n", 27);
		kill(0, SIGABRT);
		return ;
	}
	chunk = ptr_sub(ptr, sizeof(t_chunk));
	if (get_crnt_chunk_free_flag(chunk)) {
		write(2, "ft_free(): double free detected\n", 32);
		kill(0, SIGABRT);
		return ;
	}
	map_type = get_map_type_flag(chunk);
	page = get_page_from_chunk(chunk, map_type);
	if (!page) {
		write(2, "ERROR: internal error\n", 22);
		kill(0, SIGABRT);
		return ;
	}
	if (map_type == LARGE) {
		free_large(page);
		if (!g_malloc.total_alloc)
			free_all_page();
		return ;
	}
	--g_malloc.total_alloc;
	set_crnt_chunk_free_flag(chunk, 1);
	merge_free_chunk(page, chunk);
	check_for_munmap(map_type);
	if (!g_malloc.total_alloc) {
		free_all_page();
		return ;
	}
}
