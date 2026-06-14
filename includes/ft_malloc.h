#ifndef FT_MALLOC_H
# define FT_MALLOC_H

#include <stddef.h>


// https://sourceware.org/glibc/wiki/MallocInternals

/*
PAGE
*--------**--------*
|H       ||H       |
|e       ||e       |
|C       ||C       |
|i       ||        |
|e       ||        |
|C       ||        |
|e       ||e       |
|        ||        |
|C       ||        |
|i       ||        |
|e       ||        |
|        ||        |
|        ||        |
*--------**--------*
*/

/*
pour ragarder ou peux etre inserer un chunk il suffit de:
	prend le fin du chunk actuelle et faire la diff avec le chunk->next
	get_size_beetwin_chunk(void *end_addr_start_chunk, void *start_addr_end_chunk)
	ici dans cette fonctio is il ny a pas de start chunk ou de end chunk  on prendra start_page et end_page.

	pour completer ce process il faut regarder si il est intersent de stocker dans le header_page le nombre despace encore libre dans la page
	sachant que cette espace peux etre fragmenter et donc donne une indication contre intuitif. 
*/

#define N 256
#define M 1024

typedef struct s_page {
	void	*chunk;
	void	*next;
}	t_page;

typedef struct s_chunk {
	size_t	size;
	void	*next;
}	t_chunk;

/* tiny et small on le meme foncitonement un header page et des chunk, pour large une addrs une data!*/
typedef struct s_malloc {
	t_page	*tiny;
	t_page	*small;
	t_page	*large;
}	t_malloc;

typedef enum e_memory_zone {
	tiny,
	small,
	large,
} t_memory_zone;

/*
hummm non je la trouve tres bien !
elle est assez legerte je trouve !!! honnetement je suis compthend de cette struct !
et toi tu en penses quoi ?
perso il y a encore un truc ou je me tate, mais je viend davoir une idee !
jai aussi cette struct que tu na pas vue, "typedef struct s_malloc {
    t_page  *tiny;
    t_page  *small;
    t_page  *large;
}   t_malloc;"
elle est declarer en dehord des fonction elle est donc global, (on a le droit dans le sujet(a une)) du coup  
*/

#endif