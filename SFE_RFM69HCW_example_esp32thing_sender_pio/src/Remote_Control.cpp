

#include "Remote_Control.h"


/**
 * @brief Computes the magnitude of a 2 dimensional vector
 *  |v| = sqrt ( x^2 + y^2 )
 * @param[in] x 
 * @param[in] y 
 * @return float 
 */
float magnitude(int x, int y)
{
    float xf = (float) x;
    float yf = (float) y;

    float mag = sqrtf( (xf * xf) + (yf * yf) );
    return mag;
}


/**
 * @brief Compute the angle of a 2 dimensional vector
 * range 0:360
 * @param[in] x 
 * @param[in] y 
 * @return float 
 */
float angle(int x, int y)
{
    float xf = (float) x;
    float yf = (float) y;

    float ang = degrees( atan2f(yf,xf) ); // angle in degrees

    if (ang >= 0)
    {
		ang = ang;
    } 
    else 
    {
		ang += 360.0;
    }
    return ang;
}

/**
 * @brief Determine the appropriate zone from
 *  a given angle
 *  There are 8 zones a joystick can be in.
 * 
 * @param[in] angle 
 * @return int8_t 
 */
int8_t zone_detector(float angle) 
{
    if      (zones_ll[z1] < angle && angle < zones_ul[z1])
        return 1;
    else if (zones_ll[z2] < angle && angle < zones_ul[z2])
        return 2;
    else if (zones_ll[z3] < angle && angle < zones_ul[z3])
        return 3;
    else if (zones_ll[z4] < angle && angle < zones_ul[z4])
        return 4;
    else if (zones_ll[z5] < angle && angle < zones_ul[z5])
        return 5;
    else if (zones_ll[z6] < angle && angle < zones_ul[z6])
        return 6;
    else if (zones_ll[z7] < angle && angle < zones_ul[z7])
        return 7;
    else if (zones_ll[z8] < angle && angle < zones_ul[z8])
        return 8;
    else 
        return 0;
}



int joy_mapping_y(int yPos) {
  if (yPos >= 0) return map(yPos, 0, Y_POS_MAX, 0, SPEED_MAX_FORWARD);
  else return map(yPos, Y_NEG_MIN, 0, SPEED_MAX_REVERSE, -1);
}

int joy_mapping_x(int xPos) {
  if (xPos >= 10) return map(xPos, 0, X_POS_MAX, 0, SPEED_MAX_FORWARD);
  else return map(xPos, X_NEG_MIN, 0, SPEED_MAX_REVERSE, -1);
}