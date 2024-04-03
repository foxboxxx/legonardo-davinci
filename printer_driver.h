#ifndef PRINTER_DRIVER_H
#define PRINTER_DRIVER_H

struct coordinate {
    int x;
    int y;
};

typedef struct coordinate coordinate;

unsigned int find_max (unsigned int x, unsigned int y, unsigned int z);

static void steppers(uintptr_t pc, void *aux_data);

void configure_steppers(void);

void configure_limit_switches(void);

void configure_vacuum (void);

void activate_vacuum (void);

void deactivate_vacuum (void);

void move_steppers (int x, int y, int z, int velocity);

void home_steppers(void);

void move_to(int X_Desired_Position, int Y_Desired_Position, int Z_Desired_Position, int speed);

void pick_and_place(int X_End_Position, int Y_End_Position, coordinate color);

#endif