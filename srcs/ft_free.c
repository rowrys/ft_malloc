
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

static inline void
remove_chunk(t_page* current_page, t_chunk* prev_chunk, t_chunk* current_chunk) {
	if (!prev_chunk)
		current_page->chunk = current_chunk->next;
	else
		prev_chunk->next = current_chunk->next;
}

static inline void
remove_page(t_page* prev_last_empty_page, t_page* page, uint8_t type) {
	if (prev_last_empty_page)
		prev_last_empty_page->next = page->next;
	if (page == g_malloc.map[type])
		g_malloc.map[type] = NULL;
	munmap(page, sysconf(_SC_PAGE_SIZE));
}

static void
check_for_remove_page(t_page* prev_last_empty_page, t_page* prev_page, t_page* page, size_t nb_free_page, uint8_t type) {
	t_page*	last_empty_page;

	while (page->next) {
		if (!page->chunk) {
			++nb_free_page;
			prev_last_empty_page = prev_page;
			last_empty_page = page;
		}
		prev_page = page;
		page = page->next;
	}
	if (!page->chunk) {
		++nb_free_page;
		prev_last_empty_page = prev_page;
		last_empty_page = page;
	}
	if (nb_free_page >= MAX_PAGE_BEFOR_FREE || type == LARGE)
		remove_page(prev_last_empty_page, last_empty_page, type);
}

static bool
remove_from_type(t_page* current_page, void* ptr, uint8_t type) {
	size_t		nb_free_page;
	t_page*		prev_page;
	t_page*		prev_last_empty_page;
	t_chunk*	current_chunk;
	t_chunk*	prev_chunk;

	nb_free_page = 0;
	prev_page = NULL;
	while (current_page) {
		if (!current_page->chunk) {
			++nb_free_page;
			prev_last_empty_page = prev_page;
		}
		prev_chunk = NULL;
		current_chunk = current_page->chunk;
		while (current_chunk) {
			if (ptr_add(current_chunk, sizeof(t_chunk)) == ptr) {
				remove_chunk(current_page, prev_chunk, current_chunk);
				check_for_remove_page(prev_last_empty_page, prev_page, current_page, nb_free_page, type);
				return (1);
			}
			prev_chunk = current_chunk;
			current_chunk = current_chunk->next;
		}
		prev_page = current_page;
		current_page = current_page->next;
	}
	return (0);
}

void
ft_free(void* ptr) {
	if (!ptr)
		return ;
	for (uint8_t i = TINY; i <= LARGE; ++i) {
		if (remove_from_type(g_malloc.map[i], ptr, i))
			break ;
	}
}
