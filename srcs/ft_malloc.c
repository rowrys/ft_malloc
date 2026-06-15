
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <assert.h>
#include <linux/limits.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stdio.h>

t_malloc	g_malloc;

static void*
allocate_large(size_t size) {
	void*		new_memory;
	t_page		page_header;
	size_t		allocated_memory;
	int const	page_size = sysconf(_SC_PAGE_SIZE);

	allocated_memory = size + page_size - (size & (page_size - 1));
	new_memory = mmap(NULL, allocated_memory, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (new_memory == MAP_FAILED)
		return (new_memory);
	page_header.next = g_malloc.large;
	g_malloc.large = new_memory;
	*(t_page *)new_memory = page_header;
	new_memory += sizeof(t_page);
	*(t_chunk *)new_memory = (t_chunk) {.size = allocated_memory, .next = NULL};
	return (new_memory + sizeof(t_chunk));
}

void*
alloc_tiny(size_t size) {
	void*	result;
	t_page*	current_page;
	t_chunk* prev_chunk;
	t_chunk* current_chunk;
	t_chunk* next_chunk;

	result = NULL;
	current_page = g_malloc.tiny;
	if (!g_malloc.tiny) {
		result = init_zone(TINY, size) + sizeof(t_chunk);
		return (result);
	}
	current_page = g_malloc.tiny;
	prev_chunk = NULL;
	current_chunk = current_page->chunk;
	while (current_page) {
		next_chunk = NULL;

		while (current_chunk) {
			
		}
		current_page = current_page->next;
	}
	return (result + sizeof(t_chunk));
}


void*
ft_malloc(size_t size) {
	void*	resutl;

	if (!size)
		return (NULL);
	// if (size <= N)
	resutl = alloc_tiny(size);
	// else if (size <= M)
	// 	resutl = allocate_small(size);
	// else
	// 	resutl = allocate_large(size);
	return (resutl);
}

int main(void) {
	t_chunk*	test;
	void* ptr = ft_malloc(5);

	test = ptr - sizeof(t_chunk);
	printf("%lu\n", test->size);
	return (0);
}
