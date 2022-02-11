#pragma once

#define PUBLIC __attribute__((visibility("default")))

#define MAX_NR_ALIASES 48
#define MAX_NR_ADDRS 48

#include <nss.h>

enum nss_status
    PUBLIC
    _nss_docker_gethostbyname_r(const char *name,
                                struct hostent *result,
                                char *buf, size_t buflen,
                                int *errnop, int *h_errnop);

enum nss_status
    PUBLIC
    _nss_docker_gethostbyname2_r(const char *name, int af,
                                 struct hostent *result,
                                 char *buf, size_t buflen,
                                 int *errnop, int *h_errnop);

enum nss_status
    PUBLIC
    _nss_docker_gethostbyname3_r(const char *name, int af,
                                 struct hostent *result,
                                 char *buf, size_t buflen,
                                 int *errnop, int *h_errnop,
                                 int32_t *ttlp, char **canonp);
