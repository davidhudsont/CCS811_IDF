/**
 * @file Remote_Control_Defines.h
 * @author David Hudson (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <Arduino.h>


#ifndef __REMOTE_CONTROL_H__
#define __REMOTE_CONTROL_H__


#define XCALIBRATION 1775
#define YCALIBRATION 1776

#define Y_POS_MAX 2319  // Forwards
#define Y_NEG_MIN -1776 // Backwards

#define X_POS_MAX 2320  // Left
#define X_NEG_MIN -1775 // Right


#define SPEED_MIN 0
#define SPEED_MAX_FORWARD 100
#define SPEED_MAX_REVERSE -100

#define NUM_OF_ZONES (8)

typedef enum {
    FORWARDS,
    BACKWARDS,
    LEFT,
    RIGHT,
    LEFT_FORWARDS,
    RIGHT_FORWARDS,
    LEFT_BACKWARDS,
    RIGHT_BACKWARDS,
    STOP
} DIRECTION;

typedef enum {
    z1 = 0,
    z2,
    z3,
    z4,
    z5,
    z6,
    z7,
    z8
} ZONES;


// ll 
const float zones_ll[NUM_OF_ZONES] = {
    -5.0f,
    85.0f,
    175.0f,
    265.0f,
    5.0f,
    95.0f,
    185.0f,
    275.0f
};

// ul
const float zones_ul[NUM_OF_ZONES] = {
    5.0f,
    95.0f,
    185.0f,
    275.0f,
    85.0f,
    175.0f,
    265.0f,
    355.0f
};

float magnitude(int x, int y);

float angle(int x, int y);

int8_t zone_detector(float angle);

int joy_mapping_y(int yPos);
int joy_mapping_x(int xPos);


#endif