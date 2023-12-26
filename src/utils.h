#pragma once

#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

#define ERROR(...)                           \
    do                                       \
    {                                        \
        char buf[1024];                      \
        strerror_r(errno, buf, sizeof(buf)); \
        fprintf(stderr, "error: ");          \
        fprintf(stderr, __VA_ARGS__);        \
        fprintf(stderr, ": %s\n", buf);      \
    } while (0)

/**
** \brief Safe malloc wrapper
*/
static inline void *xmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
        error(1, errno, "failed when trying to allocate %lu bytes of memory", size);

    return ptr;
}

/**
** \brief Safe calloc wrapper
*/
static inline void *xcalloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL)
        error(1, errno, "failed when trying to allocate %lu bytes of memory", size * nmemb);

    return ptr;
}

/**
** \brief Safe reallocarray wrapper
*/
static inline void *xreallocarray(void *ptr, size_t nmemb, size_t size)
{
    void *new_ptr = reallocarray(ptr, nmemb, size);
    if (ptr == NULL)
        error(1, errno, "failed when trying to allocate %lu bytes of memory", size * nmemb);

    return new_ptr;
}

/**
** \brief Safe strdup wrapper
*/
static inline void *xstrdup(const char *str)
{
    void *dup_str = strdup(str);
    if (dup_str == NULL)
        error(1, errno, "failed to duplicate string");

    return dup_str;
}

/**
** \brief Parse string representation of Internet address into its bytes representation
**
** \param addr_as_str address as string
** \param af address type
** \return char* bytes representation of address
*/
char *parse_addr(const char *addr_as_str, int af);
