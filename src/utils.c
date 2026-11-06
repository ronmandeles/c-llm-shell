#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

/* Print "myshell: <what>: <strerror(errno)>" and exit.
   Uses exit(), so never call it in a child after fork(). */
static void exit_alloc_failed(const char *what)
{
    fprintf(stderr, "myshell: %s: %s\n", what, strerror(errno));
    exit(1);
}

/* Allocate `size` bytes or exit. A size of 0 is bumped to 1, because
   malloc(0) may legally return NULL and we treat NULL as failure. */
void *xmalloc(size_t size)
{
    void *p = malloc(size ? size : 1);
    if (p == NULL)
        exit_alloc_failed("malloc");
    return p;
}

/* Resize `ptr` to `size` bytes or exit. Size 0 is bumped to 1, as in xmalloc. */
void *xrealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size ? size : 1);
    if (p == NULL)
        exit_alloc_failed("realloc");
    return p;
}

/* Copy `s`, including its '\0', into new heap memory. Caller frees. */
char *xstrdup(const char *s)
{
    size_t len = strlen(s) + 1;     /* + 1 for the '\0' */
    char *copy = xmalloc(len);
    memcpy(copy, s, len);
    return copy;
}
