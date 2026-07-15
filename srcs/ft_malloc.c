
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

t_malloc	g_malloc;

static void*
split_chunk(t_page* current_page, t_chunk* current_chunk, size_t size, uint8_t map_type) {
	size_t const chunk_size = current_chunk->size;

	set_crnt_chunk_free_flag(current_chunk, 0);
	current_chunk->size = size;
	if (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk))
		write_chunk_header(ptr_add(current_chunk, size + sizeof(t_chunk)), MAPTYPE(map_type) | FREE(1), chunk_size - sizeof(t_chunk) - size);
	return (ptr_add(current_chunk, sizeof(t_chunk)));
}

static void*
alloc_large(size_t size) {
	t_chunk*	chunk;
	t_chunk*	result;
	t_page*		new_page;
	size_t		alloc_size;

	alloc_size = align_as(sizeof(t_page) + (sizeof(t_chunk) * 2) + size, PAGE_SIZE);
	new_page = mmap(NULL, alloc_size, MALLOC_PROT_MMAP, MALLOC_FLAG_MMAP, -1, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	chunk = ptr_add(new_page, sizeof(t_page));
	write_page_header(new_page, NULL, alloc_size);
	write_chunk_header(chunk, MAPTYPE(LARGE), size);
	result = ptr_add(chunk, sizeof(t_chunk));
	chunk = ptr_add(chunk, size + sizeof(t_chunk));
	write_chunk_header(chunk, MAPTYPE(LARGE) | FREE(1), alloc_size - size - (sizeof(t_chunk) * 2) - sizeof(t_page));
	new_page->next = g_malloc.map[LARGE];
	g_malloc.map[LARGE] = new_page;
	return (result);
}

static void*
alloc(uint8_t map_type, uint8_t nb_page, size_t size) {
	t_page*		prev_page;
	t_page*		current_page;
	t_chunk*	current_chunk;
	size_t		chunk_size;

	prev_page = NULL;
	current_page = g_malloc.map[map_type];
	if (!current_page)
		current_page = make_pages(map_type, nb_page, NULL);
	if (!current_page)
		return (NULL);
	while (current_page) {
		current_chunk = ptr_add(current_page, sizeof(t_page) - sizeof(t_chunk));
		chunk_size = 0;
		do {
			current_chunk = ptr_add(current_chunk, sizeof(t_chunk) + chunk_size);
			chunk_size = current_chunk->size;
			if (get_crnt_chunk_free_flag(current_chunk) && chunk_size >= size)
				return (split_chunk(current_page, current_chunk, size, map_type));
		} while (get_free_space_after_chunk(current_page, current_chunk) > sizeof(t_chunk));
		prev_page = current_page;
		current_page = current_page->next;
	}
	if (!make_pages(map_type, nb_page, prev_page))
		return (NULL);
	current_page = prev_page->next;
	return (split_chunk(current_page, ptr_add(current_page, sizeof(t_page)), size, map_type));
}

void*
ft_malloc(size_t size) {
	void*	result;
	size_t	align_size;

	align_size = align_as(size, _Alignof(void*));
	if (!size || align_size < size)
		return (NULL);
	if (align_size <= max_tiny)
		result = alloc(TINY, N, align_size);
	else if (align_size <= max_small)
		result = alloc(SMALL, M, align_size);
	else
		result = alloc_large(align_size);
	g_malloc.total_alloc = (result) ? g_malloc.total_alloc + 1 : g_malloc.total_alloc;
	return (result);
}

int
main(void) {
    // void* a = ft_malloc(1);
	// void* b = ft_malloc(2);
    // void* c = ft_malloc(4);
    // void* d = ft_malloc(8);
    // void* e = ft_malloc(16);
    // void* f = ft_malloc(32);
    // void* g = ft_malloc(64);
    // void* h = ft_malloc(128);
    // void* i = ft_malloc(256);
    // void* j = ft_malloc(512);
    // void* k = ft_malloc(1024);
    // void* l = ft_malloc(1024 * 2);
    // void* m = ft_malloc(1024 * 4); 
    // void* n = ft_malloc(1024 * 32);
    // void* o = ft_malloc(max_small);
    // void* p = ft_malloc(16*max_small);
    // void* q = ft_malloc(128*max_small);
	// if (!a || !b || !c || !d || !e || !f || !g || !h || !i || !j || !m || !n || !o || !l || !k || !p || !q)
	// 	return (1);
    // show_alloc_mem();
	// ft_free(a);
	// ft_free(b);
	// ft_free(c);
	// ft_free(d);
	// ft_free(e);
	// ft_free(f);
	// ft_free(g);
	// ft_free(h);
	// ft_free(i);
	// ft_free(j);
	// ft_free(k);
	// ft_free(l);
	// ft_free(m);
	// ft_free(n);
	// ft_free(o);
	// ft_free(p);
	// ft_free(q);
	// printf("\n\n\n\n\n");
    // show_alloc_mem();
	{
		char *addr;

		addr = ft_malloc(16);
		ft_free(NULL);
		ft_free((void *)((size_t)addr + 5));
		if (ft_realloc((void *)((size_t)addr + 5), 10) == NULL)
			printf("Bonjours\n");
		ft_free(addr);
	}

	return (0);
}
