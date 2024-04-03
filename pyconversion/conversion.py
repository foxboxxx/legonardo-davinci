# File: conversion.py
# =-=-=-=-=-=-=-=-=-=
# @author Joe Robertazzi, Winter 2024 - <tazzi@stanford.edu>
# Converts user selelected photos to bitmaps, which are then stored 
# in the C file directory for bitmaps.c to be used for printing.

# Libraries
import imageio as iio

# Constants
A_COLOR = 0xff000000
R_SHIFT = 16
G_SHIFT = 8
B_SHIFT = 0
OPEN_CURLY = '{'
CLOSED_CURLY = '}'
BITMAPS_H = "/Users/joe/cs107e_home/project/bitmaps.h"
BITMAPS_C = "/Users/joe/cs107e_home/project/bitmaps.c"
IMG_PROCESS_C = "/Users/joe/cs107e_home/project/img_process.c"
SUCCESS = 1
FAIL = 0

# Function: calc_pixel()
# =-=-=-=-=-=-=-=-=-=-=-
# Calculates the hex value given a list of RGB values, and returns said value
def calc_pixel(pixel):
    return hex(A_COLOR + (pixel[0] << R_SHIFT) + (pixel[1] << G_SHIFT) + (pixel[2] << B_SHIFT))

# Function: create_struct()
# =-=-=-=-=-=-=-=-=-=-=-=-=
# Writes the struct in `bitmaps.c` of the input image, including data such
# as the image's width, height, and name.
def create_struct(image, image_name, image_width, image_height):
    # Append basic struct information to `bitmaps.c`
    bitmaps_c = open(BITMAPS_C, "a")
    bitmaps_c.write(f'const struct img {image_name} = {OPEN_CURLY}\n') 
    bitmaps_c.write(f'\t.name = (char *)"{image_name}",\n') 
    bitmaps_c.write(f'\t.width = {image_width},\n')
    bitmaps_c.write(f'\t.height = {image_height},\n')
    bitmaps_c.write(f'\t.pixels = {OPEN_CURLY}')

    # Append pixels array containing pixels of input `image` (within struct)
    for i in range(image_height):
        bitmaps_c.write(f'\n\t\t')
        for j in range(image_width):
            bitmaps_c.write(f'{calc_pixel(image[i][j])}, ')

    # Close the struct, successfully adding to file
    bitmaps_c.write(f'\n\t{CLOSED_CURLY}')
    bitmaps_c.write(f'\n{CLOSED_CURLY};\n\n')
    bitmaps_c.close()

# Function: create_struct_header()
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Writes the struct header for a new image to the `bitmaps.c` file.
def create_struct_header(image_name):
    # Read all lines
    bitmaps_h = open(BITMAPS_H, "r")
    lines = bitmaps_h.readlines()

    # Remove last line
    lines = lines[:-1]
    bitmaps_h.close()

    # Rewrite all lines except last, and add new struct to file
    bitmaps_h = open(BITMAPS_H, "w")
    for line in lines:
        bitmaps_h.write(line)
    bitmaps_h.write(f'extern const struct img {image_name};\n')
    bitmaps_h.write("#endif")

# Function: add_to_list()
# =-=-=-=-=-=-=-=-=-=-=-=
# Adds image struct to an array of other loaded images in `img_process.c`.
def add_to_list(image_name):
    # Read all previous lines in file
    img_process_c = open(IMG_PROCESS_C, "r")
    lines = img_process_c.readlines()
    lines = lines[:-1]
    img_process_c.close()

    # Write all lines except last line
    img_process_c = open(IMG_PROCESS_C, "w")
    for line in lines:
        if "const unsigned int BITMAP_LIST_SIZE =" in line:
            line = list(line.split(" "))
            line = line[len(line) - 1]
            line = line[:(len(line) - 2)]
            insert = str(int(line) + 1)
            img_process_c.write(f'const unsigned int BITMAP_LIST_SIZE = {insert};\n')
            continue
        img_process_c.write(line)
    
    # Add the newly created image struct to array in `img_process.c`, and close array
    img_process_c.write(f'\t&{image_name},\n')
    img_process_c.write("};")

