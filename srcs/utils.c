
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

extern t_malloc	g_malloc;

size_t
get_free_space_between(void *start_addr, void *end_addr) {
	static_assert(sizeof(t_chunk) == sizeof(t_page), "sizeof(t_chunk) need to be equal to sizeof(t_page)\n");
	if (!start_addr && !end_addr)
		return (sysconf(_SC_PAGE_SIZE) - sizeof(t_page));
	return (end_addr - (start_addr + sizeof(t_page)));
}

void*
init_zone(int zone_type, size_t size) {
	void*		pages;
	t_page**	current_page;
	int			page_size;

	switch (zone_type) {
		case TINY:
			current_page = &g_malloc.tiny;
			break;
		case SMALL:
			current_page = &g_malloc.small;
			break;
		default:
			return (MAP_FAILED);
	}
	page_size = sysconf(_SC_PAGE_SIZE);
	pages = mmap(NULL,  page_size * INIT_ZONE_NB_PAGE_DEFAULT, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (pages == MAP_FAILED)
		return (pages);

	*(t_page *)pages = (t_page) {.chunk = NULL, .next = pages + page_size};
	size = size + sizeof(size_t) - (size & (sizeof(size_t) - 1));
	*(t_chunk *)(pages + sizeof(t_page)) = (t_chunk) {.next = NULL, .size = size};
	*(t_page *)(pages + page_size) = (t_page) {.chunk = NULL, .next = NULL};
	*current_page = pages;
	return (pages + sizeof(t_page));
}
