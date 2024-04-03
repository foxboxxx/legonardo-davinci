/* File: printer.c
 * =-=-=-=-=-=-=-=
 * @author Gui Suranyi, Winter 2024
 * ...
 */ 

// Library Imports
#include "gl.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "gpio_interrupt.h"
#include "interrupts.h"
#include "printf.h"
#include "ringbuffer.h"
#include "uart.h"
#include "malloc.h"
#include "hstimer.h"

// Project Imports
#include "printer_driver.h"

static const gpio_id_t X_STEP_PIN = GPIO_PB4, Y_STEP_PIN = GPIO_PB3, Z_STEP_PIN = GPIO_PB2;
static const gpio_id_t X_DIR_PIN = GPIO_PD17, Y_DIR_PIN = GPIO_PB6, Z_DIR_PIN = GPIO_PB12;
static const gpio_id_t X_LIMIT_PIN = GPIO_PC0, Y_LIMIT_PIN = GPIO_PC1, Z_LIMIT_PIN = GPIO_PB11;

static const gpio_id_t VACUUM_PIN = GPIO_PB10;

static volatile  int X_STEP_PIN_State, X_Steps, X_Current_Steps, X_Target_Interval, X_Current_Interval, X_Intermediary_Interval;

static volatile  int Y_STEP_PIN_State, Y_Steps, Y_Current_Steps, Y_Target_Interval, Y_Current_Interval, Y_Intermediary_Interval;

static volatile  int Z_STEP_PIN_State, Z_Steps, Z_Current_Steps, Z_Target_Interval, Z_Current_Interval, Z_Intermediary_Interval;

static volatile int X_Position, Y_Position, Z_Position;

static volatile unsigned int X_Zero_Reference = 45000, Y_Zero_Reference = 3000;

unsigned int find_max (unsigned int x, unsigned int y, unsigned int z) {

    if (x == 0 && y == 0 && z == 0){
        return 0;
    }
    else if (x >= y && x > z){
        return 1;
    }
    else if (y > x && y >= z){
        return 2;
    }
    else {
        return 3;
    }
}

static void steppers(uintptr_t pc, void *aux_data) {
    hstimer_interrupt_clear(HSTIMER0);

    //movement for X stepper motor
    if (X_Steps > 0){
        X_Current_Interval ++;
        if (X_Current_Interval == X_Intermediary_Interval){
            if (X_STEP_PIN_State == 1){
                gpio_write(X_STEP_PIN, 0);
                X_STEP_PIN_State = 0;
                X_Steps --;
                if (X_Intermediary_Interval > X_Target_Interval && X_Steps > 100){
                    X_Intermediary_Interval --;
                }
                else {
                    X_Intermediary_Interval ++;
                }
            }
            else {
                gpio_write(X_STEP_PIN, 1);
                X_STEP_PIN_State = 1;
            }
            X_Current_Interval = 0;
        }
    }

    //movement for Y stepper motor
    if (Y_Steps > 0){
        Y_Current_Interval ++;
        if (Y_Current_Interval == Y_Intermediary_Interval){
            if (Y_STEP_PIN_State == 1){
                gpio_write(Y_STEP_PIN, 0);
                Y_STEP_PIN_State = 0;
                Y_Steps --;
                if (Y_Intermediary_Interval > Y_Target_Interval && Y_Steps > 100){
                    Y_Intermediary_Interval --;
                }
                else {
                    Y_Intermediary_Interval ++;
                }
            }
            else {
                gpio_write(Y_STEP_PIN, 1);
                Y_STEP_PIN_State = 1;
            }
            Y_Current_Interval = 0;
        }
    }

    //movement for Z stepper motor
    if (Z_Steps > 0){
        Z_Current_Interval ++;
        if (Z_Current_Interval == Z_Intermediary_Interval){
            if (Z_STEP_PIN_State == 1){
                gpio_write(Z_STEP_PIN, 0);
                Z_STEP_PIN_State = 0;
                Z_Steps --;
                if (Z_Intermediary_Interval > Z_Target_Interval && Z_Steps > 100){
                    Z_Intermediary_Interval --;
                }
                else {
                    Z_Intermediary_Interval ++;
                }
            }
            else {
                gpio_write(Z_STEP_PIN, 1);
                Z_STEP_PIN_State = 1;
            }
            Z_Current_Interval = 0;
        }
    }
    
}

void configure_steppers(void) {
    // armtimer is intialized to number of usecs between events
    gpio_set_output(X_STEP_PIN);
    gpio_set_output(Y_STEP_PIN);
    gpio_set_output(Z_STEP_PIN);
    gpio_set_output(X_DIR_PIN);
    gpio_set_output(Y_DIR_PIN);
    gpio_set_output(Z_DIR_PIN);
    hstimer_init(HSTIMER0, 10);
    hstimer_enable(HSTIMER0);             // enable timer itself
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER0, steppers, NULL);
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER0);
}

