#ifndef BITMAPS_H
#define BITMAPS_H

#include <stdbool.h>
#include <stddef.h>

struct img {
	char* name;
	unsigned int width, height;
	unsigned int pixels[];
};

extern const struct img skull_emoji;
extern const struct img mona_lisa;
extern const struct img mango;
extern const struct img fire_hydrant;
extern const struct img stanford;
extern const struct img raspberry;
extern const struct img yoda;
extern const struct img obama;
extern const struct img smile;
extern const struct img tractor;
extern const struct img yoshi;
extern const struct img apple;
extern const struct img cal;
extern const struct img instagram;
extern const struct img totoro;
extern const struct img bulbasaur;
extern const struct img message;
extern const struct img wave;
#endif