
#include <Arduino.h>
#include <SparkFun_TB6612.h>

#ifndef __CAR_CONTROL_H__
#define __CAR_CONTROL_H__


#define PWM_MIN (0)
#define PWM_MAX (255)

#define MAG_MIN (0)
#define MAG_MAX (100)

typedef enum {
    RIGHT = 1,
    FORWARDS,
    LEFT,
    BACKWARDS,
    RIGHT_FORWARDS,
    LEFT_FORWARDS,
    LEFT_BACKWARDS,
    RIGHT_BACKWARDS
} STATES;

int speed_map(int speed);

void drive_car(int speed, int state, Motor left_motor, Motor right_motor);

void print_driving_state(int8_t state);

#endif