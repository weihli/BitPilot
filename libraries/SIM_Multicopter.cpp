/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
  multicopter simulator class
*/


#include "SIM_Multicopter.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include "BIT_MATH.h"



Motor m(90,  false,  1);

static const Motor quad_plus_motors[4] =
{
    Motor(90,  false,  1),
    Motor(270, false,  2),
    Motor(0,   true,   3),
    Motor(180, true,   4)
};

static const Motor quad_x_motors[4] =
{
    Motor(45,  false,  1),
    Motor(225, false,  2),
    Motor(315, true,   3),
    Motor(135, true,   4)
};

static const Motor hexa_motors[6] =
{
    Motor(60,   false, 1),
    Motor(60,   true,  7),
    Motor(180,  true,  4),
    Motor(180,  false, 8),
    Motor(-60,  true,  2),
    Motor(-60,  false, 3),
};

static const Motor hexax_motors[6] =
{
    Motor(30,  false,  7),
    Motor(90,  true,   1),
    Motor(150, false,  4),
    Motor(210, true,   8),
    Motor(270, false,  2),
    Motor(330, true,   3)
};

static const Motor octa_motors[8] =
{
    Motor(0,    true,  1),
    Motor(180,  true,  2),
    Motor(45,   false, 3),
    Motor(135,  false, 4),
    Motor(-45,  false, 5),
    Motor(-135, false, 6),
    Motor(270,  true,  7),
    Motor(90,   true,  8)
};

static const Motor octa_quad_motors[8] =
{
    Motor(  45, false, 1),
    Motor( -45, true,  2),
    Motor(-135, false, 3),
    Motor( 135, true,  4),
    Motor( -45, false, 5),
    Motor(  45, true,  6),
    Motor( 135, false, 7),
    Motor(-135, true,  8)
};

/*
  table of supported frame types
 */
static const Frame supported_frames[] =
{
    Frame("+",         4, quad_plus_motors),
    Frame("quad",      4, quad_plus_motors),
    Frame("copter",    4, quad_plus_motors),
    Frame("x",         4, quad_x_motors),
    Frame("hexa",      6, hexa_motors),
    Frame("hexax",     6, hexax_motors),
    Frame("octa",      8, octa_motors),
    Frame("octa-quad", 8, octa_quad_motors)
};

/*
  constructor
 */
MultiCopter::MultiCopter(const char *home_str, const char *frame_str) :
    Aircraft(home_str, frame_str),
    frame(NULL),
    hover_throttle(0.51),
    terminal_velocity(15.0),
    terminal_rotation_rate(4*radians(360.0))
{
    for (uint8_t i=0; i < 4; i++) {
        if (strcasecmp(frame_str, supported_frames[i].name) == 0) {
        	//std::cout<<"supported_frames[i].name="<<supported_frames[i].name<<std::endl;//20170818已测试，是+号
            frame = &supported_frames[i];
        }
    }
    if (frame == NULL) {
        printf("Frame '%s' not found", frame_str);
        exit(1);
    }
    //frame->num_motors=4;

    /*
       scaling from total motor power to Newtons. Allows the copter
       to hover against gravity when each motor is at hover_throttle
    */
    mass = 1.5;
    thrust_scale = (mass * GRAVITY_MSS) / (frame->num_motors * hover_throttle);

    frame_height = 0.0;


}

/*
  update the multicopter simulation by one time step
 */
