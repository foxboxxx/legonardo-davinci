/* File: img_process.c
 * =-=-=-=-=-=-=-=-=-=
 * @author Joe Robertazzi, Winter 2024 - <tazzi@stanford.edu>
 * Contains relevant functions to process images. Allows for
 * displaying images on display via bitmaps, alongside manipulating
 * said images via up/downscaling, cropping, and altering colormaps.
 */ 

// Standard Library Imports
#include <stdbool.h>

// Library Imports
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "strings.h"
#include "malloc.h"

// Project Imports
#include "img_process.h"
#include "bitmaps.h"
#include "colormaps.h"

// Constants
#define R_SHIFT 16
#define G_SHIFT 8
#define B_SHIFT 0
#define COLOR 0xff
#define NONE -1
#define BACKGROUND_REMOVAL true

/* Function: display_image()
 * =-=-=-=-=-=-=-=-=-=-=-=-=
 * Displays image on screen at {`x_start`, `y_start`}. Image displaying
 * duration depends on `seconds`, and displays indefinitely if `seconds`
 * is 0. If inputted width and height are -1, use inputted image's width
 * or height.
 */
void display_image(const struct img *input, unsigned int x_start, unsigned int y_start, unsigned int inp_width, unsigned int inp_height, bool swap, unsigned int seconds) {    
    // Initialize width and height used to print
    unsigned int WIDTH = input->width;
    unsigned int HEIGHT = input->width;
    if(inp_width != NONE) WIDTH = inp_width;
    if(inp_height != NONE) HEIGHT = inp_height;

    // If swapping buffer, initialize new gl
    if(swap) gl_init(HEIGHT, WIDTH, GL_DOUBLEBUFFER);

    // Draw each pixel individually
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            gl_draw_pixel(j + x_start, i + y_start, input->pixels[j + (i * WIDTH)]);
        }
    }
    
    // If swap true, swap buffer - if duration input, delay by duration
    if(swap) gl_swap_buffer();
    if(seconds) timer_delay(seconds);
}

/* Function: up_scale_image()
 * =-=-=-=-=-=-=-=-=-=-=-=-=-
 * Scales an input image up to a higher resolution (stretches pixels).
 * Each pixel becomes a giant pixel of `factor_x` width and `factor_y`
 * and the return image is a larger version of the original.
 */
struct img* up_scale_image(const struct img *input, unsigned int factor_x, unsigned int factor_y) {
    // Initialize result variables
    struct img* result = malloc(sizeof(struct img) + (input->width * input->height * factor_x * factor_y * sizeof(unsigned int)));
    result->name = input->name;
    result->width = input->width * factor_x;
    result->height = input->height * factor_y;
    
    // Convert pixels to larger pixels in upscaled image
    for(int i = 0; i < input->width * input->height; i++) {
        for(int y = 0; y < factor_y; y++) {
            for(int x = 0; x < factor_x; x++) {
                // 1D array calculation to add correct pixels to new bitmap array
                result->pixels[((i % input->width) * factor_x + x) + ((i / input->width) * factor_y + y) * result->width] = input->pixels[i];
            }
        }
    }

    return result;
}

/* Function: center_crop_image()
 * =-=-=-=-=-=-=-=-=-=-=-=-=
 * Crops `width_cut`/2 pixels from each horizontal side of input image, and `height_cut`/2
 * pixels from each vertical side of input image, returning resulting cropped image.
 */
struct img* center_crop_image(const struct img *input, unsigned int width_cut, unsigned int height_cut) {
    // Initialize result variables
    struct img* result = malloc(sizeof(struct img) + ((input->width - width_cut) * (input->height - height_cut) * sizeof(unsigned int)));
    result->name = input->name;
    result->width = input->width - width_cut;
    result->height = input->height - height_cut;

    // Set new array to pixels within center cropped region
    for(int i = 0; i < result->width * result->height; i++) {
        unsigned int target_pixel = input->pixels[(i % result->width + width_cut / 2) + ((i / result->width + height_cut / 2) * input->width)];
        result->pixels[i] = target_pixel;
    }

    return result;
}

