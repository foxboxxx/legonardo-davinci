/* File: printer.c
 * =-=-=-=-=-=-=-=
 * @author Joe Robertazzi, Winter 2024 - <tazzi@stanford.edu>
 * Contains all relavent functions to display and run all pages within 
 * the printer app including title page, selection screen, and printing
 * page.
 */ 

// Standard Library Imports
#include <stddef.h>

// Library Imports
#include "uart.h"
#include "strings.h"
#include "printf.h"
#include "malloc.h"
#include "timer.h"
#include "mango.h"
#include "console.h"
#include "gl.h"
#include "shell.h"
#include "keyboard.h"

// Project Imports
#include "printer.h"
#include "bitmaps.h"
#include "colormaps.h"
#include "img_process.h"
#include "printer_assets.h"
#include "printer_driver.h"

// Scene Modes
#define MODE_TITLE -2
#define MODE_QUIT -1
#define MODE_SELECT 0
#define MODE_PREVIEW 1
#define MODE_PRINTING 2

// Preview Mode Indices
#define COLOR_MODE 0
#define SCALE_MODE 1
#define PRINT_MODE 2
#define MENU_SIZE 13

// Scale Mode Indices 
#define PREV 0
#define TWNTY 1
#define FRTY_TL 2
#define FRTY_TR 3
#define FRTY_BL 4
#define FRTY_BR 5

// Key Constants
#define LEFT 0
#define RIGHT 1
#define ESC 0x95
#define ENTER 0x0a

// Arrow Keys
#define UP_ARROW 0xaa
#define DOWN_ARROW 0xab
#define LEFT_ARROW 0xac
#define RIGHT_ARROW 0xad

// Constants
#define MAX_COLORS 30
#define KEEP_TRACK_OF_LEGOS true // !! IMPORTANT -> (change to false to stop keeping track of LEGOs)
#define BACKGROUND_REMOVAL true // !! IMPORTANT  -> (change to false to disable image background removal feature)
#define COLOR_X 700
#define COLOR_Y 3500
#define COLOR_X_OFFSET 4000
#define CHAR_LIM 1024

// Global Variables
int mode = MODE_TITLE;
unsigned int quit_selection = LEFT;
unsigned int bmp_index = 0;
unsigned int idx_offset = 0;
unsigned int preview_menu_index = 0;
unsigned int printer_index = 0;
unsigned int scale_index = 0;
unsigned int image_index = 0;
struct img** curr_print;

// Module-level global variables for printer
static struct {
    input_fn_t printer_read;
} module;

/* Function: printer_init()
 * =-=-=-=-=-=-=-=-=-=-=-=-
 * Initializes the printer application, and takes one argument
 * `read_fn` which is the function that reads inputs.
 */
void printer_init(input_fn_t read_fn) {
    // Printer initialization
    module.printer_read = read_fn;	
    const int height = 20 * (gl_get_char_height() + 5);
    const int width = 40 * gl_get_char_width();
    gl_init(width, height, GL_DOUBLEBUFFER);
}

/* Function: print_title()
 * =-=-=-=-=-=-=-=-=-=-=-=
 * Displays the LEGONARDO DAVINCI splash screen for 
 * printer application. Switches scenes on ENTER press.
 */
void print_title(void) {
    if(mode == MODE_TITLE) display_image((const struct img*)&title, 0, 0, gl_get_width(), gl_get_height(), false, 0);
}

/* Function: print_header()
 * =-=-=-=-=-=-=-=-=-=-=-=-
 * Displays the header portion of the menu on the
 * selection screen.
 */
