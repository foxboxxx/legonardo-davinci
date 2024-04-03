#ifndef IMG_H
#define IMG_H

/*
 * Functions for basic image processing library. Performs operations
 * such as displaying images, scaling them up and down, changing the
 * colormaps of images, and cropping images.
 *
 * @author Joe Robertazzi - Winter 2024 - <tazzi@stanford.edu>
 */

// Standard Library Imports
#include <stdbool.h>

// Project Imports
#include "bitmaps.h"
#include "colormaps.h"

// External Constants
extern const struct img* BITMAP_LIST[];
extern const unsigned int BITMAP_LIST_SIZE;

/*
 * `display_image`
 *
 * Displays an input image (in the form of a pointer to an `img` struct) on the screen
 * at {x_start, y_start} pixels. The image remains on the screen for `seconds` seconds.
 *
 * @param input       the image to be displayed
 * @param x_start     the x location of where to begin displaying image
 * @param y_start     the y location of where to begin displaying image
 * @param inp_width   the width of the input image
 * @param inp_height  the height of the input image
 * @param swap        the bool determining whether or not to swap buffers after image drawn
 * @param seconds     the number of seconds that imge is displayed (0 for indefinite)
 */
void display_image(const struct img *input, unsigned int x_start, unsigned int y_start, unsigned int inp_width, unsigned int inp_height, bool swap, unsigned int seconds);

/*
 * `center_crop_image`
 *
 * Crops an inputted image to its center by cutting off the inputted widths and heights.
 *
 * @param input       the image to be cropped
 * @param width_cut   the number of pixels to cut off the horizontals of the image
 * @param height_cut  the number of pixels to cut off the verticls of the image
 * 
 * @return            the center cropped image as type (struct img*)
 */
struct img* center_crop_image(const struct img *input, unsigned int width_cut, unsigned int height_cut);

/*
 * `up_scale_image`
 *
 * Upscales an inputted image to the inputted x and y factors. Allows lower resolution images to
 * be displayed on larger pixel displays at the same size as on smaller pixel displays.
 *
 * @param input       the image to be upscaled
 * @param factor_x    the number of horizontal pixels per original pixel
 * @param height_cut  the number of vertical pixels per original pixel
 * 
 * @return            the upscaled image as type struct img*
 */ 
 struct img* up_scale_image(const struct img *input, unsigned int factor_x, unsigned int factor_y);

/*
 * `down_scale_image`
 *
 * Downscales an inputted image to the inputted width and heights. 
 *
 * @param input         the image to be downscaled
 * @param pixel_width   the number of horizontal pixels on scaled image
 * @param pixel_height  the number of vertical pixels on scaled image
 * 
 * @return              the downscaled image as type struct img*
 */ 
struct img* down_scale_image(const struct img *input, unsigned int pixel_width, unsigned int pixel_height);

/*
 * `palette_convert`
 *
 * Finds the closest matching color to an input pixel unsigned int on an inputted color map
 * and returns that color.
 *
 * @param pixel           the original pixel compared to the color map
 * @param color_map       the color map containing valid pixels
 * @param printing_state  the bool determining whether or not to remove the color from the color map
 *                        (if the printer is printing, the pixel is subtracted from the capacity)
 * 
 * @return                the closest matching pixel within color map as type unsigned int
 */ 
unsigned int palette_convert(unsigned int pixel, struct printer color_map, bool printing_state);

/*
 * `format_image`
 *
 * Wrapper function for `palette_convert`, which instead has an input of an entire image rather than
 * a pixel. Thus, each pixel in the image is converted to the inputted color map via `palette_convert`.
 *
 * @param input           the image to be formatted to an inputted color map
 * @param color_map       the color map containing valid pixels 
 * @param printing_state  the bool determining whether or not to remove the color from the color map
 *                        (if the printer is printing, the pixel is subtracted from the capacity)
 * 
 * @return                the formatted image as type struct img*
 */ 
struct img* format_image(const struct img *input, struct printer color_map, bool printing_state);

/*
 * `crop_image`
 *
 * Crops an inputted image from the input starting point in the image to a new image with
 * the inputted width and height 
 *
 * @param input      the image to be cropped
 * @param x_start    the starting x position of the crop
 * @param y_start    the starting y position of the crop
 * @param width      the width of cropped image
 * @param height     the height of cropped image
 * 
 * @return           the cropped image as type struct img*
 */ 
struct img* crop_image(const struct img *input, unsigned int x_start, unsigned int y_start, unsigned int width, unsigned int height);

/*
 * `are_neighbors_matching`
 *
 * Checks to see if neighboring pixels are matching the `pixel_to_match` (and if the provided pixel
 * matches the `pixel_to_match`). Primarily used to remove black backgrounds from transparent images
 * that have complete black backgrounds.
 *
 * @param input             the image to be scanned
 * @param pixel             the current pixel color
 * @param pixel_to_match    the pixel color to be matched (and check against neighbors)
 * @param index             the index of current pixel analyzed
 * 
 * @return                  the state of the input pixel - true if next to non-black pixel, false 
 *                          if surrounded by black pixels
 */ 
bool are_neighbors_matching(const struct img *input, unsigned int pixel, unsigned int pixel_to_match, int index);

#endif
