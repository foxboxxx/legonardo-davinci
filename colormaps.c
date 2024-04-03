/* File: colormaps.c
 * =-=-=-=-=-=-=-=-=
 * @author Joe Robertazzi, Winter 2024 - <tazzi@stanford.edu>
 * Provides all possible colormaps that can be utlizied for printing images.
 */ 

// Project Imports
#include "colormaps.h"

// Constant number of printers available
const unsigned int NUM_PRINTERS = 4;

// Default blue printer -> utilizes 28 Color LEGO Palette obtained from Amazon
const unsigned int S_DEFAULT_BLUE = 28;
struct cartridge C_DEFAULT_BLUE[] = {
    {0xFFFFFFFF, 100}, {0xFFF4F4EA, 100}, {0xFFDBD7D5, 100}, {0xFFA1A5A4, 100}, {0xFFB3B7C4, 100}, {0xFF829BAE, 100}, {0xFF59D9EB, 100},
    {0xFFD4F5BC, 100}, {0xFFA5D654, 100}, {0xFFB1DAF0, 100}, {0xFF65D6F0, 100}, {0xFF7FBFE3, 100}, {0xFF83ABA5, 100}, {0xFF4DD288, 100},
    {0xFF2AAADF, 100}, {0xFF81AFDA, 100}, {0xFF5BBFBF, 100}, {0xFF2471BD, 100}, {0xFF319C72, 100}, {0xFF959E72, 100}, {0xFFAB9E7A, 100},
    {0xFF112C27, 100}, {0xFF142D49, 100}, {0xFF2D322A, 100}, {0xFF393A3B, 100}, {0xFF5E5F63, 100}, {0xFF1B1D20, 100}, {0xFF2F3134, 100},
};

// Default rainbow printer -> Test printer that contains all colors of rainbow
const unsigned int S_RAINBOW = 9;
struct cartridge C_RAINBOW[] = {
    {0xFFFF0000, 100}, {0xFFFFA500, 100}, {0xFFFFFF00, 100}, 
    {0xFF00FF00, 100}, {0xFF0000FF, 100}, {0xFF4B0082, 100}, 
    {0xFFEE82EE, 100}, {0xFFFFFFFF, 100}, {0xFF000000, 100},
};

// Default empty printer which retains the image information (when converted to NONE, image won't change colormaps)
const unsigned int S_NONE = 0;
struct cartridge C_NONE[] = {};

// Modified blue printer -> utilizes 10 / 28 Color LEGO Palette obtained from Amazon (due to time constraints)
const unsigned int S_MOD_BLUE = 9;
struct cartridge C_MOD_BLUE[] = {
    {0xFFF4F4EA, 100}, {0xFFDBD7D5, 100}, {0xFF4DD288, 100},
    {0xFF319C72, 100}, {0xFFAB9E7A, 100}, {0xFF112C27, 100}, 
    {0xFF2D322A, 100}, {0xFF393A3B, 100}, {0xFF1B1D20, 100}, 
};

// `P_DEFAULT_BLUE` initialization
struct printer P_DEFAULT_BLUE = {
    .name_printer = "DEF_B",
    .num_cartridges = S_DEFAULT_BLUE,
    .list_cartridges = C_DEFAULT_BLUE,
};

// `P_RAINBOW` initialization
struct printer P_RAINBOW = {
    .name_printer = "RGB",
    .num_cartridges = S_RAINBOW,
    .list_cartridges = C_RAINBOW,
};

// `P_NONE` initialization
struct printer P_NONE = {
    .name_printer = "NONE",
    .num_cartridges = S_NONE,
    .list_cartridges = C_NONE,
};

// `P_MOD_BLUE` initialization
struct printer P_MOD_BLUE = {
    .name_printer = "MOD_B",
    .num_cartridges = S_MOD_BLUE,
    .list_cartridges = C_MOD_BLUE,
};

// List of all printers
const struct printer* PRINTER_LIST[] = {
    &P_NONE, &P_RAINBOW, &P_DEFAULT_BLUE, &P_MOD_BLUE,
};