void print_header(void) {
    if(mode > MODE_TITLE && mode < MODE_PRINTING) {
        // Header content
        char *header = (char *)"LEGONARDO DAVINCI V2.0";
        char *menu = (char *)"MENU";
        char *preview = (char *)"PREVIEW";

        // Display headers on screen
        gl_draw_string((40 * gl_get_char_width()) / 2 - (strlen(header) / 2 * gl_get_char_width()), 5, header, GL_ORANGE);
        gl_draw_rect((40 * gl_get_char_width()) / 2 - 2, 5 + 2 * gl_get_char_width(), 4, gl_get_height() - (3 * (5 + gl_get_char_width())), GL_WHITE);
        gl_draw_string((40 * gl_get_char_width()) / 4 - (strlen(menu) * gl_get_char_width() / 2), 10 + gl_get_char_height(), menu, GL_AMBER);
        gl_draw_string((40 * gl_get_char_width()) / 4 * 3 - (strlen(preview) * gl_get_char_width() / 2), 10 + gl_get_char_height(), preview, GL_AMBER);
    }
}

/* Function: print_quit()
 * =-=-=-=-=-=-=-=-=-=-=-
 * Displays the quit prompt on the screen.
 * Clicking enter on YES quits, and NO dismisses.
 */
void print_quit(void) {
    if(mode == MODE_QUIT) {
        // Displays quit prompt on screen
        gl_draw_rect(gl_get_width() / 16 * 5, gl_get_height() / 3, gl_get_width() / 16 * 6, gl_get_height() / 3, GL_WHITE);
        gl_draw_rect(gl_get_width() / 16 * 5 + 4, gl_get_height() / 3 + 4, gl_get_width() / 16 * 6 - 8, gl_get_height() / 3 - 8, GL_BLACK);
        gl_draw_string(gl_get_width() / 2 - strlen("DO YOU WANT")*gl_get_char_width() / 2, gl_get_height() / 3 + gl_get_char_height() * 3 / 2 + 4, "DO YOU WANT", GL_WHITE);
        gl_draw_string(gl_get_width() / 2 - strlen("TO QUIT?")*gl_get_char_width() / 2, gl_get_height() / 3 + gl_get_char_height() * 7 / 2 + 4, "TO QUIT?", GL_WHITE);

        // Highlight left button on "NO" selection
        if(quit_selection == LEFT) {
            gl_draw_rect(gl_get_width() * 13 / 32 - strlen("NO")*gl_get_char_width() / 2 - 2, gl_get_height() / 3 + gl_get_char_height() * 11 / 2 - 2 + 4, strlen("NO")*gl_get_char_width() + 4, gl_get_char_height() + 4, GL_WHITE);
            gl_draw_string(gl_get_width() * 13 / 32 - strlen("NO")*gl_get_char_width() / 2, gl_get_height() / 3 + gl_get_char_height() * 11 / 2 + 4, "NO", GL_BLACK);
            gl_draw_string(gl_get_width() * 19 / 32 - strlen("YES")*gl_get_char_width() / 2, gl_get_height() / 3 + gl_get_char_height() * 11 / 2 + 4, "YES", GL_WHITE);
        }

        // Highlight right button on "YES" selection
        else if(quit_selection == RIGHT) {
            gl_draw_rect(gl_get_width() * 19 / 32 - strlen("YES")*gl_get_char_width() / 2 - 2, gl_get_height() / 3 + gl_get_char_height() * 11 / 2 - 2 + 4, strlen("YES")*gl_get_char_width() + 4, gl_get_char_height() + 4, GL_WHITE);
            gl_draw_string(gl_get_width() * 13 / 32 - strlen("NO")*gl_get_char_width() / 2, gl_get_height() / 3 + gl_get_char_height() * 11 / 2 + 4, "NO", GL_WHITE);
            gl_draw_string(gl_get_width() * 19 / 32 - strlen("YES")*gl_get_char_width() / 2, gl_get_height() / 3 + gl_get_char_height() * 11 / 2 + 4, "YES", GL_BLACK);
        }
    }
}

/* Function: print_menu()
 * =-=-=-=-=-=-=-=-=-=-=-
 * Displays menu portion on SELECTION screen. Allows user
 * to scroll through all available images, highlighting 
 * the image currently selected.
 */
