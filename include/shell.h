#ifndef MYSHELL_SHELL_H
#define MYSHELL_SHELL_H

#include <stddef.h>

typedef struct {
    int last_status;    /* exit status of the last command, like $? */
} shell_state;

/* Allocation wrappers: print an error and exit instead of returning NULL. */
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(const char *s);

#endif
