#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

static void die_oom(const char *what)
{
    fprintf(stderr, "myshell: %s: %s\n", what, strerror(errno));
    exit(1);
}

void *xmalloc(size_t size)
{
    /* malloc(0) may legally return NULL; ask for 1 byte so NULL always means failure. */
    void *p = malloc(size ? size : 1);
    if (p == NULL)
        die_oom("malloc");
    return p;
}

void *xrealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size ? size : 1);
    if (p == NULL)
        die_oom("realloc");
    return p;
}

char *xstrdup(const char *s)
{
    size_t len = strlen(s) + 1;     /* + 1 for the '\0' */
    char *copy = xmalloc(len);
    memcpy(copy, s, len);
    return copy;
}