void print_menu(void) {
    if(mode > MODE_TITLE && mode < MODE_PRINTING) {
        // If menu selection is at bottom, offset all entries by one to scroll down page to next entry
        if(bmp_index == MENU_SIZE + idx_offset) idx_offset++;

        // If menu selection at top, offset all entries by negative ones to scroll up page to previous entry
        else if(bmp_index < idx_offset && idx_offset > 0) idx_offset--;
        
        // Display all entries on the list
        for(int i = 0; i < MENU_SIZE; i++) {
            // Header offset
            int header_height = 15 + (2 * gl_get_char_height());
            int padding = 6;
            int inner = 2;

            // Set default entry to "empty"
            char * entry = (char *)"(empty)";
            int row_offset = (gl_get_char_height() + 2 * padding) * i;
            
            // Draw box on the left of current entry
            gl_draw_rect(5, header_height + row_offset, gl_get_char_width() + padding, gl_get_char_height() + padding, GL_WHITE);
            gl_draw_rect(5 + inner, header_height + inner + row_offset, gl_get_char_width() + inner, gl_get_char_height() + inner, GL_BLACK);
            
            // If current entry is within BITMAP_LIST, set string to name of current image on BITMAP_LIST
            if(i < BITMAP_LIST_SIZE) entry = BITMAP_LIST[i + idx_offset]->name;
            
            // If current image is selected, highlight text and mark an "X" in the box to the left of it
            if(i == bmp_index - idx_offset) {
                gl_draw_char(5 + 3, header_height + row_offset + 4, 'X', GL_WHITE);
                if(mode <= MODE_SELECT) gl_draw_rect(gl_get_char_width() + 3 * padding, header_height + padding / 2 + row_offset - 1, strlen(entry) * gl_get_char_width(), gl_get_char_height() + 2, GL_WHITE);
                if(mode >= MODE_PREVIEW) gl_draw_rect(gl_get_char_width() + 3 * padding, header_height + padding / 2 + row_offset - 1, strlen(entry) * gl_get_char_width(), gl_get_char_height() + 2, GL_AMBER);
                gl_draw_string(gl_get_char_width() + 3 * padding , header_height + padding / 2 + row_offset, entry, GL_BLACK);
            }

            // If image not in map, set text to empty and make it GL_SILVER
            else {
                if(strcmp((const char*)entry, "(empty)") == 0) gl_draw_string(gl_get_char_width() + 3 * padding , header_height + padding / 2 + row_offset, entry, GL_SILVER);
                else gl_draw_string(gl_get_char_width() + 3 * padding , header_height + padding / 2 + row_offset, entry, GL_WHITE);
            }
        }
    }
}

/* Function: print_preview()
 * =-=-=-=-=-=-=-=-=-=-=-
 * Displays preview portion on SELECTION screen. Allows user
 * to scroll through color modes and scale modes of selected 
 * image. Also gives option to enter PRINT screen.
 */
