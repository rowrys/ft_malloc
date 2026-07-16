#ifndef FT_MALLOC_INTERN
# define FT_MALLOC_INTERN

# include <stdbool.h>
# include <stdint.h>
# include <unistd.h>
# include <string.h>
# include <signal.h>

# define MAX_PAGE_BEFOR_FREE 2
# define max_tiny 256
# define max_small 1024
# define N 7
# define M 26

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
	t_page*	map[3];
	size_t	total_alloc;
}	t_malloc;

typedef enum e_memory_zone {
	TINY,
	SMALL,
	LARGE,
} t_memory_zone;

// on the next define counted manually, keep these in sync with t_chunk_flags.
# define CRNT_CHUNK_FREE_POS 1
# define MAP_TYPE_ONE_POS 2
# define MAP_TYPE_TWO_POS 3
# define NUMBERS_OF_FLAGS 3 		// dont count END_FLAGS IT NOT A FLAG
# define FLAGS_MASK_MAP_TYPE 6		// take the bits representation of MAP_TYPE_ONE and MAP_TYPE_TWO

# define CHECKSUM(ptr, NB_FLAGS) (((size_t)ptr >> NB_FLAGS) << NB_FLAGS)
# define MAPTYPE(map_type) ((map_type << (MAP_TYPE_ONE_POS - 1)) & FLAGS_MASK_MAP_TYPE)
# define FREE(state) (state & CRNT_CHUNK_FREE)

/**
 * @brief this enum contains flags chunk.
 * each value must contain where is the bit! so 2^n, store in the enum 'n'.
 * END_FLAGS must be the last 'flags' in the enum, it used for algo, do not use it like a traditional flag.
 * 
 */
typedef enum e_chunk_flags {
	CRNT_CHUNK_FREE = 1,
	MAP_TYPE_ONE = 2,
	MAP_TYPE_TWO = 4,
	END_FLAGS = 8
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
get_chunk_free_flag(t_chunk* current_chunk) {
	return (current_chunk->flags & CRNT_CHUNK_FREE);
}

static inline void
set_chunk_free_flag(t_chunk* current_chunk, bool const state) {
	current_chunk->flags = (state) ? current_chunk->flags | CRNT_CHUNK_FREE : current_chunk->flags & ~CRNT_CHUNK_FREE;
}

static inline uint8_t
get_map_type_flag(t_chunk* chunk) {
	return ((chunk->flags & FLAGS_MASK_MAP_TYPE) >> (MAP_TYPE_ONE_POS - 1));
}

static inline void
set_map_type_flag(t_chunk* chunk, uint8_t map_type) {
	chunk->flags = chunk->flags & ~(size_t)FLAGS_MASK_MAP_TYPE;
	chunk->flags |= (map_type & FLAGS_MASK_MAP_TYPE);
}

static inline bool
check_cheksum(void const* ptr) {
	t_chunk const*	current_chunk;

	current_chunk = ptr_sub(ptr, sizeof(t_chunk));
	return (CHECKSUM(current_chunk->flags, NUMBERS_OF_FLAGS) == CHECKSUM(current_chunk, NUMBERS_OF_FLAGS));
}

static inline void
panic(int fd, char* str) {
	write(fd, str, strlen(str));
# ifdef DEBUG
	kill(0, SIGABRT);
# endif
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
t_page*	get_page_from_chunk(t_chunk* chunk, uint8_t map_type);
void*	alloc_large_page(size_t size);
void	free_all_page(void);

extern t_malloc g_malloc;

#endif
