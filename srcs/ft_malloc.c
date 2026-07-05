
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <assert.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stdio.h>

t_malloc	g_malloc;

static void*
allocate_large(size_t size) {
	void*			new_memory;
	t_page			page_header;
	int const		page_size = sysconf(_SC_PAGE_SIZE);
	int const		rest = size & (page_size - 1);
	size_t const	allocated_memory = (rest) ? size + page_size - (rest) : size;

	new_memory = mmap(NULL, allocated_memory, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (new_memory == MAP_FAILED)
		return (new_memory);
	page_header.chunk = ptr_add(new_memory, sizeof(t_page));
	page_header.next = g_malloc.map[LARGE];
	g_malloc.map[LARGE] = new_memory;
	*(t_page *)new_memory = page_header;
	new_memory = ptr_add(new_memory, sizeof(t_page));
	*(t_chunk *)new_memory = (t_chunk) {.size = allocated_memory, .next = NULL};
	return (ptr_add(new_memory, sizeof(t_chunk)));
}

static void*
allocate_new_pages(t_page* prev_page, size_t size) {
	void*		pages;
	int const	page_size = sysconf(_SC_PAGE_SIZE);	

	pages = mmap(NULL, page_size * INIT_ZONE_NB_PAGE_DEFAULT, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (pages == MAP_FAILED)
		return (pages);

	prev_page->next = pages;
	*(t_page *)pages = (t_page) {.chunk = ptr_add(pages, sizeof(t_page)), .next = ptr_add(pages, page_size)};
	*(t_chunk *)(ptr_add(pages, sizeof(t_page))) = (t_chunk) {.next = NULL, .size = size};
	*(t_page *)(ptr_add(pages, page_size)) = (t_page) {.chunk = NULL, .next = NULL};
	return (ptr_add(pages, sizeof(t_page) + sizeof(t_chunk)));
}

static void*
insert_new_chunk(t_chunk* current_chunk, t_chunk* next_chunk, size_t size) {
	void*	new_chunk;

	new_chunk = ptr_add(current_chunk, sizeof(t_chunk) + current_chunk->size);
	current_chunk->next = new_chunk;
	*(t_chunk *)new_chunk = (t_chunk) {.size = size, .next = next_chunk};
	return (ptr_add(new_chunk, sizeof(t_chunk)));
}

void*
alloc(size_t size, int type) {
	t_page*		prev_page;
	t_page*		current_page;
	t_chunk*	current_chunk;
	t_chunk*	next_chunk;

	current_page = (type == TINY) ? g_malloc.map[TINY] : g_malloc.map[SMALL];
	if (!current_page)
		return (init_zone(type, size));
	while (current_page) {
		current_chunk = current_page->chunk;
		if (!current_chunk || (size_t)current_chunk - ((size_t)current_page + sizeof(t_page)) >= size)
			return (insert_new_chunk_after_page_header(current_page, size));
		while (current_chunk) {
			next_chunk = current_chunk->next;
			if (get_free_space_between(current_page, current_chunk, next_chunk) >= size)
				return (insert_new_chunk(current_chunk, next_chunk, size));
			current_chunk = current_chunk->next;
		}
		prev_page = current_page;
		current_page = current_page->next;
	}
	return (allocate_new_pages(prev_page, size));
}

void*
ft_malloc(size_t size) {
	void*			result;
	uint8_t	const	rest = size & (_Alignof(void*) - 1);

	if (!size)
		return (NULL);
	size = (rest) ? size + _Alignof(void*) - rest : size;
	if (size <= N)
		result = alloc(size, TINY);
	else if (size <= M)
		result = alloc(size, SMALL);
	else
		result = allocate_large(size);
	if (result == MAP_FAILED)
		result = NULL;
	return (result);
}

int main(void) {
	void* ptr = ft_malloc(5);
	void* ptr2 = ft_malloc(7);
	void* PTR = ft_malloc(7458458);
	void* PTR2 = ft_malloc(7458434558);

	show_alloc_mem();
	printf("\n\n\n");

	ft_free(ptr);
	ft_free(ptr2);
	ft_free(PTR);
	show_alloc_mem();

	printf("\n\n\n");
	ft_free(PTR2);

	show_alloc_mem();
	printf("\n\n\n");

	return (0);
}