void print_preview(void) {
    if(mode > MODE_TITLE && mode < MODE_PRINTING) {
        // Set default preview to 80x80 version of image
        struct img *init_down_scale = down_scale_image(BITMAP_LIST[bmp_index], 80, 80);
        init_down_scale = format_image(init_down_scale, *PRINTER_LIST[printer_index], false);
        // Scale up by factor of 3 to make the image 240x240 (but still look 80x80)
        struct img *curr_scaled = up_scale_image(init_down_scale, 3, 3);

        // If not preview
        if(scale_index != PREV) {
            // Free image
            free(init_down_scale);
            free(curr_scaled);

            // Check scale modes, and adjust image accordingly
            if(scale_index == TWNTY) init_down_scale = down_scale_image(BITMAP_LIST[bmp_index], 20, 20);
            else {
                if(scale_index == FRTY_TL) init_down_scale = crop_image((const struct img*)down_scale_image(BITMAP_LIST[bmp_index], 40, 40), 0, 0, 20, 20);
                if(scale_index == FRTY_TR)init_down_scale = crop_image((const struct img*)down_scale_image(BITMAP_LIST[bmp_index], 40, 40), 20, 0, 20, 20);
                if(scale_index == FRTY_BL)init_down_scale = crop_image((const struct img*)down_scale_image(BITMAP_LIST[bmp_index], 40, 40), 0, 20, 20, 20);
                if(scale_index == FRTY_BR)init_down_scale = crop_image((const struct img*)down_scale_image(BITMAP_LIST[bmp_index], 40, 40), 20, 20, 20, 20);
            }

            // Format image to correct color map and scale up to 240x240 (but still look 20x20)
            curr_scaled = format_image(init_down_scale, *PRINTER_LIST[printer_index], KEEP_TRACK_OF_LEGOS);        
            curr_scaled = up_scale_image(curr_scaled, 12, 12);
        }

        // Display preview of image print along with white border around it
        gl_draw_rect(293, 18 + (2 * gl_get_char_height()), 244, 244, GL_WHITE);
        display_image(curr_scaled, 295, 20 + (2 * gl_get_char_height()), 240, 240, false, 0);

        // Set current image global to current selected image
        *curr_print = curr_scaled;
        free(init_down_scale);

        // Color mode menu under image on preview tab
        gl_draw_string(295, 268 + (5 * gl_get_char_height() / 2), "COLORMODE:<", GL_WHITE);
        const char* printer_name = PRINTER_LIST[printer_index]->name_printer;
        unsigned int printer_name_offset = (5 - strlen(printer_name)) * gl_get_char_width() / 2;
        if(preview_menu_index == COLOR_MODE && mode == MODE_PREVIEW){
            gl_draw_rect(295 + strlen("COLORMODE:<")*gl_get_char_width() + printer_name_offset - 1, 268 + (5 * gl_get_char_height() / 2) - 1, strlen(printer_name)*gl_get_char_width() + 2, gl_get_char_height() + 2, GL_WHITE);
            gl_draw_string(295 + strlen("COLORMODE:<")*gl_get_char_width() + printer_name_offset, 268 + (5 * gl_get_char_height() / 2), printer_name, GL_BLACK);
        }
        else gl_draw_string(295 + strlen("COLORMODE:<")*gl_get_char_width() + printer_name_offset, 268 + (5 * gl_get_char_height() / 2), printer_name, GL_WHITE);
        gl_draw_string(295 + strlen("COLORMODE:<_____")*gl_get_char_width(), 268 + (5 * gl_get_char_height() / 2), ">", GL_WHITE);

        // Scale mode menu under image on preview tab
        gl_draw_string(295, 268 + (4 * gl_get_char_height()) + 4, "SCALEMODE:<", GL_WHITE);
        const char* scale_name = SCALE_MODE_LIST[scale_index]->name;
        unsigned int scale_name_offset = (5 - strlen(scale_name)) * gl_get_char_width() / 2;
        if(preview_menu_index == SCALE_MODE && mode == MODE_PREVIEW){
            gl_draw_rect(295 + strlen("SCALEMODE:<")*gl_get_char_width() + scale_name_offset - 1, 268 + (4 * gl_get_char_height()) + 4 - 1, strlen(scale_name)*gl_get_char_width() + 2, gl_get_char_height() + 2, GL_WHITE);
            gl_draw_string(295 + strlen("SCALEMODE:<")*gl_get_char_width() + scale_name_offset, 268 + (4 * gl_get_char_height()) + 4, scale_name, GL_BLACK);
        }
        else gl_draw_string(295 + strlen("SCALEMODE:<")*gl_get_char_width() + scale_name_offset, 268 + (4 * gl_get_char_height()) + 4, scale_name, GL_WHITE);
        gl_draw_string(295 + strlen("SCALEMODE:<_____")*gl_get_char_width(), 268 + (4 * gl_get_char_height()) + 4, ">", GL_WHITE);

        // Print button under image on preview tab
        unsigned int TEXT_COLOR = GL_WHITE;
        gl_draw_rect(gl_get_width() / 4 * 3 - strlen("PRINT")*gl_get_char_width() / 2 - 10, 268 + (13 * gl_get_char_height() / 2) - 6, strlen("PRINT")*gl_get_char_width() + 18, gl_get_char_height() + 18, GL_WHITE);
        if(preview_menu_index == PRINT_MODE && mode == MODE_PREVIEW) {
            TEXT_COLOR = GL_BLACK;
        }
        else gl_draw_rect(gl_get_width() / 4 * 3 - strlen("PRINT")*gl_get_char_width() / 2 - 8, 268 + (13 * gl_get_char_height() / 2) - 3, strlen("PRINT")*gl_get_char_width() + 14, gl_get_char_height() + 12, GL_BLACK);
        gl_draw_string(gl_get_width() / 4 * 3 - strlen("PRINT")*gl_get_char_width() / 2, 268 + (13 * gl_get_char_height() / 2) + 4, "PRINT", TEXT_COLOR);
    }
}

