
#include "ft_malloc_intern.h"
#include "ft_printf.h"

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

size_t
get_free_space_between(void* current_page, t_chunk* start_addr, t_chunk* end_addr) {
	static_assert(sizeof(t_chunk) == sizeof(t_page), "sizeof(t_chunk) need to be equal to sizeof(t_page)\n");
	if (!end_addr)
		end_addr = ptr_add(current_page, sysconf(_SC_PAGE_SIZE));
	return ((size_t)end_addr - (size_t)(ptr_add(start_addr, sizeof(t_page) + start_addr->size)));
}

void*
init_zone(uint8_t type, size_t size) {
	void*		pages;
	t_page**	current_page;
	int	const	page_size = sysconf(_SC_PAGE_SIZE);
	uint16_t	nb_page_init;

	current_page = (type == TINY) ? &g_malloc.map[TINY] : &g_malloc.map[SMALL];
	pages = mmap(NULL,  page_size * INIT_ZONE_NB_PAGE_DEFAULT, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (pages == MAP_FAILED)
		return (MAP_FAILED);

	*(t_page *)pages = (t_page) {.chunk = ptr_add(pages, sizeof(t_page)), .next = ptr_add(pages, page_size)};
	*(t_chunk *)(ptr_add(pages, sizeof(t_page))) = (t_chunk) {.next = NULL, .size = size};
	nb_page_init = 1;
	while (nb_page_init < INIT_ZONE_NB_PAGE_DEFAULT) {
		*(t_page *)(ptr_add(pages, page_size * nb_page_init)) = (t_page) {.chunk = NULL, .next = ptr_add(pages, page_size * (nb_page_init + 1))};
		++nb_page_init;
	}
	*(t_page *)(ptr_add(pages, page_size * (nb_page_init - 1))) = (t_page) {.chunk = NULL, .next = NULL};
	*current_page = pages;
	return (ptr_add(pages, sizeof(t_page) + sizeof(t_chunk)));
}

void*
insert_new_chunk_after_page_header(void* current_page, size_t size) {
	*(t_chunk *)(ptr_add(current_page, sizeof(t_page))) = (t_chunk) {.next = ((t_page *)current_page)->chunk, .size = size};
	((t_page *)current_page)->chunk = ptr_add(current_page, sizeof(t_page));
	return (ptr_add(current_page, sizeof(t_page) + sizeof(t_chunk)));
}

static void
display_map(t_page* current_page) {
	t_chunk*	current_chunk;

	while (current_page) {
		current_chunk = current_page->chunk;
		while (current_chunk) {
			printf("%p - %p: %lu bytes\n", ptr_add(current_chunk, sizeof(t_chunk)), ptr_add(current_chunk, current_chunk->size), current_chunk->size);
			current_chunk = current_chunk->next;
		}
		current_page = current_page->next;
	}
}

void
show_alloc_mem(void) {
	printf("TINY : %p\n", (void*)g_malloc.map[TINY]);
	display_map(g_malloc.map[TINY]);
	ft_printf("SMALL : %p\n", (void*)g_malloc.map[SMALL]);
	display_map(g_malloc.map[SMALL]);
	ft_printf("LARGE : %p\n", (void*)g_malloc.map[LARGE]);
	display_map(g_malloc.map[LARGE]);
}
