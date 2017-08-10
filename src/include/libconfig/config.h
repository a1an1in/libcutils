#ifndef __CONFIG_H__
#define __CONFIG_H__

#define DEFAULT_CONFIG_BUF_LEN 1024
typedef struct configurator_s{
    allocator_t *allocator;
    void *buf;
    int buf_len;
}configurator_t;

#endif

