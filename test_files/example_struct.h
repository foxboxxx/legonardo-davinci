#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stddef.h>

//typedef struct img img_t;

struct img {
    unsigned int width, height;
    unsigned int pixels[];
};

extern const struct img test;
#endif