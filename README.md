# ft_malloc

> A custom dynamic memory allocator written in C, implementing the core behavior of `malloc()`, `free()` and `realloc()` using `mmap()` and custom memory management.

---

## Overview

`ft_malloc` is a custom memory allocator developed as part of the 42 curriculum.

The objective of this project is to understand how dynamic memory allocation works internally by designing an allocator capable of managing its own memory pages, metadata and allocation strategy without relying on the standard C library implementation.

The allocator is built directly on top of Linux virtual memory using `mmap()` and `munmap()`.

---

## Features

* Custom implementation of `malloc()`, `free()` and `realloc()`
* Three allocation zones:

  * **TINY** (≤ 256 B)
  * **SMALL** (≤ 1024 B)
  * **LARGE** (> 1024 B)
* Memory allocation through `mmap()`
* First Fit allocation strategy
* Block splitting
* Automatic block coalescing
* Reuse of freed memory
* Alignment on `max_align_t`
* Invalid pointer detection
* Double free detection
* Metadata integrity checking
* Thread synchronization using a global mutex
* Debug integrity checks

---

# Memory Architecture

The allocator separates memory into three independent zones.

```text
                 malloc(size)
                       │
                       ▼
              Determine allocation size
                       │
                       ▼
          +------------+------------+
          |            |            |
        TINY         SMALL        LARGE
          |            |            |
     Shared Pages  Shared Pages  Dedicated mmap
```

---

# Memory Layout

Each page begins with a page header followed by one or more chunks.

```text
+-----------------------------------------------------------+
| t_page                                                    |
+-----------------------------------------------------------+
| t_chunk | User Data | t_chunk | User Data | ...           |
+-----------------------------------------------------------+
```

Each chunk stores only two fields:

```c
typedef struct s_chunk
{
    size_t size;
    size_t flags;
} t_chunk;
```

The metadata remains intentionally compact while storing:

* allocation size
* allocation state
* memory zone
* integrity checksum

---

# Allocation Flow

```text
malloc(size)
      │
      ▼
Align requested size
      │
      ▼
Select memory zone
      │
      ▼
Search first available chunk
      │
      ▼
Split if necessary
      │
      ▼
Return user pointer
```

### Allocation strategy

The allocator uses a **First Fit** strategy.

The first free chunk large enough to satisfy the request is reused.

---

# Block Splitting

When a free block is larger than requested, it is divided into two chunks.

Before:

```text
+-------------------------------+
|          FREE (512 B)         |
+-------------------------------+
```

After `malloc(128)`:

```text
+---------------+---------------+
| USED (128 B)  | FREE          |
+---------------+---------------+
```

This reduces internal fragmentation while allowing the remaining memory to be reused later.

---

# Free Flow

```text
free(ptr)
     │
     ▼
Validate pointer
     │
     ▼
Mark chunk as FREE
     │
     ▼
Merge adjacent free blocks
     │
     ▼
Release unused pages if possible
```

Adjacent free chunks are automatically merged to reduce fragmentation.

---

# Reallocation

`realloc()` attempts to resize memory while minimizing copies.

It follows three possible paths:

### Shrinking

```text
USED 512 B
      │
      ▼
USED 128 B + FREE
```

### In-place expansion

If the following chunk is free and large enough, the block is extended without moving the allocation.

### Relocation

If expansion is impossible:

```text
malloc()
     │
     ▼
memcpy()
     │
     ▼
free(old)
```

---

# Memory Zones

| Zone      | Allocation Size | Strategy           |
| --------- | --------------: | ------------------ |
| **TINY**  |         ≤ 256 B | Shared pages       |
| **SMALL** |        ≤ 1024 B | Shared pages       |
| **LARGE** |        > 1024 B | Dedicated `mmap()` |

Large allocations always receive their own memory mapping.

---

# Integrity Checks

Each chunk stores lightweight metadata allowing the allocator to detect:

* invalid pointers
* corrupted headers
* double frees

When compiled in **DEBUG** mode, every page can additionally be verified to detect metadata corruption during development.

---

# Thread Synchronization

The allocator protects its internal structures with a global `pthread_mutex_t`.

This guarantees that concurrent calls to:

* `malloc()`
* `free()`
* `realloc()`
* `show_alloc_mem()`

cannot corrupt the allocator's internal state.

---

# Build

```bash
make
```

Debug build:

```bash
make debug
```

Clean:

```bash
make clean
```

Rebuild:

```bash
make re
```

---

# Usage

```c
#include "ft_malloc.h"

int main(void)
{
    char *buffer = malloc(64);

    buffer[0] = 'A';

    show_alloc_mem();

    free(buffer);

    return (0);
}
```

---

# Tested

The allocator has been tested with:

* thousands of consecutive allocations
* repeated allocation/free cycles
* large memory allocations
* `realloc()`
* alignment verification
* invalid pointer detection
* double free detection

---

# Current Limitations

Like most educational allocators, this implementation favors simplicity and readability over raw performance.

Current limitations include:

* First Fit linear search
* global mutex
* Linux only
* no allocation benchmarks yet
* no advanced free lists or arenas

---

# Future Improvements

Possible future enhancements include:

* allocation benchmarks
* faster free block lookup
* improved multi-thread scalability
* allocation statistics
* additional debugging utilities

---

# What I Learned

This project significantly improved my understanding of:

* low-level C programming
* virtual memory management
* Linux system calls
* allocator design
* memory fragmentation
* metadata management
* synchronization
* systems programming

---

## Conclusion

`ft_malloc` is a complete educational implementation of a dynamic memory allocator built directly on top of Linux virtual memory.

Beyond reproducing the behavior of `malloc()`, `free()` and `realloc()`, the project explores the design decisions behind modern allocators, including page management, block splitting, coalescing, metadata organization and memory reuse.

---

## AI Assistance

Parts of this README were refined with the help of AI tools to improve clarity, structure, and overall presentation.

All technical content, design decisions, and implementation details are original and reflect the author's work.