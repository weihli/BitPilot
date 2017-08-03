/*
 * radio.cpp
 *
 *  Created on: 2017-8-1
 *      Author: wangbo
 */

#include <inttypes.h>

int8_t failsafeCounter = 0;		// we wait a second to take over the throttle and send the plane circling

#include "copter.h"

void Copter::init_rc_in()
{
#if 0
	// set rc reversing
	update_servo_switches();
	g.channel_roll.set_reverse(reverse_roll);
	g.channel_pitch.set_reverse(reverse_pitch);
	g.channel_rudder.set_reverse(reverse_rudder);

	// set rc channel ranges
	g.channel_roll.set_angle(ROLL_SERVO_MAX);
	g.channel_pitch.set_angle(PITCH_SERVO_MAX);
	g.channel_rudder.set_angle(RUDDER_SERVO_MAX);
	g.channel_throttle.set_range(0, 100);

	// set rc dead zones
	g.channel_roll.dead_zone 	= 60;
	g.channel_pitch.dead_zone 	= 60;
	g.channel_rudder.dead_zone 	= 60;
	g.channel_throttle.dead_zone = 6;

	//set auxiliary ranges
	g.rc_5.set_range(0,1000);
	g.rc_6.set_range(0,1000);
	g.rc_7.set_range(0,1000);
	g.rc_8.set_range(0,1000);
#endif
}

void Copter::init_rc_out()
{
#if 0

	AP_RC.OutputCh(CH_1, 	g.channel_roll.radio_trim);					// Initialization of servo outputs
	AP_RC.OutputCh(CH_2, 	g.channel_pitch.radio_trim);
	AP_RC.OutputCh(CH_3, 	g.channel_throttle.radio_min);
	AP_RC.OutputCh(CH_4, 	g.channel_rudder.radio_trim);

	AP_RC.OutputCh(CH_5, 	g.rc_5.radio_trim);
	AP_RC.OutputCh(CH_6, 	g.rc_6.radio_trim);
	AP_RC.OutputCh(CH_7,   g.rc_7.radio_trim);
    AP_RC.OutputCh(CH_8,   g.rc_8.radio_trim);

	AP_RC.Init();		// APM Radio initialization

	AP_RC.OutputCh(CH_1, 	g.channel_roll.radio_trim);					// Initialization of servo outputs
	APM_RC.OutputCh(CH_2, 	g.channel_pitch.radio_trim);
	APM_RC.OutputCh(CH_3, 	g.channel_throttle.radio_min);
	APM_RC.OutputCh(CH_4, 	g.channel_rudder.radio_trim);

	APM_RC.OutputCh(CH_5, 	g.rc_5.radio_trim);
	APM_RC.OutputCh(CH_6, 	g.rc_6.radio_trim);
	APM_RC.OutputCh(CH_7,   g.rc_7.radio_trim);
    APM_RC.OutputCh(CH_8,   g.rc_8.radio_trim);
#endif
}

void read_radio()
{
#if 0
	ch1_temp = APM_RC.InputCh(CH_ROLL);
	ch2_temp = APM_RC.InputCh(CH_PITCH);

	if(mix_mode == 0){
		g.channel_roll.set_pwm(ch1_temp);
		g.channel_pitch.set_pwm(ch2_temp);
	}else{
		g.channel_roll.set_pwm(reverse_elevons * (reverse_ch2_elevon * int(ch2_temp - elevon2_trim) - reverse_ch1_elevon * int(ch1_temp - elevon1_trim)) / 2 + 1500);
		g.channel_pitch.set_pwm((reverse_ch2_elevon * int(ch2_temp - elevon2_trim) + reverse_ch1_elevon * int(ch1_temp - elevon1_trim)) / 2 + 1500);
	}

	g.channel_throttle.set_pwm(APM_RC.InputCh(CH_3));
	g.channel_rudder.set_pwm(APM_RC.InputCh(CH_4));
	g.rc_5.set_pwm(APM_RC.InputCh(CH_5));
	g.rc_6.set_pwm(APM_RC.InputCh(CH_6));
	g.rc_7.set_pwm(APM_RC.InputCh(CH_7));
	g.rc_8.set_pwm(APM_RC.InputCh(CH_8));

	//  TO DO  - go through and patch throttle reverse for RC_Channel library compatibility
	#if THROTTLE_REVERSE == 1
		radio_in[CH_THROTTLE] = radio_max(CH_THROTTLE) + radio_min(CH_THROTTLE) - radio_in[CH_THROTTLE];
	#endif

	throttle_failsafe(g.channel_throttle.radio_in);

	g.channel_throttle.servo_out = g.channel_throttle.control_in;

	if (g.channel_throttle.servo_out > 50) {
		if(airspeed_enabled == true) {
			airspeed_nudge = (g.flybywire_airspeed_max * 100 - g.airspeed_cruise) * ((g.channel_throttle.norm_input()-0.5) / 0.5);
        } else {
			throttle_nudge = (g.throttle_max - g.throttle_cruise) * ((g.channel_throttle.norm_input()-0.5) / 0.5);
		}
	} else {
		airspeed_nudge = 0;
		throttle_nudge = 0;
	}

	/*
	Serial.printf_P(PSTR("OUT 1: %d\t2: %d\t3: %d\t4: %d \n"),
				g.rc_1.control_in,
				g.rc_2.control_in,
				g.rc_3.control_in,
				g.rc_4.control_in);
	*/
#endif
}

