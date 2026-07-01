#ifndef FT_MALLOC_INTERN
# define FT_MALLOC_INTERN

# include <stddef.h>

# define INIT_ZONE_NB_PAGE_DEFAULT 2
# define N 256
# define M 1024

# define MALLOC_PROT_MMAP PROT_READ | PROT_WRITE
# define MALLOC_FLAG_MMAP MAP_ANONYMOUS | MAP_PRIVATE

typedef unsigned char uchar;

static inline void*
ptr_add(void *p, size_t n) {
	return (uchar*)p + n;
}

/**
 * @brief Returns the number of bytes available for allocation between two t_chunk
 * 
 * @param start_addr Address of the first chunk
 * @param end_addr Address of the second chunk
 * @return size_t Number of bytes available between the two chunks
 */
size_t	get_free_space_between(void *current_page, void *start_addr, void *end_addr);

/**
 * @brief Initializes a tiny or small zone and adds the first chunk
 * 
 * @param zone_type The type of zone to initialize
 * @param size Size of the first chunk
 * @return void* On success, returns a pointer to the first usable address after the page header and chunk header. On failure, returns MAP_FAILED
 */
void*	init_zone(int zone_type, size_t size);

/**
 * @brief this fonction insert a chunk at page + sizeof(t_page)
 * 
 * @param current_page witch page to insert chunk
 * @param size Size of the first data
 * @return void* Returns a pointer to the first usable address after the page header and chunk header
 */
void*
insert_new_chunk_after_page_header(void* current_page, size_t size);

#endif
