#include <arpa/inet.h>
#include <netdb.h>
#include <nss.h>
#include <string.h>

#include "docker_api.h"
#include "docker_nss.h"
#include "utils.h"

enum nss_status _nss_docker_gethostbyname_r(const char *name,
                                            struct hostent *result,
                                            char *buf, size_t buflen,
                                            int *errnop, int *h_errnop)
{
    return _nss_docker_gethostbyname3_r(name, AF_INET, result, buf, buflen, errnop, h_errnop, NULL, NULL);
}

enum nss_status _nss_docker_gethostbyname2_r(const char *name, int af,
                                             struct hostent *result,
                                             char *buf, size_t buflen,
                                             int *errnop, int *h_errnop)
{
    return _nss_docker_gethostbyname3_r(name, af, result, buf, buflen, errnop, h_errnop, NULL, NULL);
}

/**
** \brief Returns the name without docker local domain
*/
char *strip_docker_local_domain(const char *name)
{
    if (name == NULL)
        return NULL;

    char *stripped_name = xstrdup(name);

    for (char *c = stripped_name; *c != '\0'; ++c)
    {
        if (*c == '.')
        {
            if (strcmp(c, DOCKER_LOCAL_DOMAIN) == 0)
            {
                *c = '\0'; // Terminate the string here
                break;
            }
        }
    }

    return stripped_name;
}

enum nss_status _nss_docker_gethostbyname3_r(const char *name, int af,
                                             struct hostent *result,
                                             char *buf, size_t buflen,
                                             int *errnop, int *h_errnop,
                                             int32_t *ttlp, char **canonp)
{
    /* I don't know for sure what ttlp is, my guess is that it's a pointer to
    the TTL but since it's seems that no one uses it, neither am I... */
    if (ttlp)
        *ttlp = 0;

    if (af == AF_UNSPEC)
        af = AF_INET;

    (void)buf;
    (void)buflen;

    char *stripped_name = strip_docker_local_domain(name);
    struct container *container = inspect_container(stripped_name, af);
    free(stripped_name);
    if (container == NULL)
        return NSS_STATUS_NOTFOUND;

    char **r_aliases = xcalloc(2, sizeof(char *));
    r_aliases[0] = container->short_uuid;

    result->h_name = container->name;
    result->h_aliases = r_aliases;
    result->h_addrtype = af;
    result->h_length = 4;
    result->h_addr_list = container->addr_list;

    *errnop = 0;
    *h_errnop = 0;

    /* I don't know for sure what canonp is, my guess is that it's a pointer to
    a list of "canonical names" */
    if (canonp)
        *canonp = result->h_name;

    return NSS_STATUS_SUCCESS;
}