/* Function: down_scale_image()
 * =-=-=-=-=-=-=-=-=-=-=-=-=
 * Downscales input image to resulting image with dimensions
 * spanning inputted `scaled_width` x `scaled_height`
 */
struct img* down_scale_image(const struct img *input, unsigned int scaled_width, unsigned int scaled_height) {
    // Return original image if attempted to scale down to higher resolution
    if(scaled_width >= input->width || scaled_height >= input->height) {
        return (struct img*)input;
    }

    // Initialize original width/heights
    int crop_width = input->width;
    int crop_height = input->height;

    // Find closest common multiple of `scaled_width` to `crop_width`
    for(int i = input->width; i > 0; i--) {
        if(i % scaled_width == 0) {
            crop_width = i;
            break;
        }
    }

    // Find closest common multiple of `scaled_height` to `crop_height`
    for(int i = input->height; i > 0; i--) {
        if(i % scaled_height == 0) {
            crop_height = i;
            break;
        }
    }

    // If image is too small for integer division errors to become negligible, center crop to common multiples found above
    if(input->width < 1000 || input->height < 1000) {
        input = center_crop_image(input, input->width - crop_width, input->height - crop_height);
    }

    // Initialize result variables
    struct img* result = malloc(sizeof(struct img) + (scaled_width * scaled_height * sizeof(unsigned int)));
    result->name = input->name;;
    result->width = scaled_width;
    result->height = scaled_height;

    // Initialize scale ratios
    unsigned int x_pixel_ratio = input->width / scaled_width;
    unsigned int y_pixel_ratio = input->height / scaled_height;

    // Add median pixel within each ratio of original image to new image
    for(int i = 0; i < scaled_width * scaled_height; i++) {
        // 1D array calculation to find median pixel
        unsigned int target_pixel = input->pixels[(i % scaled_width) * x_pixel_ratio + (x_pixel_ratio / 2) + ((input->width * (i / scaled_width) * y_pixel_ratio + (y_pixel_ratio / 2)))];
        result->pixels[i] = target_pixel;
    }
    
    return result;
}

/* Function: format_image()
 * =-=-=-=-=-=-=-=-=-=-=-=-=
 * Formats image to match inputted color map. Utilizes `palette_convert()`
 * to convert pixels to closest match from inputted color map.
 */
struct img* format_image(const struct img *input, struct printer color_map, bool printing_state) {
    // Initialize result variables
    unsigned int input_size = input->height * input->width;
    struct img* result = malloc(sizeof(struct img) + (input_size * sizeof(unsigned int)));
    result->name = input->name;
    result->height = input->height;
    result->width = input->width;

    // Declare copy of cartridge values if not in printing mode (doesn't remove pixels during print)
    unsigned int cap_copy[color_map.num_cartridges];
    
    // Copy cartridge values to copy delcared above
    for(int i = 0; i < color_map.num_cartridges; i++) {
        cap_copy[i] = color_map.list_cartridges[i].capacity;
    } 

    // Convert the pixels to new colors using `palette_convert()`
    for(int i = 0; i < input_size; i++) {

        // !!! DELETE IF BROKEN - BETA FEATURE
        if(BACKGROUND_REMOVAL) {
            if(are_neighbors_matching(input, input->pixels[i], GL_BLACK, i)) {
                result->pixels[i] = GL_BLACK;
                continue;
            }
        }
        // !!! DELETE IF BROKEN - BETA FEATURE

        unsigned int converted_pixel = palette_convert(input->pixels[i], color_map, printing_state);
        result->pixels[i] = converted_pixel;
    }

    // Return original color values to normal (so values only change when directly calling palette_convert)
    for(int i = 0; i < color_map.num_cartridges; i++) {
        color_map.list_cartridges[i].capacity = cap_copy[i];
    }

    return result;
}

/* Function: crop_image()
 * =-=-=-=-=-=-=-=-=-=-=-=-=
 * Crops input image starting at {`x_start`,`y_start`}, and 
 * has width `width` and height `height`.
 */
struct img* crop_image(const struct img *input, unsigned int x_start, unsigned int y_start, unsigned int width, unsigned int height) {
    // Initialize resulting variables
    unsigned int result_size = height * width;
    struct img* result = malloc(sizeof(struct img) + (result_size * sizeof(unsigned int)));
    result->name = input->name;
    result->height = height;
    result->width = width;