/* Function: print_printer()
 * =-=-=-=-=-=-=-=-=-=-=-
 * Displays PRINTING screen, with live print of selected
 * image along with counts of pixels being used for print.
 */
void print_printer(void) {
    if(mode == MODE_PRINTING) {
        // Realtime printer display dimensions
        unsigned int width = 20;
        unsigned int height = 20;
        unsigned int up_scale = 300;

        // Downscale image to 20x20 mode as default, if not, check for the different 40x40 modes
        struct img *down_scale = down_scale_image(BITMAP_LIST[bmp_index], 20, 20);
        if(scale_index != TWNTY) {
            free(down_scale);
            if(scale_index == FRTY_TL)down_scale = crop_image((const struct img*)down_scale_image(BITMAP_LIST[bmp_index], 40, 40), 0, 0, 20, 20);
            if(scale_index == FRTY_TR)down_scale = crop_image((const struct img*)down_scale_image(BITMAP_LIST[bmp_index], 40, 40), 20, 0, 20, 20);
            if(scale_index == FRTY_BL)down_scale = crop_image((const struct img*)down_scale_image(BITMAP_LIST[bmp_index], 40, 40), 0, 20, 20, 20);
            if(scale_index == FRTY_BR)down_scale = crop_image((const struct img*)down_scale_image(BITMAP_LIST[bmp_index], 40, 40), 20, 20, 20, 20);
        }

        // Format image to selected color map
        struct img *temp = format_image((const struct img*)down_scale, *PRINTER_LIST[printer_index], true);
        free(down_scale);
        down_scale = temp;

        // Home printer back to 0,0
        home_steppers();

        // Printing of image using LEGOs, executes program on LEGONARDO DAVINCI machine and then prints printed piece to screen
        for(int i = 0; i < width * height && mode == MODE_PRINTING; i++) {

            // Removes black background from images (to save LEGO pieces)
            if(BACKGROUND_REMOVAL && ((down_scale->pixels[i] == GL_BLACK))) continue;

            // Determine offset for color pick-up
            unsigned int offset = 0;
            for(int j = 0; j < PRINTER_LIST[printer_index]->num_cartridges; j++) {
                if(PRINTER_LIST[printer_index]->list_cartridges[j].color == down_scale->pixels[i]) {
                    offset = j;
                    break;
                }   
            }

            // Move printer to color, pick up color, move to pixel location, place pixel
            coordinate color_pickup;
            color_pickup.x = COLOR_X + (COLOR_X_OFFSET * offset);
            color_pickup.y = COLOR_Y;
            pick_and_place(i % 20, i / 20, color_pickup);

            // Realtime print section of screen
            gl_clear(GL_BLACK);
            gl_draw_string(309 / 2 - strlen("REALTIME PRINT")*gl_get_char_width() / 2, 10, "REALTIME PRINT", GL_AMBER);
            gl_draw_rect(5, 10 + gl_get_char_height() * 3 / 2, 304, 304, GL_WHITE);
            gl_draw_rect(7, 12 + gl_get_char_height() * 3 / 2, 300, 300, GL_BLACK);
            for(int j = 0; j <= i; j++) {
                
                // !!! DELETE IF BROKEN - BETA FEATURE
                if(BACKGROUND_REMOVAL && ((down_scale->pixels[j] == GL_BLACK))) continue;
                // !!! DELETE IF BROKEN - BETA FEATURE

                unsigned int x_pos = j % width * up_scale / width;
                unsigned int y_pos = j / width * up_scale / height;
                gl_draw_rect(7 + x_pos, 12 + gl_get_char_height() * 3 / 2 + y_pos, up_scale / width, up_scale / height, down_scale->pixels[j]);
            }

            // Calculating percent of image printed, and converting to string
            char percent_str[CHAR_LIM];    
            unsigned long percent = (1 + i) * 100 / (width * height);
            if(!percent) {
                percent_str[0] = '0';
                percent_str[1] = '\0';
            }
            else num_to_string(percent_str, CHAR_LIM, percent, 10, 0);
            strlcat(percent_str, "%", CHAR_LIM);

            // Progress bar section of screen under realtime print
            gl_draw_string(309 / 2 - strlen("PROGRESS:____")*gl_get_char_width() / 2, 328 + gl_get_char_height() * 2, "PROGRESS:", GL_AMBER);
            gl_draw_string(309 / 2 - strlen("PROGRESS:____")*gl_get_char_width() / 2 + strlen("PROGRESS:")*gl_get_char_width(), 328 + gl_get_char_height() * 2, percent_str, GL_AMBER);
            gl_draw_rect(5, 328 + gl_get_char_height() * 7 / 2, 304, 38, GL_WHITE);
            gl_draw_rect(9, 332 + gl_get_char_height() * 7 / 2, 296, 28, GL_BLACK);
            gl_draw_rect(9, 332 + gl_get_char_height() * 7 / 2, 296 * percent / 100, 28, GL_MOSS);

            // Cartridge levels on right side of screen - prints how many pieces of each color remain
            gl_draw_string(436 - strlen("INK CARTRIDGES")*gl_get_char_width() / 2, 10, "INK CARTRIDGES", GL_AMBER);
            for(int c = 0; c < MAX_COLORS; c++) {
                gl_draw_rect(320 + (c % 3 * gl_get_char_width() * 6), 36 + (c / 3 * gl_get_char_height() * 5/2), 20, 20, GL_WHITE);
                if(c < PRINTER_LIST[printer_index]->num_cartridges) {
                    // Draw square of color on left side of number
                    gl_draw_rect(322 + (c % 3 * gl_get_char_width() * 6), 38 + (c / 3 * gl_get_char_height() * 5/2), 16, 16, PRINTER_LIST[printer_index]->list_cartridges[c].color);
                    
                    // Calculate number of LEGOs and convert to string
                    char count_str[CHAR_LIM];
                    count_str[0] = '\0';
                    unsigned long lego_count = PRINTER_LIST[printer_index]->list_cartridges[c].capacity;
                    num_to_string(count_str, CHAR_LIM, lego_count, 10, 0);
                    if(!lego_count) {
                        count_str[0] = '0';
                        count_str[1] = '\0';
                    }
                    
                    // Print string next to color
                    gl_draw_string(342 + (c % 3 * gl_get_char_width() * 6), 38 + (c / 3 * gl_get_char_height() * 5/2), count_str, GL_WHITE);

                    // Remove printed pixel from capacity
                    if(PRINTER_LIST[printer_index]->list_cartridges[c].color == down_scale->pixels[i]) {
                        (PRINTER_LIST[printer_index]->list_cartridges[c].capacity)--;
                    }
                }
                // If colors less than max of 30, print "N/A" with color square showing up as red "X"
                else {
                    gl_draw_rect(322 + (c % 3 * gl_get_char_width() * 6), 38 + (c / 3 * gl_get_char_height() * 5/2), 16, 16, GL_BLACK);
                    gl_draw_char(323 + (c % 3 * gl_get_char_width() * 6), 39 + (c / 3 * gl_get_char_height() * 5/2), 'X', GL_RED);
                    gl_draw_string(342 + (c % 3 * gl_get_char_width() * 6), 38 + (c / 3 * gl_get_char_height() * 5/2), "N/A", GL_WHITE);
                }
            }
            gl_swap_buffer();

        }
        free(down_scale);
        gl_swap_buffer();
    }

}

