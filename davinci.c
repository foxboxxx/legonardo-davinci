/* File: davinci.c
 * =-=-=-=-=-=-=-=
 * @author Joe Robertazzi - Winter 2024 - <tazzi@stanford.edu>
 * Main program to run Legonardo Davinci project. Contains
 * all relavent dependencies and runs the printer in main
 * function.
 */ 

// Library Imports
#include "console.h"
#include "interrupts.h"
#include "keyboard.h"
#include "shell.h"
#include "timer.h"
#include "uart.h"

// Project Imports
#include "bitmaps.h"
#include "colormaps.h"
#include "img_process.h"
#include "printer.h"
#include "printer_assets.h"
#include "printer_driver.h"

// Constants
#define NA -1

// Main Executon
void main(void) {
    // Asset Initialization:
    interrupts_init();
    gpio_init();
    timer_init();
    uart_init();

    // ----- BUTTON TEST
    configure_steppers();
    configure_limit_switches();
    configure_vacuum();
    // ----- BUTTON TEST


    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    // Shell Initialization:
    // console_init(30, 80, GL_GREEN, GL_BLACK);
    // shell_init(keyboard_read_next, console_printf);
    // interrupts_global_enable();
    // shell_run();

    // Printer Initialization:
    printer_init(keyboard_read_next);
    interrupts_global_enable();
    printer_run();
}