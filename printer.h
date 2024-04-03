#ifndef PRINTER_H
#define PRINTER_H

/*
 * Functions for the printer app to be displayed on monitor while printer
 * is printing. Provides a user interface for selecting an image to print,
 * selecting the scale at which said image is printed, and colors used for
 * such a print.
 *
 * @author Joe Robertazzi - Winter 2024 - <tazzi@stanford.edu>
 */

// Standard Library Imports
#include <stddef.h>

// Library Imports
#include "shell.h"

/*
 * `printer_init`
 *
 * Initializes the printer with the function `read_fn` to capture keystrokes. 
 * Additionally initializes the printer's display and frame buffers.
 *
 * @param read_fn     the read function used for determining printer actions - typically keyboard read next
 */
void printer_init(input_fn_t read_fn);

/*
 * `printer_read_input`
 *
 * Reads and evaluates an input on the keyboard (or `read_fn` function). Returns 1 for successful
 * return and 0 for program termination
 * 
 * @return            the int determining whether to continue running the program or to terminate
 */
int printer_read_input(void);

/*
 * `print_title`
 *
 * Displays the title page for the priner app. Shows up as a splash screen for LEGONARDO DAVINICI,
 * and dismisses on press of ENTER key.
 */
void print_title(void);

/*
 * `print_title`
 *
 * Displays the HEADEr on the menu page of the printer app.
 */
void print_header(void);

/*
 * `print_quit`
 *
 * Displays the QUIT prompt on the menu page of the printer app.
 */
void print_quit(void);

/*
 * `print_menu`
 *
 * Displays the MENU section on the menu page of the printer app.
 */
void print_menu(void);

/*
 * `print_preview`
 *
 * Displays the PREVIEW section on the menu page of the printer app.
 */
void print_preview(void);

/*
 * `print_printer`
 *
 * Displays the PRINTING page of the printer app (print in progress).
 */
void print_printer(void);

/*
 * `printer_run`
 *
 * Runs the printer app, displaying all pages in a while loop depending 
 * on the mode of the app.
 */
void printer_run(void);

#endif