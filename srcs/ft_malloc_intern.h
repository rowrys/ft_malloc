#ifndef FT_MALLOC_INTERN
# define FT_MALLOC_INTERN

# include <stdbool.h>
# include <stddef.h>
#include <stdio.h>

# define INIT_ZONE_NB_PAGE_DEFAULT 2
# define MAX_PAGE_BEFOR_FREE 3
# define N 256
# define M 2048

# define MALLOC_PROT_MMAP PROT_READ | PROT_WRITE
# define MALLOC_FLAG_MMAP MAP_ANONYMOUS | MAP_PRIVATE

typedef unsigned char uchar;

typedef struct s_page {
	void	*next;
	size_t	size;
}	t_page;

typedef struct s_chunk {
	size_t	size;
	size_t	flags;
}	t_chunk;

typedef struct s_malloc {
	t_page	*map[3];
}	t_malloc;

typedef enum e_memory_zone {
	TINY,
	SMALL,
	LARGE,
} t_memory_zone;

typedef enum e_chunk_flags {
	CRNT_CHUNK_FREE,
} t_chunk_flags;

static inline void*
ptr_add(void const* p, size_t const n) {
	return (uchar*)p + n;
}

static inline void*
ptr_sub(void const* p, size_t const n) {
	return (uchar*)p - n;
}

static inline bool
can_check_next_chunk(t_page const* current_page, t_chunk const* current_chunk) {
	const void* end_addrs_chunk = ptr_add(current_chunk, sizeof(t_chunk) * 2 + current_chunk->size);
	return ((size_t)ptr_sub(end_addrs_chunk, (size_t)current_page) < current_page->size);
}

extern t_malloc g_malloc;

#endif
