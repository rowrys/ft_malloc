
#include "ft_malloc.h"

#include <assert.h>
#include <linux/limits.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

t_malloc	g_malloc;

static inline size_t	check_free_space_betwin(void *start_addr, void *end_addr) {
	return (end_addr - start_addr);
}

static inline void*
alloc_news_pages_in_zone(size_t nb_pages, t_page* end_page) {


	// page_header = (t_page) { 0 };

	switch (zone_type) {
		case tiny: case small:
		
		case large:

	}
	return (new_memory);
}


static void*
allocate_large(size_t size) {
	void*		new_memory;
	t_page		page_header;
	size_t		allocated_memory;
	int			nb_page;
	int const	page_size = sysconf(_SC_PAGE_SIZE);

	nb_page = size / page_size + (size % page_size != 0);
	allocated_memory = nb_page * page_size;
	new_memory = mmap(NULL, allocated_memory, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (new_memory == MAP_FAILED)
		return (new_memory);
	page_header.next = g_malloc.large;
	g_malloc.large = new_memory;
	*(t_page *)new_memory = page_header;
	new_memory += sizeof(t_page);
	*(t_chunk *)new_memory = (t_chunk) {.size = allocated_memory, .next = NULL};
	return (new_memory + + sizeof(t_chunk));
}


void*
malloc(size_t size) {
	void*	resutl;

	if (!size)
		return (NULL);
	if (size <= N) {
		if (!g_malloc.tiny && alloc_news_pages(2, tiny) == MAP_FAILED)
			return (NULL);
		tant que (check_free_space_betwin() < size + sizeof(t_chunk) && current_chunk)
			current_chunk = current_chunk.next;
		si (!current_chunk) {
			allocate_new_page();
			link new_page_to_lst;
		} si non {
			put_header
			return (current_chunk + sizeof(t_chunk));
		}

	} else if (size <= M) {

	} else {
		resutl = allocate_large(size);
	}
	return (resutl);
}

#include <stdio.h>

int main(void) {

	return (0);
}