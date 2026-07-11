
#include "ft_malloc_intern.h"

#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>

static void*
make_pages(uint8_t map_type, uint8_t nb_pages, t_page* prev_page) {
	(void)map_type;
	(void)nb_pages;
	(void)prev_page;
	return (NULL);
}


void*
ft_malloc(size_t size) {
	void*			result;
	uint8_t	const	rest = size & (_Alignof(void*) - 1);

	if (!size)
		return (NULL);
	size = (rest) ? size + _Alignof(void*) - rest : size;
	if (size <= N)
		result = NULL;
	else if (size <= M)
		result = NULL;
	else
		result = NULL;
	if (result == MAP_FAILED)
		result = NULL;
	return (result);
}
