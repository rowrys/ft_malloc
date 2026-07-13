
#include "ft_malloc.h"
#include "ft_malloc_intern.h"

#include <signal.h>
#include <sys/mman.h>

static void
free_large(t_page* page) {
	t_page*	current_page;

	current_page = g_malloc.map[LARGE];
	while (current_page) {
		if (current_page == page) {
			munmap(page, page->size);		
			return ;
		}
		current_page = current_page->next;
	}
	write(2, "ft_free(): invalid pointer\n", 27);
	kill(0, SIGABRT);
}

void
ft_free(void* ptr) {
	t_chunk*	chunk;
	t_page*		page;

	if (!ptr)
		return ;
	if (!check_cheksum(ptr)) {
		write(2, "ft_free(): invalid pointer\n", 27);
		kill(0, SIGABRT);
		return ;
	}
	chunk = ptr_sub(ptr, sizeof(t_chunk));
	if (get_crnt_chunk_free_flag(chunk)) {
		write(2, "free(): double free detected\n", 29);
		kill(0, SIGABRT);
		return ;
	}
	page = get_page_from_chunk(chunk);
	if (chunk->size > M) {
		free_large(page);
		return ;
	}
	set_crnt_chunk_free(chunk, 1);
	// regarder si le prochain chunk est vide, si oui merge les deux chunk
}
