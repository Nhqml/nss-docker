#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>
#include <curl/curl.h>

#include "docker_api.h"
#include "utils.h"

size_t jsonify(void *data, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;

    struct cJSON *json = cJSON_Parse(data);
    *(struct cJSON **)userdata = json;

    return realsize;
}

char *_inspect_container_url(const char *name)
{
    char *url = xmalloc(_DOCKER_API_URL_BASE_SIZE + strlen(name) + 1);
    sprintf(url, DOCKER_API_ENDPOINT DOCKER_API_VERSION DOCKER_API_URL_P1 "%s" DOCKER_API_URL_P2, name);

    return url;
}

struct container *container_new(const char *name)
{
    struct container *container = xmalloc(sizeof(struct container));

    size_t name_len = strlen(name);

    container->name = xmalloc(name_len + 1);
    strcpy(container->name, name);
    container->short_uuid = xmalloc(DOCKER_SHORT_UUID_SIZE + 1);

    container->addr_list = xcalloc(1, sizeof(char *));
    container->n_addr = 0;

    return container;
}

struct container *container_set_short_uuid(struct container *container, const char *uuid)
{
    strncpy(container->short_uuid, uuid, DOCKER_SHORT_UUID_SIZE);
    container->short_uuid[DOCKER_SHORT_UUID_SIZE] = '\0';

    return container;
}

struct container *container_add_addr(struct container *container, char *addr)
{
    if (addr != NULL)
    {
        ++container->n_addr;
        container->addr_list = xreallocarray(container->addr_list, container->n_addr + 1, sizeof(char *));
        container->addr_list[container->n_addr - 1] = addr;
        container->addr_list[container->n_addr] = NULL;
    }

    return container;
}

void container_free(struct container *container)
{
    if (container != NULL)
    {
        free(container->name);
        free(container->short_uuid);

        for (char **addr = container->addr_list; *addr != NULL; ++addr)
            free(*addr);
        free(container->addr_list);

        free(container);
    }
}

struct container *inspect_container(const char *name, int af)
{
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        ERROR("error: failed when trying to retrieve containers: unable to initialize curl");
        return NULL;
    }

    struct curl_slist *headers = curl_slist_append(NULL, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, DOCKER_API_SOCK);

    char *docker_api_url = _inspect_container_url(name);
    curl_easy_setopt(curl, CURLOPT_URL, docker_api_url);

    struct cJSON *json;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, jsonify);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&json);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        ERROR("failed when trying to retrieve containers: %s", curl_easy_strerror(res));
        return NULL;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    free(docker_api_url);
    curl_easy_cleanup(curl);

    if (http_code != 200)
        return NULL;

    struct container *container = NULL;

    struct cJSON *container_name = cJSON_GetObjectItemCaseSensitive(json, "Name");
    if (!cJSON_IsString(container_name))
        return NULL;

    container = container_new(container_name->valuestring + 1); // +1 to skip leading '/' in the name

    struct cJSON *container_uuid = cJSON_GetObjectItemCaseSensitive(json, "Id");
    if (cJSON_IsString(container_uuid))
        container_set_short_uuid(container, container_uuid->valuestring);

    struct cJSON *container_netsettings = cJSON_GetObjectItemCaseSensitive(json, "NetworkSettings");
    if (container_netsettings != NULL)
    {
        struct cJSON *container_nets = cJSON_GetObjectItemCaseSensitive(container_netsettings, "Networks");

        for (int i = 0; i < cJSON_GetArraySize(container_nets); ++i)
        {
            struct cJSON *container_net = cJSON_GetArrayItem(container_nets, i);
            if (container_net != NULL)
            {
                char *container_addr = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(container_net, "IPAddress"));

                if (container_addr != NULL && container_addr[0] != '\0')
                    container_add_addr(container, parse_addr(container_addr, af));
            }
        }
    }

    cJSON_free(json);

    if (container->n_addr == 0)
    {
        container_free(container);
        container = NULL;
    }

    return container;
}