    // Sets pixels within cropped range to new image pixel array
    for(int i = 0; i < result_size; i++) {
        result->pixels[i] = input->pixels[(x_start + (i % width)) + ((y_start + i / width) * input->width)];
    }

    return result;
}

/* Function: palette_convert()
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=
 * Converts input pixel to closest color from input color map.
 */
unsigned int palette_convert(unsigned int pixel, struct printer color_map, bool printing_state) {



    // Initialize variables
    unsigned int min_distance = 3 * COLOR;
    unsigned int candidate = pixel;
    unsigned int temp_distance = 0;
    int candidate_index = 0;

    // Initialize color values
    unsigned int red = (pixel >> R_SHIFT) & COLOR;
    unsigned int green = (pixel >> G_SHIFT) & COLOR;
    unsigned int blue = (pixel >> B_SHIFT) & COLOR;

    // Cycle through possible colors in color map to find best matching color
    for(int i = 0; i < color_map.num_cartridges; i++) {
        // Initialize RGB values of current color
        unsigned int CMAP_blue = (color_map.list_cartridges[i].color >> B_SHIFT) & COLOR;
        unsigned int CMAP_green = (color_map.list_cartridges[i].color >> G_SHIFT) & COLOR;
        unsigned int CMAP_red = (color_map.list_cartridges[i].color >> R_SHIFT) & COLOR;

        // Find total distance of input pixel to current color
        if(red > CMAP_red) temp_distance += red - CMAP_red;
        else temp_distance += CMAP_red - red;
        if(green > CMAP_green) temp_distance += green - CMAP_green;
        else temp_distance += CMAP_green - green;
        if(blue > CMAP_blue) temp_distance += blue - CMAP_blue;
        else temp_distance += CMAP_blue - blue;

        // If new minimum distance found, set to return pixel
        if(temp_distance < min_distance && color_map.list_cartridges[i].capacity > 0) {
            min_distance = temp_distance;
            candidate = color_map.list_cartridges[i].color; 
            candidate_index = i;
        }

        // Reset temp distance
        temp_distance = 0;
    }

    // If image is printing, after each LEGO piece printed, remove one from cartridge capacity
    if(color_map.num_cartridges && printing_state) (color_map.list_cartridges[candidate_index].capacity)--;

    return candidate;
}

// !!! DELETE IF BROKEN - BETA FEATURE
/* Function: are_neighbors_matching()
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 * Returns true if all neighbors of input pixel match `pixel_to_match`, and false if not.
 */
bool are_neighbors_matching(const struct img *input, unsigned int pixel, unsigned int pixel_to_match, int index) {
    // If current pixel color isn't `pixel_to_match`, terminate
    if(pixel != pixel_to_match) return false;

    // Declare edge cases
    bool top_edge_case = false, right_edge_case = false, left_edge_case = false, bottom_edge_case = false;

    // Initialize edge cases
    if(index < input->width) top_edge_case = true;
    if(index > input->width * (input->height - 1)) bottom_edge_case = true;
    if(index % input->width == 0) left_edge_case = true;
    if(index % (input->width - 1) == 0) right_edge_case = true;

    // Perform checks based on edge cases
    if(!top_edge_case && input->pixels[index - input->width] != pixel_to_match) return false;
    if(!bottom_edge_case && input->pixels[index + input->width] != pixel_to_match) return false;
    if(!left_edge_case && input->pixels[index - 1] != pixel_to_match) return false;
    if(!right_edge_case && input->pixels[index + 1] != pixel_to_match) return false;

    // If all checks pass, return true
    return true;
}

// External Constants
const unsigned int BITMAP_LIST_SIZE = 18;
const struct img* BITMAP_LIST[] = {
    &skull_emoji, 
    &mona_lisa, 
    &mango,
    &fire_hydrant,
	&stanford,
	&raspberry,
	&yoda,
	&obama,
	&smile,
	&tractor,
	&yoshi,
	&apple,
	&cal,
	&instagram,
	&totoro,
	&bulbasaur,
	&message,
	&wave,
};