/*
 * imu.cpp
 *
 *  Created on: 2017-8-7
 *      Author: wangbo
 */




#include "imu.h"

//添加动力模型
#include "fdm.h"
#include "BIT_MATH.h"

#include "global.h"
#include "all_external_device.h"

// XXX secret knowledge about the APM/oilpan wiring
//
#define A_LED_PIN   37
#define C_LED_PIN   35

// Sensors: GYROX, GYROY, GYROZ, ACCELX, ACCELY, ACCELZ
const uint8_t IMU::_sensors[6]        = { 1, 2, 0, 4, 5, 6};	// Channel assignments on the APM oilpan
const int8_t  IMU::_sensor_signs[6]	= {	1,-1,-1, 1,-1,-1};  // Channel orientation vs. normal

// Temp compensation curve constants
// These must be produced by measuring data and curve fitting
// [X/Y/Z gyro][A/B/C or 0 order/1st order/2nd order constants]
//
const float   IMU::_gyro_temp_curve[3][3] = {
	{1658,0,0},			// Values to use if no temp compensation data available
	{1658,0,0},			// Based on average values for 20 sample boards
	{1658,0,0}
};

IMU::IMU()
{


}
 const float     IMU:: _gravity = 423.8;       ///< 1G in the raw data coming from the accelerometer
														// Value based on actual sample data from 20 boards
 const float      IMU::_accel_scale = 9.80665 / 423.8; ///< would like to use _gravity here, but cannot

// IDG500 Sensitivity (from datasheet) => 2.0mV/degree/s, 0.8mV/ADC step => 0.8/3.33 .4
// Tested values : 0.4026, ?, 0.4192
//
 const float      IMU::_gyro_gain_x = 0.4;     // X axis Gyro gain
 const float      IMU::_gyro_gain_y = 0.41;    // Y axis Gyro gain
 const float      IMU::_gyro_gain_z = 0.41;    // Z axis Gyro gain

// Maximum possible value returned by an offset-corrected sensor channel
//
 const float     IMU:: _adc_constraint = 900;

// Gyro and Accelerometer calibration criterial
//
 const float		IMU::_gyro_total_cal_change = 4.0;		// Experimentally derived - allows for some minor motion
 const float		IMU::_gyro_max_cal_offset = 320.0;
 const float		IMU::_accel_total_cal_change = 4.0;
 const float		IMU::_accel_max_cal_offset = 250.0;


void
IMU::init()
{
}

/**************************************************/

void
IMU::init_gyro()
{
    _init_gyro();

}

void
IMU::_init_gyro()
{
	int flashcount = 0;
	int tc_temp;
	float adc_in;
	float prev[3] = {0,0,0};
	float total_change;
	float max_offset;

}

void
IMU::init_accel()
{
    _init_accel();
}

void
IMU::_init_accel()
{
	int flashcount = 0;
	float adc_in;
	float prev[6] = {0,0,0};
	float total_change;
	float max_offset;


}

/**************************************************/
// Returns the temperature compensated raw gyro value
//---------------------------------------------------

float
IMU::_sensor_compensation(uint8_t channel, int temperature) const
{
    // do gyro temperature compensation
    if (channel < 3) {

        return  _gyro_temp_curve[channel][0] +
                _gyro_temp_curve[channel][1] * temperature +
                _gyro_temp_curve[channel][2] * temperature * temperature;
    }

    // do fixed-offset accelerometer compensation
    return 2041;    // Average raw value from a 20 board sample
}

float
IMU::_sensor_in(uint8_t channel, int temperature)
{
    float   adc_in;

    /*
     * 这里是写从传感器设备把数据读入到adc_in中
     */
    return adc_in;
}


bool
IMU::update(void)
{
	int tc_temp =0;
	//int tc_temp = _adc->Ch(_gyro_temp_ch);
	//上面这句话是读取imu的数据，然后下面赋值
#if 0
	// convert corrected gyro readings to delta acceleration
	//
	_gyro.x = ToRad(_gyro_gain_x) * _sensor_in(0, tc_temp);
	_gyro.y = ToRad(_gyro_gain_y) * _sensor_in(1, tc_temp);
	_gyro.z = ToRad(_gyro_gain_z) * _sensor_in(2, tc_temp);

	// convert corrected accelerometer readings to acceleration
	//
	_accel.x = _accel_scale * _sensor_in(3, tc_temp);
	_accel.y = _accel_scale * _sensor_in(4, tc_temp);
	_accel.z = _accel_scale * _sensor_in(5, tc_temp);


	_gyro.x = ToRad(0.4) ;
	_gyro.y = ToRad(0.4);
	_gyro.z = ToRad(0.4 );

	// convert corrected accelerometer readings to acceleration
	//
//	_accel.x = _accel_scale * 423.0 ;
//	_accel.y = _accel_scale *423.0 ;
	_accel.x = 0.0 ;
	_accel.y = 0.0 ;
	_accel.z = _accel_scale * 423.0 ;
#endif
	/*
	 * 总之，这个函数是必须不断获取acc gyro的数据，把数据给到_gyro，_accel这个三维向量里
	 * 然后才能把数据给到ahrs
	 * 然后更新dcm矩阵
	 */

#if 0
	_gyro.x = fdm_feed_back.phidot;
	_gyro.y = fdm_feed_back.thetadot;
	_gyro.z = fdm_feed_back.psidot;

	_accel.x = fdm_feed_back.A_X_pilot;
	_accel.y = fdm_feed_back.A_Y_pilot;
	_accel.z = fdm_feed_back.A_Z_pilot;
#else
	/*
	 * 上面的其实不需要了
	 * 硬件驱动把更新的数据赋值给all_external_device_input
	 * 我的飞控软件自成体系，从all_external_device_input获取数据就可以了
	 */

	_gyro.x = all_external_device_input._gyro_x;
	_gyro.y = all_external_device_input._gyro_y;
	_gyro.z = all_external_device_input._gyro_z;

	_accel.x = all_external_device_input._accel_x;
	_accel.y = all_external_device_input._accel_y;
	_accel.z = all_external_device_input._accel_z;
#endif
	// always updated
	return true;
}
