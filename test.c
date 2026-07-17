
#include "includes/ft_malloc.h"
#include <stddef.h>

void show_alloc_mem(void);

int
t0(void) {
	int		i;
    char*	addr;

	(void)addr;
    i = 0; 
    while (i < 1024) {
        i++;
    } 
    return (0); 
}

int
t1(void) {
	int i;
	char *addr;

	i = 0;
	while (i < 1024) {
		addr = (char*)malloc(1024);
		addr[0] = 42;
		i++;
	}
	return (0);
}

int
t2(void) {
	int i;
	char *addr;

	i = 0;
	while (i < 1024) {
		addr = (char*)malloc(1024);
		addr[0] = 42;
		free(addr);
		i++;
	}
	return (0);
}

#include <unistd.h>
#include <string.h>
#define M (1024*1024)

void print(char *s) {
	write(1, s, strlen(s));
}

int
t3(void) {
	char *addr1;
	char *addr2;
	char *addr3;

	(void)addr2;
	addr1 = (char*)malloc(16*M);
	strcpy(addr1, "Bonjours\n");
	print(addr1);
	addr2 = (char*)malloc(16*M);
	addr3 = (char*)realloc(addr1, 128*M);
	addr3[127*M] = 42;
	print(addr3);
	return (0);
}

int
t4(void) {
	char*	addr1;
	char*	addr3;

	addr1 = (char*)malloc(16*M);
	strcpy(addr1, "Bonjours\n");
	print(addr1);
	addr3 = (char*)realloc(addr1, 128*M);
	addr3[127*M] = 42;
	print(addr3);
	return (0);
}

int
t5(void) {
	char*	addr;

	addr = malloc(16);
	free(NULL);
	free((void *)((size_t)addr + 5));
	if (realloc((void *)((size_t)addr + 5), 10) == NULL)
		print("Bonjours\n");
	free(addr);
	return (0);
}

int
t6(void) {
	int		i;
	int*	addr[5001];

	for (i = 0; i < 5000; ++i) {
		addr[i] = malloc(sizeof(int));
		*addr[i] = i;
	}
	for (i = 0; i < 5000; ++i) {
		if (i != *addr[i])
			write(1, "ERROR\n", 6);
		free(addr[i]);
	}
	return (0);
}

#include <stdlib.h>

int
t7(void) {
	int		i;
	int		alignment;
	char*	addr;

	i = 1;
	alignment = _Alignof(max_align_t);
	while (i <= 100) {
		addr = (char*)malloc(i);
		if (addr == NULL) {
			print("Failed to allocate memory\n");
			exit(1);
		}
		if ((((unsigned long) (addr)) % alignment) != 0) {
			print("malloc returned a non aligned boundary\n");
			free(addr);
			return (1);
		}
		i++;
		free(addr);
	}
	return (0);
}

int
t8(void) {
	void* a = malloc(1);
    void* b = malloc(2);
    void* c = malloc(4);
    void* d = malloc(8);
    void* e = malloc(16);
    void* f = malloc(32);
    void* g = malloc(64);
    void* h = malloc(128);
    void* i = malloc(256);
    void* j = malloc(512);
    void* k = malloc(1024);
    void* l = malloc(1024 * 2);
    void* m = malloc(1024 * 4); 
    void* n = malloc(1024 * 32);
    void* o = malloc(M);
    void* p = malloc(16*M);
    void* q = malloc(128*M);
    show_alloc_mem();
    return (0); 
}

int
t9(void) {
	char*	addr1;
	char*	addr2;
	char*	addr3;

	addr1 = (char*)malloc(16*M);
	if (addr1 == NULL) {
		print("Failed to allocate memory\n");
		exit(1);
	}
	strcpy(addr1, "Hello world!\n");
	print(addr1);
	addr2 = (char*)malloc(16*M);
	if (addr2 == NULL) {
		print("Failed to allocate memory\n");
		exit(1);
	}
	addr3 = (char*)realloc(addr1, 128*M);
	if (addr3 == NULL) {
		print("Failed to reallocate memory\n");
		exit(1);
	}
	addr3[127*M] = 42;
	print(addr3);
	return (0);
}

int
main(void) {
	t1();
	t2();
	t3();
	t4();
	t5();
	t6();
	t7();
	show_alloc_mem();
	t8();
	t9();
	return (0);
}
