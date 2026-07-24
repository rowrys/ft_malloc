
#include "ft_malloc_intern.h"
#include "ft_printf.h"

#include <stddef.h>
#include <stdio.h>

static inline void
display_chunk(char* fmt, t_chunk* current_chunk, size_t used_mem) {
	ft_printf(fmt, ptr_add(current_chunk, sizeof(t_chunk)), ptr_add(current_chunk, sizeof(t_chunk) + used_mem), used_mem);
} 

static size_t
show_map(uint8_t map_type) {
	t_page*		current_page;
	t_chunk*	current_chunk;
	size_t		used_mem;
	size_t		chunk_size;
	size_t		total_used_mem;
	size_t		nb_pages;

	(void)nb_pages;
	total_used_mem = 0;
	nb_pages = 0;
	current_page = g_malloc.map[map_type];
	while (current_page) {
		current_chunk = ptr_add(current_page, sizeof(t_page) - sizeof(t_chunk));
		chunk_size = 0;
		do {
			current_chunk = ptr_add(current_chunk, sizeof(t_chunk) + chunk_size);
			chunk_size = current_chunk->size;
			used_mem = chunk_size;
			if (!get_chunk_free_flag(current_chunk)) {
				total_used_mem += used_mem;
				display_chunk("%p - %p : %l bytes\n", current_chunk, used_mem);
			}
			# ifdef DEBUG
			else
				display_chunk("FREE: %p - %p : %l bytes\n", current_chunk, used_mem);
			# endif

		} while (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk));
		++nb_pages;
		current_page = current_page->next;
	}
	# ifdef DEBUG
	ft_printf("numbers of pages: %l\n", nb_pages);
	# endif
	return (total_used_mem);
}

void
show_alloc_mem(void) {
	size_t	total_used_mem;

	pthread_mutex_lock(&g_malloc.lock);
	total_used_mem = 0;
	ft_printf("TINY : %p\n", (void*)g_malloc.map[TINY]);
	total_used_mem += show_map(TINY);
	ft_printf("SMALL : %p\n", (void*)g_malloc.map[SMALL]);
	total_used_mem += show_map(SMALL);
	ft_printf("LARGE : %p\n", (void*)g_malloc.map[LARGE]);
	total_used_mem += show_map(LARGE);
	ft_printf("Total : %l bytes\n", total_used_mem);
	pthread_mutex_unlock(&g_malloc.lock);
}