void throttle_failsafe(uint16_t pwm)
{
#if 0
	if(g.throttle_fs_enabled == 0)
		return;

	//check for failsafe and debounce funky reads
	// ------------------------------------------
	if (pwm < g.throttle_fs_value){
		// we detect a failsafe from radio
		// throttle has dropped below the mark
		failsafeCounter++;
		if (failsafeCounter == 9){
			SendDebug_P("MSG FS ON ");
			SendDebugln(pwm, DEC);
		}else if(failsafeCounter == 10) {
			ch3_failsafe = true;
			//set_failsafe(true);
			//failsafeCounter = 10;
		}else if (failsafeCounter > 10){
			failsafeCounter = 11;
		}

	}else if(failsafeCounter > 0){
		// we are no longer in failsafe condition
		// but we need to recover quickly
		failsafeCounter--;
		if (failsafeCounter > 3){
			failsafeCounter = 3;
		}
		if (failsafeCounter == 1){
			SendDebug_P("MSG FS OFF ");
			SendDebugln(pwm, DEC);
		}else if(failsafeCounter == 0) {
			ch3_failsafe = false;
			//set_failsafe(false);
			//failsafeCounter = -1;
		}else if (failsafeCounter <0){
			failsafeCounter = -1;
		}
	}
#endif
}

void trim_control_surfaces()
{
#if 0
	read_radio();
	// Store control surface trim values
	// ---------------------------------
	if(mix_mode == 0){
		g.channel_roll.radio_trim = g.channel_roll.radio_in;
		g.channel_pitch.radio_trim = g.channel_pitch.radio_in;
		g.channel_rudder.radio_trim = g.channel_rudder.radio_in;
	}else{
		elevon1_trim = ch1_temp;
		elevon2_trim = ch2_temp;
		//Recompute values here using new values for elevon1_trim and elevon2_trim
		//We cannot use radio_in[CH_ROLL] and radio_in[CH_PITCH] values from read_radio() because the elevon trim values have changed
		uint16_t center 			= 1500;
		g.channel_roll.radio_trim 	= center;
		g.channel_pitch.radio_trim 	= center;
	}

	// save to eeprom
	g.channel_roll.save_eeprom();
	g.channel_pitch.save_eeprom();
	g.channel_throttle.save_eeprom();
	g.channel_rudder.save_eeprom();
#endif
}

void trim_radio()
{
#if 0
	for (int y = 0; y < 30; y++) {
		read_radio();
	}

	// Store the trim values
	// ---------------------
	if(mix_mode == 0){
		g.channel_roll.radio_trim 		= g.channel_roll.radio_in;
		g.channel_pitch.radio_trim 		= g.channel_pitch.radio_in;
		//g.channel_throttle.radio_trim 	= g.channel_throttle.radio_in;
		g.channel_rudder.radio_trim 	= g.channel_rudder.radio_in;

	} else {
		elevon1_trim = ch1_temp;
		elevon2_trim = ch2_temp;
		uint16_t center = 1500;
		g.channel_roll.radio_trim 	= center;
		g.channel_pitch.radio_trim 	= center;
		g.channel_rudder.radio_trim = g.channel_rudder.radio_in;
	}

	// save to eeprom
	g.channel_roll.save_eeprom();
	g.channel_pitch.save_eeprom();
	//g.channel_throttle.save_eeprom();
	g.channel_rudder.save_eeprom();
#endif
}