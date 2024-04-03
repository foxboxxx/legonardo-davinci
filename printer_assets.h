#ifndef PRINTER_ASSETS_H
#define PRINTER_ASSETS_H

/*
 * Functions and assets needed for `printer.c`. Includes things such as
 * an int to string functiong alongside the bitmap which contains the title
 * splash screen for the printer application.
 *
 * @author Joe Robertazzi - Winter 2024 - <tazzi@stanford.edu>
 */

// Standard Library Imports
#include <stdbool.h>
#include <stddef.h>

// Project Imports
#include "bitmaps.h"

/*
 * `num_to_string`
 *
 * Converts an input numerical value (of type unsigned long) to a string.
 * 
 * @param buf          the char* buffer where the final string is stored
 * @param bufsize      the size of `buf`
 * @param val          the inputted value to be converted to a string
 * @param base         the base which the string is to be converted to 
 * @param min_width    the minimum width of the 
 */
int num_to_string(char *buf, size_t bufsize, unsigned long val, int base, size_t min_width);

struct scale_mode {
    char* name;
    unsigned int width, height;
};

extern struct scale_mode PREV;
extern struct scale_mode TWNTY;
extern struct scale_mode FRTY_TL;
extern struct scale_mode FRTY_TR;
extern struct scale_mode FRTY_BR;
extern struct scale_mode FRTY_BL;
extern const struct scale_mode* SCALE_MODE_LIST[];
extern const unsigned int NUM_SCALES;
extern const struct img title;

#endif