void configure_limit_switches(void){
    gpio_set_input (X_LIMIT_PIN);
    gpio_set_pullup(X_LIMIT_PIN);
    gpio_set_input (Y_LIMIT_PIN);
    gpio_set_pullup(Y_LIMIT_PIN);
    gpio_set_input (Z_LIMIT_PIN);
    gpio_set_pullup(Z_LIMIT_PIN);
}

void configure_vacuum (void) {
    gpio_set_output(VACUUM_PIN);
}

void activate_vacuum (void) {
    gpio_write(VACUUM_PIN, 1);
}

void deactivate_vacuum (void) {
    gpio_write(VACUUM_PIN, 0);
}

void move_steppers (int x, int y, int z, int velocity){
    // If the desired x move, is less than 0, we need to flip the Dir pin output, and get the absolute of the steps
    if (x < 0){
        gpio_write(X_DIR_PIN, 0);
        X_Steps = -x;
    }
    else {
        gpio_write(X_DIR_PIN, 1);
        X_Steps = x;
    }

    if (y < 0){
        gpio_write(Y_DIR_PIN, 0);
        Y_Steps = -y;
    }
    else {
        gpio_write(Y_DIR_PIN, 1);
        Y_Steps = y;
    }

    if (z < 0){
        gpio_write(Z_DIR_PIN, 0);
        Z_Steps = -z;
    }
    else {
        gpio_write(Z_DIR_PIN, 1);
        Z_Steps = z;
    }

    unsigned int largestMove = find_max(X_Steps, Y_Steps, Z_Steps);
    X_Intermediary_Interval = 150; // STARTING SPEED FOR THE STEPPERS
    Y_Intermediary_Interval = 150; // LOWER SPEED = MORE TORQUE
    Z_Intermediary_Interval = 150;

    X_Target_Interval = velocity;
    Y_Target_Interval = velocity;
    Z_Target_Interval = velocity;
}

void home_steppers(void){
    while (gpio_read(Z_LIMIT_PIN) == 1){
        gpio_write(Z_LIMIT_PIN, 0);
        Z_Steps = 101;
        Z_Target_Interval = 10;
        Z_Intermediary_Interval = 20;
    }
    
    while (Z_Steps != 0) {}

    while (gpio_read(Y_LIMIT_PIN) == 1){
        gpio_write(Y_DIR_PIN, 0);
        Y_Steps = 101;
        Y_Target_Interval = 10;
        Y_Intermediary_Interval = 20;
    }

    while (Y_Steps != 0) {}

    while (gpio_read(X_LIMIT_PIN) == 1){
        gpio_write(X_DIR_PIN, 0);
        X_Steps = 101;
        X_Target_Interval = 10;
        X_Intermediary_Interval = 20;
    }
    while (X_Steps != 0) {}
    
}

void move_to(int X_Desired_Position, int Y_Desired_Position, int Z_Desired_Position, int speed){
    //calculate how much each motor needs to move
    int X_move = X_Desired_Position - X_Position;
    int Y_move = Y_Desired_Position - Y_Position;
    int Z_move = Z_Desired_Position - Z_Position;

    // update the current position for each of the axis
    X_Position = X_Desired_Position;
    Y_Position = Y_Desired_Position;
    Z_Position = Z_Desired_Position;

    //move the stepper to the desired position
    printf("X_move: %d,  Y_move: %d, Z_move: %d,   %d\n", X_move, Y_move, Z_move, ((X_move * X_move) + (Y_move * Y_move) + (Z_move * Z_move)));
    move_steppers(X_move, Y_move, Z_move, speed);

    while (X_Steps != 0 || Y_Steps != 0 || Z_Steps != 0) {}

}

void pick_and_place(int X_End_Position, int Y_End_Position, coordinate color){
    int Brick_X_Coordinate = color.x;
    int Brick_Y_Coordinate = color.y; 

    unsigned int X_Coordinate_End_Position = X_End_Position * 3200 + X_Zero_Reference;
    unsigned int Y_Coordinate_End_Position = Y_End_Position * 3200 + Y_Zero_Reference;

    move_to(Brick_X_Coordinate, Brick_Y_Coordinate, 3000, 7);                    // First go to the position where the brick of the specified color is located
    move_to(Brick_X_Coordinate, Brick_Y_Coordinate, 8000, 10);                   // Then lower the pick and place nozzle 
    activate_vacuum();                                                           // Turn the vacuum on
    move_to(Brick_X_Coordinate, Brick_Y_Coordinate, 3000, 10);                   // Go back up
    move_to(X_Coordinate_End_Position, Y_Coordinate_End_Position, 3000, 7);      // Go to the end position of the brick
    move_to(X_Coordinate_End_Position, Y_Coordinate_End_Position, 9000, 10);     // Place the part
    deactivate_vacuum();                                                         // Turn the vacuum off
    move_to(X_Coordinate_End_Position, Y_Coordinate_End_Position, 3000, 10);     // Raise the pick and place arm
    
}