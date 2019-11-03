


#include "Car_Control.h"


int speed_map(int speed)
{
        return map(speed, MAG_MIN, MAG_MAX, PWM_MIN, PWM_MAX);
}


void drive_car(int speed, int state, Motor  left_motor, Motor right_motor)
{
    switch(state) {
        case RIGHT:
            right(left_motor, right_motor, speed);
            break;
        case FORWARDS:
            forward(left_motor, right_motor, speed);
            break;
        case LEFT:
            left(left_motor, right_motor, speed);
            break;
        case BACKWARDS:
            back(left_motor, right_motor, speed);
            break;
        case RIGHT_FORWARDS:
            left_motor.drive(speed);
            right_motor.drive(speed/2);
            break;
        case LEFT_FORWARDS:
            left_motor.drive(speed/2);
            right_motor.drive(speed);
            break;
        case LEFT_BACKWARDS:
            left_motor.drive(-speed/2);
            right_motor.drive(-speed);
            break;
        case RIGHT_BACKWARDS:
            left_motor.drive(-speed);
            right_motor.drive(-speed/2);
            break;
        default:
            brake(left_motor, right_motor);
        break;
    }
}





void print_driving_state(int8_t state)
{
    switch(state) {
        case RIGHT:
            Serial.println("RIGHT");
            break;
        case FORWARDS:
            Serial.println("FORWARDS");
            break;
        case LEFT:
            Serial.println("LEFT");
            break;
        case BACKWARDS:
            Serial.println("BACKWARDS");
            break;
        case RIGHT_FORWARDS:
            Serial.println("RIGHT_FORWARDS");
            break;
        case LEFT_FORWARDS:
            Serial.println("LEFT_FORWARDS");
            break;
        case LEFT_BACKWARDS:
            Serial.println("LEFT_BACKWARDS");
            break;
        case RIGHT_BACKWARDS:
            Serial.println("RIGHT_BACKWARDS");
            break;
        default:
            Serial.println("Stopped");
        break;
    }    
}