# Function: create_bitmap()
# =-=-=-=-=-=-=-=-=-=-=-=-=
# Loads file into `bitmaps.c`, using helper functions `create_struct()`, 
# `create_struct_header()`, and `add_to_list()`.
def create_bitmap(filepath, name):
    # Check if file exists
    try:
        file = open(filepath)
    except IOError:
        print("Failed to read file.")
        return FAIL
    file.close()

    # Check that file format is supported
    if filepath[-4:] != ".png" and filepath[-5:] != ".jpeg" and filepath[-4:] != ".jpg":
        print("Invalid file type.")
        return
    
    # Convert file to pixels, and write pixels into C library
    image = iio.imread(filepath)
    create_struct(image, name, image.shape[1], image.shape[0])
    create_struct_header(name)
    add_to_list(name)
    return SUCCESS

# Function: reset_bitmap()
# Clears the `bitmaps.c`, `bitmaps.h`, and `img_process.c` files and all other traces of previously loaded
# images. Resets to blank slate.
def reset_bitmap():
    # Reset `bitmaps.h`
    bitmaps_h = open(BITMAPS_H, "w")
    bitmaps_h.write("#ifndef BITMAPS_H\n")
    bitmaps_h.write("#define BITMAPS_H\n\n")
    bitmaps_h.write("#include <stdbool.h>\n")
    bitmaps_h.write("#include <stddef.h>\n\n")
    bitmaps_h.write("struct img {\n")
    bitmaps_h.write("\tchar* name;\n")
    bitmaps_h.write("\tunsigned int width, height;\n")
    bitmaps_h.write("\tunsigned int pixels[];\n")
    bitmaps_h.write("};\n\n")
    bitmaps_h.write("#endif")
    bitmaps_h.close()

    # Reset `bitmaps.c`
    bitmaps_c = open(BITMAPS_C, "w")
    bitmaps_c.write('#include "bitmaps.h"\n\n')
    bitmaps_c.close()

    # Reset `img_process.c`
    img_process_c = open(IMG_PROCESS_C, "r")
    lines = img_process_c.readlines()
    img_process_c.close()
    img_process_c = open(IMG_PROCESS_C, "w")
    for line in lines:
        if "const unsigned int BITMAP_LIST_SIZE =" in line:
            img_process_c.write(f'const unsigned int BITMAP_LIST_SIZE = 0;')
            continue
        img_process_c.write(line)
        if "const struct img* BITMAP_LIST[] = {" in line:
            break
    img_process_c.write("};")

# Main Program
print("Welcome to the LEGO Printer image initializer please use the `help` command to begin! (`q` to quit)")
while(1):
    # Input
    inp = input("> ")
    inp = inp.split()

    # Empty input, ignore
    if not len(inp):
        continue

    # Execute help command
    elif inp[0] == 'help':
        print("\tload [filepath] [filename] --> Loads file to bitmap library")
        print("\tclear                     --> Clears bitmap library")
        print("\thelp                      --> Prints all commands")
        print("\tq                         --> Terminates program")
        continue

    # Execute quit command
    elif inp[0] == 'q':
        print("Successfully quit!")
        break

    # Execute clear command
    elif inp[0] == "clear":
        reset_bitmap()
        print("Successfully cleared bitmap library!")

    # Execute load command
    elif inp[0] == "load":
        if len(inp) < 3:
            print("Invalid syntax. Format: `load [filepath] [filename]`")
        else:
            if create_bitmap(inp[1], inp[2]):
                print(f'Sucessfully added "{inp[2]}" to bitmap library!')
            continue

    # Invalid command - throw erorr
    else:
        print("Invalid command. Please use load`, `clear`, `help`, or `q`.")