#ifndef FT_MALLOC_INTERN
# define FT_MALLOC_INTERN

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <stdio.h>
# include <unistd.h>

# define INIT_ZONE_NB_PAGE_DEFAULT 2
# define MAX_PAGE_BEFOR_FREE 3
# define N 256
# define M 2048

# define CHECKSUM(ptr, NB_FLAGS) (((size_t)ptr >> NB_FLAGS) << NB_FLAGS)
# define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

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

# define NUMBERS_OF_FLAGS 1 /* counted manually, keep these in sync with t_chunk_flags, dont count END_FLAGS IT NOT A FLAG*/

/**
 * @brief this enum contains flags chunk.
 * each value must contain where is the bit! so 2^n, store in the enum 'n'.
 * END_FLAGS must be the last 'flags' in the enum, it used for algo, do not use it like a traditional flag.
 * 
 */
typedef enum e_chunk_flags {
	CRNT_CHUNK_FREE = 1,
	END_FLAGS = 2
} t_chunk_flags;

static inline void*
ptr_add(void const* p, size_t const n) {
	return (uchar*)p + n;
}

static inline void*
ptr_sub(void const* p, size_t const n) {
	return (uchar*)p - n;
}

static inline size_t
get_free_space_after_chunk(t_page const* current_page, t_chunk const* current_chunk) {
	const void* end_addrs_chunk = ptr_add(current_chunk, sizeof(t_chunk) + current_chunk->size);
	return (current_page->size - (size_t)ptr_sub(end_addrs_chunk, (size_t)current_page));
}

static inline void
write_page_header(t_page* page, void* next, size_t const size) {
	page->next = next;
	page->size = size;
}

static inline void
write_chunk_header(t_chunk* chunk, size_t const flags, size_t const size) {
	chunk->size = size;
	chunk->flags = CHECKSUM(chunk, NUMBERS_OF_FLAGS) | (flags & (END_FLAGS - 1));
}

static inline bool
get_crnt_chunk_free_flag(t_chunk* current_chunk) {
	return (current_chunk->flags & CRNT_CHUNK_FREE);
}

static inline void
set_crnt_chunk_free(t_chunk* current_chunk, bool const state) {
	current_chunk->flags = (state) ? current_chunk->flags | CRNT_CHUNK_FREE : current_chunk->flags & ~CRNT_CHUNK_FREE;
}

static inline bool
check_cheksum(void const* ptr) {
	t_chunk const*	current_chunk;

	current_chunk = ptr_sub(ptr, sizeof(t_chunk));
	return (CHECKSUM(current_chunk->flags, NUMBERS_OF_FLAGS) == CHECKSUM(current_chunk, NUMBERS_OF_FLAGS));
}

/**
 * @brief return aligned size
 * 
 * @param size the size to align
 * @param align align to ..., WARN it must be 2^n
 * @return size_t aligned size
 */
static inline size_t
align_as(size_t size, uint16_t align) {
	uint16_t const	rest = size & (align - 1);

	size = (rest) ? size + align - rest : size;
	return (size);
}

void*	make_pages(uint8_t const map_type, uint8_t nb_pages, t_page* last_page);
void*	alloc_large_page(size_t size);

extern t_malloc g_malloc;

#endif
