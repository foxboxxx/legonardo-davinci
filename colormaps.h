#ifndef COLORMAPS_H
#define COLORMAPS_H

/*
 * Provides structs and presets for color maps associated with
 * LEGO pieces. Structs include `cartidiges` which contain color 
 * information and number of LEGO pieces within said cartridge, and
 * `printers` which contain sets of cartidges creating a colormap.
 *
 * @author Joe Robertazzi, Winter 2024 - <tazzi@stanford.edu>
 */

// Standard Library Imports
#include <stddef.h>

// Contains a `color` and `capacity`
struct cartridge {
	unsigned int color, capacity;
};

// Contains a `num_cartidges`, `list_cartridges`, and `name_printer`
struct printer {
    unsigned int num_cartridges;
    struct cartridge* list_cartridges;
    const char* name_printer;
};

// List of printers, an array of all printers, and the total number of printers available
extern struct printer P_MOD_BLUE;
extern struct printer P_DEFAULT_BLUE;
extern struct printer P_RAINBOW;
extern struct printer P_NONE;
extern const struct printer* PRINTER_LIST[];
extern const unsigned int NUM_PRINTERS;

#endif