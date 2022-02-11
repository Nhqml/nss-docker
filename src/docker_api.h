#pragma once

#define DOCKER_API_SOCK "/var/run/docker.sock"
#define DOCKER_API_ENDPOINT "http://localhost/"
#define DOCKER_API_VERSION "v1.41"

#define DOCKER_API_URL_P1 "/containers/"
#define DOCKER_API_URL_P2 "/json"

#define _DOCKER_API_URL_BASE_SIZE 39

#define DOCKER_UUID_SIZE 64
#define DOCKER_SHORT_UUID_SIZE 12

#define DOCKER_LOCAL_DOMAIN ".docker.local"

struct container
{
    char *name;
    char *short_uuid;
    size_t n_addr;
    char **addr_list;
};

struct container *inspect_container(const char *name, int af);