/* Function: printer_read_input()
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 * Waits for next keyboard input, and alters screen
 * accordingly (changing modes, selecting options, etc.).
 */
int printer_read_input(void) {
    unsigned char input = module.printer_read();

    // If title screen, wait for enter to be pressed
    if(mode == MODE_TITLE) {
        if(input == ENTER) mode = MODE_SELECT;   
        return 1; // Continue
    }

    // If quit screen, either quit application or cancel (depending on choice)
    if(mode == MODE_QUIT) {
        if(input == LEFT_ARROW) quit_selection = LEFT;
        else if(input == RIGHT_ARROW) quit_selection = RIGHT;
        else if(input == ENTER && quit_selection == RIGHT) return 0; // Quit
        else if(input == ESC) {
            mode = MODE_SELECT;
            return 1; // Continue
        }
    }

    // If select screen (menu side)
    if(mode == MODE_SELECT) {
        if((input == UP_ARROW && bmp_index > 0) || (input == DOWN_ARROW && bmp_index < BITMAP_LIST_SIZE - 1)) {
            printer_index = 0;
            scale_index = 0;
            preview_menu_index = 0;
        }

        // Up arrow, move up list, down arrow, move down list
        if(input == UP_ARROW && bmp_index > 0) bmp_index--;
        else if(input == DOWN_ARROW && bmp_index < BITMAP_LIST_SIZE - 1) bmp_index++; 
    }

    // If select screen (preview side)
    if(mode == MODE_PREVIEW) {
        // Change modes up and down depending on up arrow / down arrow
        if(input == UP_ARROW && preview_menu_index > 0) preview_menu_index--;
        else if(input == DOWN_ARROW && preview_menu_index < 2) preview_menu_index++;
        
        // Cycle through options on color mode
        else if(preview_menu_index == COLOR_MODE) {
            if(input == LEFT_ARROW) {
                if(printer_index == 0) printer_index = NUM_PRINTERS - 1;
                else printer_index--;
            }
            else if(input == RIGHT_ARROW) {
                if(printer_index == NUM_PRINTERS - 1) printer_index = 0;
                else printer_index++;
            }
        }

        // Cycle through options on scale mode
        else if(preview_menu_index == SCALE_MODE) {
            if(input == LEFT_ARROW) {
                if(scale_index == 0) scale_index = NUM_SCALES - 1;
                else scale_index--;
            }
            else if(input == RIGHT_ARROW) {
                if(scale_index == NUM_SCALES - 1) scale_index = 0;
                else scale_index++;
            }
        }
    }

    // After printing is done, switch to preview mode on any input
    if(mode == MODE_PRINTING) {
        mode = MODE_PREVIEW;
        return 1;
    }

    // General cases - ESC goes to previous mode and ENTER goes to next mode
    if(input == ESC) mode--;
    if(input == ENTER && mode <= MODE_PRINTING) {
        if(mode == MODE_PREVIEW && preview_menu_index == PRINT_MODE && scale_index != PREV) mode++;
        if(mode <= MODE_SELECT) mode++;
	}
    return 1;
}

/* Function: printer_run()
 * =-=-=-=-=-=-=-=-=-=-=-=
 * Runs the printer app and keeps it in while loop
 * until program terminates.
 */
void printer_run(void) {
    int state = 1;

    while(state) {
        // Clear to black
        gl_clear(GL_BLACK);

        // Print elements on screen (depending on what mode is active)
        print_title();
        print_header();
        print_menu();
        print_preview();
        print_quit();
        print_printer();

        // Swap buffer and wait for next input
        gl_swap_buffer();
        state = printer_read_input();
    }
}