void MultiCopter::update(const struct sitl_input &input)
{
    float motor_speed[frame->num_motors];

    //std::cout<<"num_motors="<<frame->num_motors<<std::endl;

    //for (uint8_t i=0; i<frame->num_motors; i++) {
    for (uint8_t i=0; i<4; i++) {
        uint16_t servo = input.servos[frame->motors[i].servo-1];
        // assume 1000 to 2000 PWM range
        if (servo <= 1000) {
            //motor_speed[i] = 0;
        	motor_speed[i] = 0.0;//本来应该是=0.0的
        } else if(servo >= 2000) {
        	//motor_speed[i] = 1;
        	//motor_speed[i] = 0.9865;
        	motor_speed[i] =1.0;
        }else{
        	motor_speed[i] = (servo-1000) / 1000.0f;
        }
    }

    // how much time has passed?
    float delta_time = frame_time_us * 1.0e-6f;//rate_hz=100，初始化时，所以时间是0.01秒更新频率
   // float delta_time = 1;

//    std::cout<<"delta_time="<<delta_time<<std::endl;

    // rotational acceleration, in rad/s/s, in body frame
    Vector3f rot_accel;
    float thrust = 0.0f; // newtons

    for (uint8_t i=0; i<frame->num_motors; i++) {
    	//std::cout<<"motors_speed[i]="<<motor_speed[i]<<std::endl;
    	//std::cout<<"sinf(radians(frame->motors[i].angle))="<<sinf(radians(frame->motors[i].angle))<<std::endl;//如果是0度的话，发现并不是0，而是一个极小数8.74228e-08
//        rot_accel.x  += -radians(5000.0) * sinf(radians(frame->motors[i].angle)) * motor_speed[i];
//        rot_accel.y  +=  radians(5000.0) * cosf(radians(frame->motors[i].angle)) * motor_speed[i];
        rot_accel.x  += -radians(5000.0) * sin(radians(frame->motors[i].angle)) * motor_speed[i];
         rot_accel.y  +=  radians(5000.0) * cos(radians(frame->motors[i].angle)) * motor_speed[i];

        //std::cout<<"motors_speed[i]="<<motor_speed[i]<<std::endl;//20170818已测试
        if (frame->motors[i].clockwise) {
            rot_accel.z -= motor_speed[i] * radians(400.0);
        } else {
            rot_accel.z += motor_speed[i] * radians(400.0);
        }
        //std::cout<<"thrust_scale="<<thrust_scale<<std::endl;
        thrust += motor_speed[i] * thrust_scale; // newtons
    }

    /*
     * 之所以有下面的限幅，是因为sin(0) 和cos(1)在应该等于0的地方是个极小值，flightgear已经不能识别了
     */
    if(rot_accel.x>-1e-5  && rot_accel.x<1e-5)
    {
    	rot_accel.x=0.0;
    }

    if(rot_accel.y>-1e-5  && rot_accel.y<1e-5)
    {
    	rot_accel.y=0.0;
    }

    if(rot_accel.z>-1e-5  && rot_accel.z<1e-5)
    {
    	rot_accel.z=0.0;
    }
//
//    std::cout<<"rot_accel.x="<<rot_accel.x<<std::endl;
//    std::cout<<"rot_accel.y="<<rot_accel.y<<std::endl;
//    std::cout<<"rot_accel.z="<<rot_accel.z<<std::endl;

//
//    // rotational air resistance
//    rot_accel.x -= gyro.x * radians(5000.0) / terminal_rotation_rate;
//    rot_accel.y -= gyro.y * radians(5000.0) / terminal_rotation_rate;
//    rot_accel.z -= gyro.z * radians(400.0)  / terminal_rotation_rate;

    // rotational air resistance
    rot_accel.x -= gyro.x * radians(5000.0) / terminal_rotation_rate;
    rot_accel.y -= gyro.y * radians(5000.0) / terminal_rotation_rate;
    rot_accel.z -= gyro.z * radians(400.0)  / terminal_rotation_rate;
//    std::cout<<"terminal_rotation_rate="<<terminal_rotation_rate<<std::endl;
//
//    std::cout<<"rot_accel.x="<<rot_accel.x<<std::endl;
//    std::cout<<"rot_accel.y="<<rot_accel.y<<std::endl;
//    std::cout<<"rot_accel.z="<<rot_accel.z<<std::endl;

    // update rotational rates in body frame
    gyro += rot_accel * delta_time;//这个delta_time就显得很重要了，初始化是0.01秒，应该是更新的速率

//    std::cout<<"20170818 gyro.x="<<gyro.x<<std::endl;
//     std::cout<<"20170818 gyro.y="<<gyro.y<<std::endl;
//     std::cout<<"20170818 gyro.z="<<gyro.z<<std::endl;


    // update attitude
    dcm.rotate(gyro * delta_time);//这个rotate函数其实就是旋转矩阵的一阶增量算法，cnb(t+1)=cnb(t)+cnb(t)*S(gyro)，这个S函数是叉乘向量的意思，20170818经过公式推导没有问题
//    std::cout<<"fdm dcm rotation="<<dcm.c.x<<std::endl;


    dcm.normalize();

//    std::cout<<"fdm  0818  dcm.c.x="<<dcm.c.x<<std::endl;

    // air resistance
    //Vector3f air_resistance = -velocity_ef * (GRAVITY_MSS/terminal_velocity);
    Vector3f air_resistance ;
    air_resistance.x=0;
    air_resistance.y=0;
    air_resistance.z=0;

    accel_body = Vector3f(0, 0, -thrust / mass);
    Vector3f accel_earth = dcm * accel_body;//得到参考坐标系下的加速度
//    std::cout<<"accel_earth.x="<<accel_earth.x<<std::endl;
//	std::cout<<"accel_earth.y="<<accel_earth.y<<std::endl;
//	std::cout<<"accel_earth.z="<<accel_earth.z<<std::endl;



    accel_earth += Vector3f(0, 0, GRAVITY_MSS);
//    accel_earth += air_resistance;
//    std::cout<<"air_resistance.x="<<air_resistance.x<<std::endl;
//    std::cout<<"air_resistance.y="<<air_resistance.y<<std::endl;
//    std::cout<<"air_resistance.z="<<air_resistance.z<<std::endl;
//
//
//    std::cout<<"air_resistance.z="<<air_resistance.z<<std::endl;
    // if we're on the ground, then our vertical acceleration is limited
    // to zero. This effectively adds the force of the ground on the aircraft

//    std::cout<<"position.z="<<position.z<<std::endl;
//    std::cout<<"home.alt="<<home.alt<<std::endl;
    if (on_ground(position) && accel_earth.z > 0) {
//    	std::cout<<"0818 on ground "<<std::endl;
        accel_earth.z = 0;
    }

    // work out acceleration as seen by the accelerometers. It sees the kinematic
    // acceleration (ie. real movement), plus gravity
    accel_body = dcm.transposed() * (accel_earth + Vector3f(0, 0, -GRAVITY_MSS));

//	std::cout<<"thrust_scale="<<thrust_scale<<std::endl;
	//std::cout<<"frame->num_motors="<<frame->num_motors<<std::endl;
    // add some noise
    add_noise(thrust / (thrust_scale * frame->num_motors));

    // new velocity vector
    velocity_ef += accel_earth * delta_time;

    //velocity_air_bf = dcm.transposed() * velocity_ef;//20170817增加机体的速度
    velocity_air_bf += accel_body* delta_time;//20170818增加机体的速度

    // new position vector
    Vector3f old_position = position;
    position += velocity_ef * delta_time;



    // assume zero wind for now
    airspeed = velocity_ef.length();

    // constrain height to the ground
    if (on_ground(position)) {

    	//std::cout<<"莫不是进了on ground"<<std::endl;

        if (!on_ground(old_position)) {
            printf("Hit ground at %f m/s\n", velocity_ef.z);

            //velocity_ef.zero();

            // zero roll/pitch, but keep yaw
            float r, p, y;
            dcm.to_euler(&r, &p, &y);
            dcm.from_euler(0, 0, y);

            position.z = -(ground_level + frame_height - home.alt*0.01f);
        }
    }

    // update lat/lon/altitude
    update_position();
}

