#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <stddef.h>

typedef struct s_page {
	void	*chunk;
	void	*next;
}	t_page;

typedef struct s_chunk {
	size_t	size;
	void	*next;
}	t_chunk;

typedef struct s_malloc {
	t_page	*tiny;
	t_page	*small;
	t_page	*large;
}	t_malloc;

typedef enum e_memory_zone {
	TINY,
	SMALL,
	LARGE,
} t_memory_zone;

#endif
