/*
 * compass_hmc5843.h
 *
 *  Created on: 2017-7-31
 *      Author: wangbo
 */

#ifndef COMPASS_HMC5843_H_
#define COMPASS_HMC5843_H_

#include <math.h>
#include "matrix3.h"

#include "compass.h"

// orientations for DIYDrones magnetometer
#define BIT_COMPASS_COMPONENTS_UP_PINS_FORWARD ROTATION_NONE
#define BIT_COMPASS_COMPONENTS_UP_PINS_FORWARD_RIGHT ROTATION_YAW_45
#define BIT_COMPASS_COMPONENTS_UP_PINS_RIGHT ROTATION_YAW_90
#define BIT_COMPASS_COMPONENTS_UP_PINS_BACK_RIGHT ROTATION_YAW_135
#define BIT_COMPASS_COMPONENTS_UP_PINS_BACK ROTATION_YAW_180
#define BIT_COMPASS_COMPONENTS_UP_PINS_BACK_LEFT ROTATION_YAW_225
#define BIT_COMPASS_COMPONENTS_UP_PINS_LEFT ROTATION_YAW_270
#define BIT_COMPASS_COMPONENTS_UP_PINS_FORWARD_LEFT ROTATION_YAW_315
#define BIT_COMPASS_COMPONENTS_DOWN_PINS_FORWARD ROTATION_ROLL_180
#define BIT_COMPASS_COMPONENTS_DOWN_PINS_FORWARD_RIGHT ROTATION_ROLL_180_YAW_45
#define BIT_COMPASS_COMPONENTS_DOWN_PINS_RIGHT ROTATION_ROLL_180_YAW_90
#define BIT_COMPASS_COMPONENTS_DOWN_PINS_BACK_RIGHT ROTATION_ROLL_180_YAW_135
#define BIT_COMPASS_COMPONENTS_DOWN_PINS_BACK ROTATION_PITCH_180
#define BIT_COMPASS_COMPONENTS_DOWN_PINS_BACK_LEFT ROTATION_ROLL_180_YAW_225
#define BIT_COMPASS_COMPONENTS_DOWN_PINS_LEFT ROTATION_ROLL_180_YAW_270
#define BIT_COMPASS_COMPONENTS_DOWN_PINS_FORWARD_LEFT ROTATION_ROLL_180_YAW_315

// orientations for Sparkfun magnetometer
#define BIT_COMPASS_SPARKFUN_COMPONENTS_UP_PINS_FORWARD ROTATION_YAW_270
#define BIT_COMPASS_SPARKFUN_COMPONENTS_UP_PINS_FORWARD_RIGHT ROTATION_YAW_315
#define BIT_COMPASS_SPARKFUN_COMPONENTS_UP_PINS_RIGHT ROTATION_NONE
#define BIT_COMPASS_SPARKFUN_COMPONENTS_UP_PINS_BACK_RIGHT ROTATION_YAW_45
#define BIT_COMPASS_SPARKFUN_COMPONENTS_UP_PINS_BACK ROTATION_YAW_90
#define BIT_COMPASS_SPARKFUN_COMPONENTS_UP_PINS_BACK_LEFT ROTATION_YAW_135
#define BIT_COMPASS_SPARKFUN_COMPONENTS_UP_PINS_LEFT ROTATION_YAW_180
#define BIT_COMPASS_SPARKFUN_COMPONENTS_UP_PINS_FORWARD_LEFT ROTATION_YAW_225
#define BIT_COMPASS_SPARKFUN_COMPONENTS_DOWN_PINS_FORWARD ROTATION_ROLL_180_YAW_90
#define BIT_COMPASS_SPARKFUN_COMPONENTS_DOWN_PINS_FORWARD_RIGHT ROTATION_ROLL_180_YAW_135
#define BIT_COMPASS_SPARKFUN_COMPONENTS_DOWN_PINS_RIGHT ROTATION_PITCH_180
#define BIT_COMPASS_SPARKFUN_COMPONENTS_DOWN_PINS_BACK_RIGHT ROTATION_ROLL_180_YAW_225
#define BIT_COMPASS_SPARKFUN_COMPONENTS_DOWN_PINS_BACK ROTATION_ROLL_180_YAW_270
#define BIT_COMPASS_SPARKFUN_COMPONENTS_DOWN_PINS_BACK_LEFT ROTATION_ROLL_180_YAW_315
#define BIT_COMPASS_SPARKFUN_COMPONENTS_DOWN_PINS_LEFT ROTATION_ROLL_180
#define BIT_COMPASS_SPARKFUN_COMPONENTS_DOWN_PINS_FORWARD_LEFT ROTATION_ROLL_180_YAW_45

class BIT_Compass_HMC5843 : public Compass
{
  private:
	float calibration[3];
  public:
	BIT_Compass_HMC5843() {}
	virtual bool init();
	virtual void read();
};



#endif /* COMPASS_HMC5843_H_ */
