#include <arpa/inet.h>
#include <sys/types.h>

#include "utils.h"

char *parse_addr(const char *addr_as_str, int af)
{
    char *addr_bytes;
    if (af == AF_INET)
    {
        struct in_addr addr;
        if (inet_pton(af, addr_as_str, &addr) != 1)
            return NULL;
        addr_bytes = xmalloc(4 * sizeof(char));
        memcpy(addr_bytes, &addr.s_addr, 4);
    }
    else if (af == AF_INET6)
    {
        struct in6_addr addr;
        if (inet_pton(af, addr_as_str, &addr) != 1)
            return NULL;
        addr_bytes = xmalloc(16 * sizeof(char));
        memcpy(addr_bytes, &addr.__in6_u, 16);
    }

    return addr_bytes;
}
