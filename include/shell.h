#ifndef MYSHELL_SHELL_H
#define MYSHELL_SHELL_H

#include <stddef.h>

typedef struct {
    int last_status;    /* exit status of the last command, like $? */
} shell_state;

/*
 * Allocate `size` bytes on the heap, like malloc(3).
 * Never returns NULL: on failure it prints an error to stderr and exits.
 * A size of 0 still returns a unique pointer that must be passed to free().
 */
void *xmalloc(size_t size);

/*
 * Resize the block at `ptr` to `size` bytes, like realloc(3).
 * Never returns NULL: on failure it prints an error to stderr and exits.
 * The old pointer is invalid after the call; use the returned one.
 */
void *xrealloc(void *ptr, size_t size);

/*
 * Return a heap-allocated copy of the string `s`, like strdup(3).
 * Never returns NULL. The caller owns the copy and must free() it.
 */
char *xstrdup(const char *s);

#endif
