#ifndef MYSHELL_SHELL_H
#define MYSHELL_SHELL_H

#include <stddef.h>

typedef struct {
    int last_status;    /* exit status of the last command, like $? */
} shell_state;

/* malloc(3) that never returns NULL: exits on failure. */
void *xmalloc(size_t size);

/* realloc(3) that never returns NULL: exits on failure.
   `ptr` is invalid afterwards; use the returned pointer. */
void *xrealloc(void *ptr, size_t size);

/* strdup(3) that never returns NULL. The caller must free() the copy. */
char *xstrdup(const char *s);

#endif
