/*
 * GCS_Mavlink.cpp
 *
 *  Created on: 2017-9-18
 *      Author: wangbo
 */

#include "copter.h"

#ifdef LINUX_OS
// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

// default sensors are present and healthy: gyro, accelerometer, barometer, rate_control, attitude_stabilization, yaw_position, altitude control, x/y position control, motor_control
#define MAVLINK_SENSOR_PRESENT_DEFAULT (MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL | MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE | MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL | MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION | MAV_SYS_STATUS_SENSOR_YAW_POSITION | MAV_SYS_STATUS_SENSOR_Z_ALTITUDE_CONTROL | MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL | MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS | MAV_SYS_STATUS_AHRS)

//20170930我这里面的函数都是从apm3.3复制粘贴过来的，但是我又要参照apm2.6的一些函数来写


/*
 *  send a message on both GCS links
 */
void Copter::gcs_send_message(enum ap_message id)
{
	gcs.send_message(id);
//
//    for (uint8_t i=0; i<num_gcs; i++) {
//        if (gcs[i].initialised) {
//            gcs[i].send_message(id);
//        }
//    }
}

void Copter::gcs_send_heartbeat(void)
{
    gcs_send_message(MSG_HEARTBEAT);
}

void Copter::gcs_send_deferred(void)
{
    gcs_send_message(MSG_RETRY_DEFERRED);
}

/*
 *  !!NOTE!!
 *
 *  the use of NOINLINE separate functions for each message type avoids
 *  a compiler bug in gcc that would cause it to use far more stack
 *  space than is needed. Without the NOINLINE we use the sum of the
 *  stack needed for each message type. Please be careful to follow the
 *  pattern below when adding any new messages
 */

NOINLINE void Copter::send_heartbeat(mavlink_channel_t chan)
{
#if 0
    uint8_t base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
    uint8_t system_status = ap.land_complete ? MAV_STATE_STANDBY : MAV_STATE_ACTIVE;
    uint32_t custom_mode = control_mode;

    // set system as critical if any failsafe have triggered
    if (failsafe.radio || failsafe.battery || failsafe.gcs || failsafe.ekf)  {
        system_status = MAV_STATE_CRITICAL;
    }

    // work out the base_mode. This value is not very useful
    // for APM, but we calculate it as best we can so a generic
    // MAVLink enabled ground station can work out something about
    // what the MAV is up to. The actual bit values are highly
    // ambiguous for most of the APM flight modes. In practice, you
    // only get useful information from the custom_mode, which maps to
    // the APM flight mode and has a well defined meaning in the
    // ArduPlane documentation
    base_mode = MAV_MODE_FLAG_STABILIZE_ENABLED;
    switch (control_mode) {
    case AUTO:
    case RTL:
    case LOITER:
    case GUIDED:
    case CIRCLE:
    case POSHOLD:
    case BRAKE:
        base_mode |= MAV_MODE_FLAG_GUIDED_ENABLED;
        // note that MAV_MODE_FLAG_AUTO_ENABLED does not match what
        // APM does in any mode, as that is defined as "system finds its own goal
        // positions", which APM does not currently do
        break;
    }

    // all modes except INITIALISING have some form of manual
    // override if stick mixing is enabled
    base_mode |= MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;

#if HIL_MODE != HIL_MODE_DISABLED
    base_mode |= MAV_MODE_FLAG_HIL_ENABLED;
#endif

    // we are armed if we are not initialising
    if (motors.armed()) {
        base_mode |= MAV_MODE_FLAG_SAFETY_ARMED;
    }

    // indicate we have set a custom mode
    base_mode |= MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

    mavlink_msg_heartbeat_send(
        chan,
#if (FRAME_CONFIG == QUAD_FRAME)
        MAV_TYPE_QUADROTOR,
#elif (FRAME_CONFIG == TRI_FRAME)
        MAV_TYPE_TRICOPTER,
#elif (FRAME_CONFIG == HEXA_FRAME || FRAME_CONFIG == Y6_FRAME)
        MAV_TYPE_HEXAROTOR,
#elif (FRAME_CONFIG == OCTA_FRAME || FRAME_CONFIG == OCTA_QUAD_FRAME)
        MAV_TYPE_OCTOROTOR,
#elif (FRAME_CONFIG == HELI_FRAME)
        MAV_TYPE_HELICOPTER,
#elif (FRAME_CONFIG == SINGLE_FRAME)  //because mavlink did not define a singlecopter, we use a rocket
        MAV_TYPE_ROCKET,
#elif (FRAME_CONFIG == COAX_FRAME)  //because mavlink did not define a singlecopter, we use a rocket
        MAV_TYPE_ROCKET,
#else
  #error Unrecognised frame type
#endif
        MAV_AUTOPILOT_ARDUPILOTMEGA,
        base_mode,
        custom_mode,
        system_status);
#endif
}

NOINLINE void Copter::send_attitude(mavlink_channel_t chan)
{
#if 0
    const Vector3f &gyro = ins.get_gyro();
    mavlink_msg_attitude_send(
        chan,
        millis(),
        ahrs.roll,
        ahrs.pitch,
        ahrs.yaw,
        gyro.x,
        gyro.y,
        gyro.z);
#endif
}

//#if AC_FENCE == ENABLED
//NOINLINE void Copter::send_limits_status(mavlink_channel_t chan)
//{
//    fence_send_mavlink_status(chan);
//}
//#endif


NOINLINE void Copter::send_extended_status1(mavlink_channel_t chan)
{


}

void NOINLINE Copter::send_location(mavlink_channel_t chan)
{

}

void NOINLINE Copter::send_nav_controller_output(mavlink_channel_t chan)
{

}

// report simulator state
void NOINLINE Copter::send_simstate(mavlink_channel_t chan)
{

}

void NOINLINE Copter::send_hwstatus(mavlink_channel_t chan)
{

}

void NOINLINE Copter::send_servo_out(mavlink_channel_t chan)
{
#if HIL_MODE != HIL_MODE_DISABLED
    // normalized values scaled to -10000 to 10000
    // This is used for HIL.  Do not change without discussing with HIL maintainers

#if FRAME_CONFIG == HELI_FRAME
    mavlink_msg_rc_channels_scaled_send(
        chan,
        millis(),
        0, // port 0
        g.rc_1.servo_out,
        g.rc_2.servo_out,
        g.rc_3.radio_out,
        g.rc_4.servo_out,
        0,
        0,
        0,
        0,
        receiver_rssi);
#else
    mavlink_msg_rc_channels_scaled_send(
        chan,
        millis(),
        0,         // port 0
        g.rc_1.servo_out,
        g.rc_2.servo_out,
        g.rc_3.radio_out,
        g.rc_4.servo_out,
        10000 * g.rc_1.norm_output(),
        10000 * g.rc_2.norm_output(),
        10000 * g.rc_3.norm_output(),
        10000 * g.rc_4.norm_output(),
        receiver_rssi);
#endif
#endif // HIL_MODE
}

void NOINLINE Copter::send_radio_out(mavlink_channel_t chan)
{

}

void NOINLINE Copter::send_vfr_hud(mavlink_channel_t chan)
{

}

void NOINLINE Copter::send_current_waypoint(mavlink_channel_t chan)
{

}

/*
  send RPM packet
 */
void NOINLINE Copter::send_rpm(mavlink_channel_t chan)
{

}


/*
  send PID tuning message
 */
void Copter::send_pid_tuning(mavlink_channel_t chan)
{

}


void NOINLINE Copter::send_statustext(mavlink_channel_t chan)
{

}

// are we still delaying telemetry to try to avoid Xbee bricking?
//bool Copter::telemetry_delayed(mavlink_channel_t chan)
//{
//
//}


// try to send a message, return false if it won't fit in the serial tx buffer
bool GCS_MAVLINK::try_send_message(enum ap_message id)
{
//    if (copter.telemetry_delayed(chan)) {
//        return false;
//    }

#if HIL_MODE != HIL_MODE_SENSORS
    // if we don't have at least 250 micros remaining before the main loop
    // wants to fire then don't send a mavlink message. We want to
    // prioritise the main flight control loop over communications
    if (copter.scheduler.time_available_usec() < 250 && copter.motors.armed()) {
        copter.gcs_out_of_time = true;
        return false;
    }
#endif

//    switch(id) {
//    case MSG_HEARTBEAT:
//        CHECK_PAYLOAD_SIZE(HEARTBEAT);
//        copter.gcs[chan-MAVLINK_COMM_0].last_heartbeat_time = hal.scheduler->millis();
//        copter.send_heartbeat(chan);
//        break;
//
//    case MSG_EXTENDED_STATUS1:
//        // send extended status only once vehicle has been initialised
//        // to avoid unnecessary errors being reported to user
//        if (copter.ap.initialised) {
//            CHECK_PAYLOAD_SIZE(SYS_STATUS);
//            copter.send_extended_status1(chan);
//            CHECK_PAYLOAD_SIZE(POWER_STATUS);
//            copter.gcs[chan-MAVLINK_COMM_0].send_power_status();
//        }
//        break;
//
//    case MSG_EXTENDED_STATUS2:
//        CHECK_PAYLOAD_SIZE(MEMINFO);
//        copter.gcs[chan-MAVLINK_COMM_0].send_meminfo();
//        break;
//
//    case MSG_ATTITUDE:
//        CHECK_PAYLOAD_SIZE(ATTITUDE);
//        copter.send_attitude(chan);
//        break;
//
//    case MSG_LOCATION:
//        CHECK_PAYLOAD_SIZE(GLOBAL_POSITION_INT);
//        copter.send_location(chan);
//        break;
//
//    case MSG_LOCAL_POSITION:
//        CHECK_PAYLOAD_SIZE(LOCAL_POSITION_NED);
//        send_local_position(copter.ahrs);
//        break;
//
//    case MSG_NAV_CONTROLLER_OUTPUT:
//        CHECK_PAYLOAD_SIZE(NAV_CONTROLLER_OUTPUT);
//        copter.send_nav_controller_output(chan);
//        break;
//
//    case MSG_GPS_RAW:
//        return copter.gcs[chan-MAVLINK_COMM_0].send_gps_raw(copter.gps);
//
//    case MSG_SYSTEM_TIME:
//        CHECK_PAYLOAD_SIZE(SYSTEM_TIME);
//        copter.gcs[chan-MAVLINK_COMM_0].send_system_time(copter.gps);
//        break;
//
//    case MSG_SERVO_OUT:
//        CHECK_PAYLOAD_SIZE(RC_CHANNELS_SCALED);
//        copter.send_servo_out(chan);
//        break;
//
//    case MSG_RADIO_IN:
//        CHECK_PAYLOAD_SIZE(RC_CHANNELS_RAW);
//        copter.gcs[chan-MAVLINK_COMM_0].send_radio_in(copter.receiver_rssi);
//        break;
//
//    case MSG_RADIO_OUT:
//        CHECK_PAYLOAD_SIZE(SERVO_OUTPUT_RAW);
//        copter.send_radio_out(chan);
//        break;
//
//    case MSG_VFR_HUD:
//        CHECK_PAYLOAD_SIZE(VFR_HUD);
//        copter.send_vfr_hud(chan);
//        break;
//
//    case MSG_RAW_IMU1:
//        CHECK_PAYLOAD_SIZE(RAW_IMU);
//        copter.gcs[chan-MAVLINK_COMM_0].send_raw_imu(copter.ins, copter.compass);
//        break;
//
//    case MSG_RAW_IMU2:
//        CHECK_PAYLOAD_SIZE(SCALED_PRESSURE);
//        copter.gcs[chan-MAVLINK_COMM_0].send_scaled_pressure(copter.barometer);
//        break;
//
//    case MSG_RAW_IMU3:
//        CHECK_PAYLOAD_SIZE(SENSOR_OFFSETS);
//        copter.gcs[chan-MAVLINK_COMM_0].send_sensor_offsets(copter.ins, copter.compass, copter.barometer);
//        break;
//
//    case MSG_CURRENT_WAYPOINT:
//        CHECK_PAYLOAD_SIZE(MISSION_CURRENT);
//        copter.send_current_waypoint(chan);
//        break;
//
//    case MSG_NEXT_PARAM:
//        CHECK_PAYLOAD_SIZE(PARAM_VALUE);
//        copter.gcs[chan-MAVLINK_COMM_0].queued_param_send();
//        break;
//
//    case MSG_NEXT_WAYPOINT:
//        CHECK_PAYLOAD_SIZE(MISSION_REQUEST);
//        copter.gcs[chan-MAVLINK_COMM_0].queued_waypoint_send();
//        break;
//
//    case MSG_RANGEFINDER:
//#if CONFIG_SONAR == ENABLED
//        CHECK_PAYLOAD_SIZE(RANGEFINDER);
//        copter.send_rangefinder(chan);
//#endif
//        break;
//
//    case MSG_RPM:
//        CHECK_PAYLOAD_SIZE(RPM);
//        copter.send_rpm(chan);
//        break;
//
//    case MSG_TERRAIN:
//#if AP_TERRAIN_AVAILABLE
//        CHECK_PAYLOAD_SIZE(TERRAIN_REQUEST);
//        copter.terrain.send_request(chan);
//#endif
//        break;
//
//    case MSG_CAMERA_FEEDBACK:
//#if CAMERA == ENABLED
//        CHECK_PAYLOAD_SIZE(CAMERA_FEEDBACK);
//        copter.camera.send_feedback(chan, copter.gps, copter.ahrs, copter.current_loc);
//#endif
//        break;
//
//    case MSG_STATUSTEXT:
//        CHECK_PAYLOAD_SIZE(STATUSTEXT);
//        copter.send_statustext(chan);
//        break;
//
//    case MSG_LIMITS_STATUS:
//#if AC_FENCE == ENABLED
//        CHECK_PAYLOAD_SIZE(LIMITS_STATUS);
//        copter.send_limits_status(chan);
//#endif
//        break;
//
//    case MSG_AHRS:
//        CHECK_PAYLOAD_SIZE(AHRS);
//        copter.gcs[chan-MAVLINK_COMM_0].send_ahrs(copter.ahrs);
//        break;
//
//    case MSG_SIMSTATE:
//#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
//        CHECK_PAYLOAD_SIZE(SIMSTATE);
//        copter.send_simstate(chan);
//#endif
//        CHECK_PAYLOAD_SIZE(AHRS2);
//        copter.gcs[chan-MAVLINK_COMM_0].send_ahrs2(copter.ahrs);
//        break;
//
//    case MSG_HWSTATUS:
//        CHECK_PAYLOAD_SIZE(HWSTATUS);
//        copter.send_hwstatus(chan);
//        break;
//
//    case MSG_MOUNT_STATUS:
//#if MOUNT == ENABLED
//        CHECK_PAYLOAD_SIZE(MOUNT_STATUS);
//        copter.camera_mount.status_msg(chan);
//#endif // MOUNT == ENABLED
//        break;
//
//    case MSG_BATTERY2:
//        CHECK_PAYLOAD_SIZE(BATTERY2);
//        copter.gcs[chan-MAVLINK_COMM_0].send_battery2(copter.battery);
//        break;
//
//    case MSG_OPTICAL_FLOW:
//#if OPTFLOW == ENABLED
//        CHECK_PAYLOAD_SIZE(OPTICAL_FLOW);
//        copter.gcs[chan-MAVLINK_COMM_0].send_opticalflow(copter.ahrs, copter.optflow);
//#endif
//        break;
//
//    case MSG_GIMBAL_REPORT:
//#if MOUNT == ENABLED
//        CHECK_PAYLOAD_SIZE(GIMBAL_REPORT);
//        copter.camera_mount.send_gimbal_report(chan);
//#endif
//        break;
//
//    case MSG_EKF_STATUS_REPORT:
//        CHECK_PAYLOAD_SIZE(EKF_STATUS_REPORT);
//        copter.ahrs.get_NavEKF().send_status_report(chan);
//        break;
//
//    case MSG_FENCE_STATUS:
//    case MSG_WIND:
//        // unused
//        break;
//
//    case MSG_PID_TUNING:
//        CHECK_PAYLOAD_SIZE(PID_TUNING);
//        copter.send_pid_tuning(chan);
//        break;
//
//    case MSG_VIBRATION:
//        CHECK_PAYLOAD_SIZE(VIBRATION);
//        send_vibration(copter.ins);
//        break;
//
//    case MSG_MISSION_ITEM_REACHED:
//        CHECK_PAYLOAD_SIZE(MISSION_ITEM_REACHED);
//        mavlink_msg_mission_item_reached_send(chan, mission_item_reached_index);
//        break;
//
//    case MSG_RETRY_DEFERRED:
//        break; // just here to prevent a warning
//    }

    return true;
}



// see if we should send a stream now. Called at 50Hz
//bool GCS_MAVLINK::stream_trigger(enum streams stream_num)
//{
//    if (stream_num >= NUM_STREAMS) {
//        return false;
//    }
//    float rate = (uint8_t)streamRates[stream_num].get();
//
//    // send at a much lower rate while handling waypoints and
//    // parameter sends
//    if ((stream_num != STREAM_PARAMS) &&
//        (waypoint_receiving || _queued_parameter != NULL)) {
//        rate *= 0.25f;
//    }
//
//    if (rate <= 0) {
//        return false;
//    }
//
//    if (stream_ticks[stream_num] == 0) {
//        // we're triggering now, setup the next trigger point
//        if (rate > 50) {
//            rate = 50;
//        }
//        stream_ticks[stream_num] = (50 / rate) - 1 + stream_slowdown;
//        return true;
//    }
//
//    // count down at 50Hz
//    stream_ticks[stream_num]--;
//    return false;
//}

void
GCS_MAVLINK::data_stream_send(void)
{
//    if (waypoint_receiving) {
//        // don't interfere with mission transfer
//        return;
//    }
//
//    if (!copter.in_mavlink_delay && !copter.motors.armed()) {
//        handle_log_send(copter.DataFlash);
//    }
//
//    copter.gcs_out_of_time = false;
//
//    if (_queued_parameter != NULL) {
//        if (streamRates[STREAM_PARAMS].get() <= 0) {
//            streamRates[STREAM_PARAMS].set(10);
//        }
//        if (stream_trigger(STREAM_PARAMS)) {
//            send_message(MSG_NEXT_PARAM);
//        }
//        // don't send anything else at the same time as parameters
//        return;
//    }
//
//    if (copter.gcs_out_of_time) return;
//
//    if (copter.in_mavlink_delay) {
//        // don't send any other stream types while in the delay callback
//        return;
//    }
//
//    if (stream_trigger(STREAM_RAW_SENSORS)) {
//        send_message(MSG_RAW_IMU1);
//        send_message(MSG_RAW_IMU2);
//        send_message(MSG_RAW_IMU3);
//    }
//
//    if (copter.gcs_out_of_time) return;
//
//    if (stream_trigger(STREAM_EXTENDED_STATUS)) {
//        send_message(MSG_EXTENDED_STATUS1);
//        send_message(MSG_EXTENDED_STATUS2);
//        send_message(MSG_CURRENT_WAYPOINT);
//        send_message(MSG_GPS_RAW);
//        send_message(MSG_NAV_CONTROLLER_OUTPUT);
//        send_message(MSG_LIMITS_STATUS);
//    }
//
//    if (copter.gcs_out_of_time) return;
//
//    if (stream_trigger(STREAM_POSITION)) {
//        send_message(MSG_LOCATION);
//        send_message(MSG_LOCAL_POSITION);
//    }
//
//    if (copter.gcs_out_of_time) return;
//
//    if (stream_trigger(STREAM_RAW_CONTROLLER)) {
//        send_message(MSG_SERVO_OUT);
//    }
//
//    if (copter.gcs_out_of_time) return;
//
//    if (stream_trigger(STREAM_RC_CHANNELS)) {
//        send_message(MSG_RADIO_OUT);
//        send_message(MSG_RADIO_IN);
//    }
//
//    if (copter.gcs_out_of_time) return;
//
//    if (stream_trigger(STREAM_EXTRA1)) {
//        send_message(MSG_ATTITUDE);
//        send_message(MSG_SIMSTATE);
//        send_message(MSG_PID_TUNING);
//    }
//
//    if (copter.gcs_out_of_time) return;
//
//    if (stream_trigger(STREAM_EXTRA2)) {
//        send_message(MSG_VFR_HUD);
//    }
//
//    if (copter.gcs_out_of_time) return;
//
//    if (stream_trigger(STREAM_EXTRA3)) {
//        send_message(MSG_AHRS);
//        send_message(MSG_HWSTATUS);
//        send_message(MSG_SYSTEM_TIME);
//        send_message(MSG_RANGEFINDER);
//#if AP_TERRAIN_AVAILABLE
//        send_message(MSG_TERRAIN);
//#endif
//        send_message(MSG_BATTERY2);
//        send_message(MSG_MOUNT_STATUS);
//        send_message(MSG_OPTICAL_FLOW);
//        send_message(MSG_GIMBAL_REPORT);
//        send_message(MSG_EKF_STATUS_REPORT);
//        send_message(MSG_VIBRATION);
//        send_message(MSG_RPM);
//    }
}


//void GCS_MAVLINK::handle_guided_request(AP_Mission::Mission_Command &cmd)
//{
//    copter.do_guided(cmd);
//}
//
//void GCS_MAVLINK::handle_change_alt_request(AP_Mission::Mission_Command &cmd)
//{
//    // add home alt if needed
//    if (cmd.content.location.flags.relative_alt) {
//        cmd.content.location.alt += copter.ahrs.get_home().alt;
//    }
//
//    // To-Do: update target altitude for loiter or waypoint controller depending upon nav mode
//}

void GCS_MAVLINK::handleMessage(mavlink_message_t* msg)
{
    uint8_t result = MAV_RESULT_FAILED;         // assume failure.  Each messages id is responsible for return ACK or NAK if required

    switch (msg->msgid) {

    case MAVLINK_MSG_ID_HEARTBEAT:      // MAV ID: 0
    {
        // We keep track of the last time we received a heartbeat from our GCS for failsafe purposes
       // if(msg->sysid != copter.g.sysid_my_gcs) break;
        //copter.failsafe.last_heartbeat_ms = hal.scheduler->millis();
        //copter.pmTest1++;
        break;
    }

//    case MAVLINK_MSG_ID_SET_MODE:       // MAV ID: 11
//    {
//        handle_set_mode(msg, FUNCTOR_BIND(&copter, &Copter::set_mode, bool, uint8_t));
//        break;
//    }
//
//    case MAVLINK_MSG_ID_PARAM_REQUEST_READ:         // MAV ID: 20
//    {
//        handle_param_request_read(msg);
//        break;
//    }
//
//    case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:         // MAV ID: 21
//    {
//        // mark the firmware version in the tlog
//        send_text_P(SEVERITY_LOW, PSTR(FIRMWARE_STRING));
//
//#if defined(PX4_GIT_VERSION) && defined(NUTTX_GIT_VERSION)
//        send_text_P(SEVERITY_LOW, PSTR("PX4: " PX4_GIT_VERSION " NuttX: " NUTTX_GIT_VERSION));
//#endif
//        send_text_P(SEVERITY_LOW, PSTR("Frame: " FRAME_CONFIG_STRING));
//        handle_param_request_list(msg);
//        break;
//    }
//
//    case MAVLINK_MSG_ID_PARAM_SET:     // 23
//    {
//        handle_param_set(msg, &copter.DataFlash);
//        break;
//    }
//
//    case MAVLINK_MSG_ID_MISSION_WRITE_PARTIAL_LIST: // MAV ID: 38
//    {
//        handle_mission_write_partial_list(copter.mission, msg);
//        break;
//    }
//
//    // GCS has sent us a mission item, store to EEPROM
//    case MAVLINK_MSG_ID_MISSION_ITEM:           // MAV ID: 39
//    {
//        if (handle_mission_item(msg, copter.mission)) {
//            copter.DataFlash.Log_Write_EntireMission(copter.mission);
//        }
//        break;
//    }
//
//    // read an individual command from EEPROM and send it to the GCS
//    case MAVLINK_MSG_ID_MISSION_REQUEST:     // MAV ID: 40
//    {
//        handle_mission_request(copter.mission, msg);
//        break;
//    }
//
//    case MAVLINK_MSG_ID_MISSION_SET_CURRENT:    // MAV ID: 41
//    {
//        handle_mission_set_current(copter.mission, msg);
//        break;
//    }
//
//    // GCS request the full list of commands, we return just the number and leave the GCS to then request each command individually
//    case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:       // MAV ID: 43
//    {
//        handle_mission_request_list(copter.mission, msg);
//        break;
//    }
//
//    // GCS provides the full number of commands it wishes to upload
//    //  individual commands will then be sent from the GCS using the MAVLINK_MSG_ID_MISSION_ITEM message
//    case MAVLINK_MSG_ID_MISSION_COUNT:          // MAV ID: 44
//    {
//        handle_mission_count(copter.mission, msg);
//        break;
//    }
//
//    case MAVLINK_MSG_ID_MISSION_CLEAR_ALL:      // MAV ID: 45
//    {
//        handle_mission_clear_all(copter.mission, msg);
//        break;
//    }
//
//    case MAVLINK_MSG_ID_REQUEST_DATA_STREAM:    // MAV ID: 66
//    {
//        handle_request_data_stream(msg, false);
//        break;
//    }
//
//    case MAVLINK_MSG_ID_GIMBAL_REPORT:
//    {
//#if MOUNT == ENABLED
//        handle_gimbal_report(copter.camera_mount, msg);
//#endif
//        break;
//    }
//
//    case MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE:       // MAV ID: 70
//    {
//        // allow override of RC channel values for HIL
//        // or for complete GCS control of switch position
//        // and RC PWM values.
//        if(msg->sysid != copter.g.sysid_my_gcs) break;                         // Only accept control from our gcs
//        mavlink_rc_channels_override_t packet;
//        int16_t v[8];
//        mavlink_msg_rc_channels_override_decode(msg, &packet);
//
//        v[0] = packet.chan1_raw;
//        v[1] = packet.chan2_raw;
//        v[2] = packet.chan3_raw;
//        v[3] = packet.chan4_raw;
//        v[4] = packet.chan5_raw;
//        v[5] = packet.chan6_raw;
//        v[6] = packet.chan7_raw;
//        v[7] = packet.chan8_raw;
//
//        // record that rc are overwritten so we can trigger a failsafe if we lose contact with groundstation
//        copter.failsafe.rc_override_active = hal.rcin->set_overrides(v, 8);
//
//        // a RC override message is considered to be a 'heartbeat' from the ground station for failsafe purposes
//        copter.failsafe.last_heartbeat_ms = hal.scheduler->millis();
//        break;
//    }
//
//    // Pre-Flight calibration requests
//    case MAVLINK_MSG_ID_COMMAND_LONG:       // MAV ID: 76
//    {
//        // decode packet
//        mavlink_command_long_t packet;
//        mavlink_msg_command_long_decode(msg, &packet);
//
//        switch(packet.command) {
//
//        case MAV_CMD_START_RX_PAIR:
//            // initiate bind procedure
//            if (!hal.rcin->rc_bind(packet.param1)) {
//                result = MAV_RESULT_FAILED;
//            } else {
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_NAV_TAKEOFF: {
//            // param3 : horizontal navigation by pilot acceptable
//            // param4 : yaw angle   (not supported)
//            // param5 : latitude    (not supported)
//            // param6 : longitude   (not supported)
//            // param7 : altitude [metres]
//
//            float takeoff_alt = packet.param7 * 100;      // Convert m to cm
//
//            if(copter.do_user_takeoff(takeoff_alt, is_zero(packet.param3))) {
//                result = MAV_RESULT_ACCEPTED;
//            } else {
//                result = MAV_RESULT_FAILED;
//            }
//            break;
//        }
//
//
//        case MAV_CMD_NAV_LOITER_UNLIM:
//            if (copter.set_mode(LOITER)) {
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_NAV_RETURN_TO_LAUNCH:
//            if (copter.set_mode(RTL)) {
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_NAV_LAND:
//            if (copter.set_mode(LAND)) {
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_CONDITION_YAW:
//            // param1 : target angle [0-360]
//            // param2 : speed during change [deg per second]
//            // param3 : direction (-1:ccw, +1:cw)
//            // param4 : relative offset (1) or absolute angle (0)
//            if ((packet.param1 >= 0.0f)   &&
//            	(packet.param1 <= 360.0f) &&
//            	(is_zero(packet.param4) || is_equal(packet.param4,1.0f))) {
//            	copter.set_auto_yaw_look_at_heading(packet.param1, packet.param2, (int8_t)packet.param3, (uint8_t)packet.param4);
//                result = MAV_RESULT_ACCEPTED;
//            } else {
//                result = MAV_RESULT_FAILED;
//            }
//            break;
//
//        case MAV_CMD_DO_CHANGE_SPEED:
//            // param1 : unused
//            // param2 : new speed in m/s
//            // param3 : unused
//            // param4 : unused
//            if (packet.param2 > 0.0f) {
//                copter.wp_nav.set_speed_xy(packet.param2 * 100.0f);
//                result = MAV_RESULT_ACCEPTED;
//            } else {
//                result = MAV_RESULT_FAILED;
//            }
//            break;
//
//        case MAV_CMD_DO_SET_HOME:
//            // param1 : use current (1=use current location, 0=use specified location)
//            // param5 : latitude
//            // param6 : longitude
//            // param7 : altitude (absolute)
//            result = MAV_RESULT_FAILED; // assume failure
//            if(is_equal(packet.param1,1.0f) || (is_zero(packet.param5) && is_zero(packet.param6) && is_zero(packet.param7))) {
//                if (copter.set_home_to_current_location_and_lock()) {
//                    result = MAV_RESULT_ACCEPTED;
//                }
//            } else {
//                // sanity check location
//                if (fabsf(packet.param5) > 90.0f || fabsf(packet.param6) > 180.0f) {
//                    break;
//                }
//                Location new_home_loc;
//                new_home_loc.lat = (int32_t)(packet.param5 * 1.0e7f);
//                new_home_loc.lng = (int32_t)(packet.param6 * 1.0e7f);
//                new_home_loc.alt = (int32_t)(packet.param7 * 100.0f);
//                if (!copter.far_from_EKF_origin(new_home_loc)) {
//                    if (copter.set_home_and_lock(new_home_loc)) {
//                        result = MAV_RESULT_ACCEPTED;
//                    }
//                }
//            }
//            break;
//
//        case MAV_CMD_DO_FLIGHTTERMINATION:
//            if (packet.param1 > 0.5f) {
//                copter.init_disarm_motors();
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_DO_SET_ROI:
//            // param1 : regional of interest mode (not supported)
//            // param2 : mission index/ target id (not supported)
//            // param3 : ROI index (not supported)
//            // param5 : x / lat
//            // param6 : y / lon
//            // param7 : z / alt
//            // sanity check location
//            if (fabsf(packet.param5) > 90.0f || fabsf(packet.param6) > 180.0f) {
//                break;
//            }
//            Location roi_loc;
//            roi_loc.lat = (int32_t)(packet.param5 * 1.0e7f);
//            roi_loc.lng = (int32_t)(packet.param6 * 1.0e7f);
//            roi_loc.alt = (int32_t)(packet.param7 * 100.0f);
//            copter.set_auto_yaw_roi(roi_loc);
//            result = MAV_RESULT_ACCEPTED;
//            break;
//
//        case MAV_CMD_MISSION_START:
//            if (copter.motors.armed() && copter.set_mode(AUTO)) {
//                copter.set_auto_armed(true);
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_PREFLIGHT_CALIBRATION:
//            // exit immediately if armed
//            if (copter.motors.armed()) {
//                result = MAV_RESULT_FAILED;
//                break;
//            }
//            if (is_equal(packet.param1,1.0f)) {
//                // gyro offset calibration
//                copter.ins.init_gyro();
//                // reset ahrs gyro bias
//                if (copter.ins.gyro_calibrated_ok_all()) {
//                    copter.ahrs.reset_gyro_drift();
//                    result = MAV_RESULT_ACCEPTED;
//                } else {
//                    result = MAV_RESULT_FAILED;
//                }
//            } else if (is_equal(packet.param3,1.0f)) {
//                // fast barometer calibration
//                copter.init_barometer(false);
//                result = MAV_RESULT_ACCEPTED;
//            } else if (is_equal(packet.param4,1.0f)) {
//                result = MAV_RESULT_UNSUPPORTED;
//            } else if (is_equal(packet.param5,1.0f)) {
//                // 3d accel calibration
//                float trim_roll, trim_pitch;
//                // this blocks
//                AP_InertialSensor_UserInteract_MAVLink interact(this);
//                if(copter.ins.calibrate_accel(&interact, trim_roll, trim_pitch)) {
//                    // reset ahrs's trim to suggested values from calibration routine
//                    copter.ahrs.set_trim(Vector3f(trim_roll, trim_pitch, 0));
//                    result = MAV_RESULT_ACCEPTED;
//                } else {
//                    result = MAV_RESULT_FAILED;
//                }
//            } else if (is_equal(packet.param5,2.0f)) {
//                // accel trim
//                float trim_roll, trim_pitch;
//                if(copter.ins.calibrate_trim(trim_roll, trim_pitch)) {
//                    // reset ahrs's trim to suggested values from calibration routine
//                    copter.ahrs.set_trim(Vector3f(trim_roll, trim_pitch, 0));
//                    result = MAV_RESULT_ACCEPTED;
//                } else {
//                    result = MAV_RESULT_FAILED;
//                }
//            } else if (is_equal(packet.param6,1.0f)) {
//                // compassmot calibration
//                result = copter.mavlink_compassmot(chan);
//            }
//            break;
//
//        case MAV_CMD_PREFLIGHT_SET_SENSOR_OFFSETS:
//            if (is_equal(packet.param1,2.0f)) {
//                // save first compass's offsets
//                copter.compass.set_and_save_offsets(0, packet.param2, packet.param3, packet.param4);
//                result = MAV_RESULT_ACCEPTED;
//            }
//            if (is_equal(packet.param1,5.0f)) {
//                // save secondary compass's offsets
//                copter.compass.set_and_save_offsets(1, packet.param2, packet.param3, packet.param4);
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_COMPONENT_ARM_DISARM:
//            if (is_equal(packet.param1,1.0f)) {
//                // attempt to arm and return success or failure
//                if (copter.init_arm_motors(true)) {
//                    result = MAV_RESULT_ACCEPTED;
//                }
//            } else if (is_zero(packet.param1) && (copter.mode_has_manual_throttle(copter.control_mode) || copter.ap.land_complete || is_equal(packet.param2,21196.0f)))  {
//                // force disarming by setting param2 = 21196 is deprecated
//                copter.init_disarm_motors();
//                result = MAV_RESULT_ACCEPTED;
//            } else {
//                result = MAV_RESULT_UNSUPPORTED;
//            }
//            break;
//
//        case MAV_CMD_DO_SET_SERVO:
//            if (copter.ServoRelayEvents.do_set_servo(packet.param1, packet.param2)) {
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_DO_REPEAT_SERVO:
//            if (copter.ServoRelayEvents.do_repeat_servo(packet.param1, packet.param2, packet.param3, packet.param4*1000)) {
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_DO_SET_RELAY:
//            if (copter.ServoRelayEvents.do_set_relay(packet.param1, packet.param2)) {
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_DO_REPEAT_RELAY:
//            if (copter.ServoRelayEvents.do_repeat_relay(packet.param1, packet.param2, packet.param3*1000)) {
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN:
//            if (is_equal(packet.param1,1.0f) || is_equal(packet.param1,3.0f)) {
//                AP_Notify::events.firmware_update = 1;
//                copter.update_notify();
//                hal.scheduler->delay(50);
//                // when packet.param1 == 3 we reboot to hold in bootloader
//                hal.scheduler->reboot(is_equal(packet.param1,3.0f));
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//
//        case MAV_CMD_DO_FENCE_ENABLE:
//#if AC_FENCE == ENABLED
//            result = MAV_RESULT_ACCEPTED;
//            switch ((uint16_t)packet.param1) {
//                case 0:
//                    copter.fence.enable(false);
//                    break;
//                case 1:
//                    copter.fence.enable(true);
//                    break;
//                default:
//                    result = MAV_RESULT_FAILED;
//                    break;
//            }
//#else
//            // if fence code is not included return failure
//            result = MAV_RESULT_FAILED;
//#endif
//            break;
//
//#if PARACHUTE == ENABLED
//        case MAV_CMD_DO_PARACHUTE:
//            // configure or release parachute
//            result = MAV_RESULT_ACCEPTED;
//            switch ((uint16_t)packet.param1) {
//                case PARACHUTE_DISABLE:
//                    copter.parachute.enabled(false);
//                    copter.Log_Write_Event(DATA_PARACHUTE_DISABLED);
//                    break;
//                case PARACHUTE_ENABLE:
//                    copter.parachute.enabled(true);
//                    copter.Log_Write_Event(DATA_PARACHUTE_ENABLED);
//                    break;
//                case PARACHUTE_RELEASE:
//                    // treat as a manual release which performs some additional check of altitude
//                    copter.parachute_manual_release();
//                    break;
//                default:
//                    result = MAV_RESULT_FAILED;
//                    break;
//            }
//            break;
//#endif
//
//        case MAV_CMD_DO_MOTOR_TEST:
//            // param1 : motor sequence number (a number from 1 to max number of motors on the vehicle)
//            // param2 : throttle type (0=throttle percentage, 1=PWM, 2=pilot throttle channel pass-through. See MOTOR_TEST_THROTTLE_TYPE enum)
//            // param3 : throttle (range depends upon param2)
//            // param4 : timeout (in seconds)
//            result = copter.mavlink_motor_test_start(chan, (uint8_t)packet.param1, (uint8_t)packet.param2, (uint16_t)packet.param3, packet.param4);
//            break;
//
//#if EPM_ENABLED == ENABLED
//        case MAV_CMD_DO_GRIPPER:
//            // param1 : gripper number (ignored)
//            // param2 : action (0=release, 1=grab). See GRIPPER_ACTIONS enum.
//            if(!copter.epm.enabled()) {
//                result = MAV_RESULT_FAILED;
//            } else {
//                result = MAV_RESULT_ACCEPTED;
//                switch ((uint8_t)packet.param2) {
//                    case GRIPPER_ACTION_RELEASE:
//                        copter.epm.release();
//                        break;
//                    case GRIPPER_ACTION_GRAB:
//                        copter.epm.grab();
//                        break;
//                    default:
//                        result = MAV_RESULT_FAILED;
//                        break;
//                }
//            }
//            break;
//#endif
//
//        case MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES: {
//            if (is_equal(packet.param1,1.0f)) {
//                copter.gcs[chan-MAVLINK_COMM_0].send_autopilot_version(FIRMWARE_VERSION);
//                result = MAV_RESULT_ACCEPTED;
//            }
//            break;
//        }
//
//        default:
//            result = MAV_RESULT_UNSUPPORTED;
//            break;
//        }
//
//        // send ACK or NAK
//        mavlink_msg_command_ack_send_buf(msg, chan, packet.command, result);
//
//        break;
//    }
//
//    case MAVLINK_MSG_ID_COMMAND_ACK:        // MAV ID: 77
//    {
//        copter.command_ack_counter++;
//        break;
//    }
//
//    case MAVLINK_MSG_ID_SET_POSITION_TARGET_LOCAL_NED:     // MAV ID: 84
//    {
//        // decode packet
//        mavlink_set_position_target_local_ned_t packet;
//        mavlink_msg_set_position_target_local_ned_decode(msg, &packet);
//
//        // exit if vehicle is not in Guided mode or Auto-Guided mode
//        if ((copter.control_mode != GUIDED) && !(copter.control_mode == AUTO && copter.auto_mode == Auto_NavGuided)) {
//            break;
//        }
//
//        // check for supported coordinate frames
//        if (packet.coordinate_frame != MAV_FRAME_LOCAL_NED &&
//            packet.coordinate_frame != MAV_FRAME_LOCAL_OFFSET_NED &&
//            packet.coordinate_frame != MAV_FRAME_BODY_NED &&
//            packet.coordinate_frame != MAV_FRAME_BODY_OFFSET_NED) {
//            break;
//        }
//
//        bool pos_ignore      = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_POS_IGNORE;
//        bool vel_ignore      = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_VEL_IGNORE;
//        bool acc_ignore      = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_ACC_IGNORE;
//
//        /*
//         * for future use:
//         * bool force           = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_FORCE;
//         * bool yaw_ignore      = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_YAW_IGNORE;
//         * bool yaw_rate_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_YAW_RATE_IGNORE;
//         */
//
//        // prepare position
//        Vector3f pos_vector;
//        if (!pos_ignore) {
//            // convert to cm
//            pos_vector = Vector3f(packet.x * 100.0f, packet.y * 100.0f, -packet.z * 100.0f);
//            // rotate to body-frame if necessary
//            if (packet.coordinate_frame == MAV_FRAME_BODY_NED ||
//                packet.coordinate_frame == MAV_FRAME_BODY_OFFSET_NED) {
//                copter.rotate_body_frame_to_NE(pos_vector.x, pos_vector.y);
//            }
//            // add body offset if necessary
//            if (packet.coordinate_frame == MAV_FRAME_LOCAL_OFFSET_NED ||
//                packet.coordinate_frame == MAV_FRAME_BODY_NED ||
//                packet.coordinate_frame == MAV_FRAME_BODY_OFFSET_NED) {
//                pos_vector += copter.inertial_nav.get_position();
//            } else {
//                // convert from alt-above-home to alt-above-ekf-origin
//                pos_vector.z = copter.pv_alt_above_origin(pos_vector.z);
//            }
//        }
//
//        // prepare velocity
//        Vector3f vel_vector;
//        if (!vel_ignore) {
//            // convert to cm
//            vel_vector = Vector3f(packet.vx * 100.0f, packet.vy * 100.0f, -packet.vz * 100.0f);
//            // rotate to body-frame if necessary
//            if (packet.coordinate_frame == MAV_FRAME_BODY_NED || packet.coordinate_frame == MAV_FRAME_BODY_OFFSET_NED) {
//                copter.rotate_body_frame_to_NE(vel_vector.x, vel_vector.y);
//            }
//        }
//
//        // send request
//        if (!pos_ignore && !vel_ignore && acc_ignore) {
//            copter.guided_set_destination_posvel(pos_vector, vel_vector);
//        } else if (pos_ignore && !vel_ignore && acc_ignore) {
//            copter.guided_set_velocity(vel_vector);
//        } else if (!pos_ignore && vel_ignore && acc_ignore) {
//            copter.guided_set_destination(pos_vector);
//        } else {
//            result = MAV_RESULT_FAILED;
//        }
//
//        break;
//    }
//
//    case MAVLINK_MSG_ID_SET_POSITION_TARGET_GLOBAL_INT:    // MAV ID: 86
//    {
//        // decode packet
//        mavlink_set_position_target_global_int_t packet;
//        mavlink_msg_set_position_target_global_int_decode(msg, &packet);
//
//        // exit if vehicle is not in Guided mode or Auto-Guided mode
//        if ((copter.control_mode != GUIDED) && !(copter.control_mode == AUTO && copter.auto_mode == Auto_NavGuided)) {
//            break;
//        }
//
//        // check for supported coordinate frames
//        if (packet.coordinate_frame != MAV_FRAME_GLOBAL_INT &&
//            packet.coordinate_frame != MAV_FRAME_GLOBAL_RELATIVE_ALT_INT &&
//            packet.coordinate_frame != MAV_FRAME_GLOBAL_TERRAIN_ALT_INT) {
//            break;
//        }
//
//        bool pos_ignore      = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_POS_IGNORE;
//        bool vel_ignore      = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_VEL_IGNORE;
//        bool acc_ignore      = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_ACC_IGNORE;
//
//        /*
//         * for future use:
//         * bool force           = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_FORCE;
//         * bool yaw_ignore      = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_YAW_IGNORE;
//         * bool yaw_rate_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_YAW_RATE_IGNORE;
//         */
//
//        Vector3f pos_ned;
//
//        if(!pos_ignore) {
//            Location loc;
//            loc.lat = packet.lat_int;
//            loc.lng = packet.lon_int;
//            loc.alt = packet.alt*100;
//            switch (packet.coordinate_frame) {
//                case MAV_FRAME_GLOBAL_RELATIVE_ALT_INT:
//                    loc.flags.relative_alt = true;
//                    loc.flags.terrain_alt = false;
//                    break;
//                case MAV_FRAME_GLOBAL_TERRAIN_ALT_INT:
//                    loc.flags.relative_alt = true;
//                    loc.flags.terrain_alt = true;
//                    break;
//                case MAV_FRAME_GLOBAL_INT:
//                default:
//                    loc.flags.relative_alt = false;
//                    loc.flags.terrain_alt = false;
//                    break;
//            }
//            pos_ned = copter.pv_location_to_vector(loc);
//        }
//
//        if (!pos_ignore && !vel_ignore && acc_ignore) {
//            copter.guided_set_destination_posvel(pos_ned, Vector3f(packet.vx * 100.0f, packet.vy * 100.0f, -packet.vz * 100.0f));
//        } else if (pos_ignore && !vel_ignore && acc_ignore) {
//            copter.guided_set_velocity(Vector3f(packet.vx * 100.0f, packet.vy * 100.0f, -packet.vz * 100.0f));
//        } else if (!pos_ignore && vel_ignore && acc_ignore) {
//            copter.guided_set_destination(pos_ned);
//        } else {
//            result = MAV_RESULT_FAILED;
//        }
//
//        break;
//    }
//
//#if HIL_MODE != HIL_MODE_DISABLED
//    case MAVLINK_MSG_ID_HIL_STATE:          // MAV ID: 90
//    {
//        mavlink_hil_state_t packet;
//        mavlink_msg_hil_state_decode(msg, &packet);
//
//        // set gps hil sensor
//        Location loc;
//        loc.lat = packet.lat;
//        loc.lng = packet.lon;
//        loc.alt = packet.alt/10;
//        Vector3f vel(packet.vx, packet.vy, packet.vz);
//        vel *= 0.01f;
//
//        gps.setHIL(0, AP_GPS::GPS_OK_FIX_3D,
//                   packet.time_usec/1000,
//                   loc, vel, 10, 0, true);
//
//        // rad/sec
//        Vector3f gyros;
//        gyros.x = packet.rollspeed;
//        gyros.y = packet.pitchspeed;
//        gyros.z = packet.yawspeed;
//
//        // m/s/s
//        Vector3f accels;
//        accels.x = packet.xacc * (GRAVITY_MSS/1000.0f);
//        accels.y = packet.yacc * (GRAVITY_MSS/1000.0f);
//        accels.z = packet.zacc * (GRAVITY_MSS/1000.0f);
//
//        ins.set_gyro(0, gyros);
//
//        ins.set_accel(0, accels);
//
//        copter.barometer.setHIL(packet.alt*0.001f);
//        copter.compass.setHIL(0, packet.roll, packet.pitch, packet.yaw);
//        copter.compass.setHIL(1, packet.roll, packet.pitch, packet.yaw);
//
//        break;
//    }
//#endif //  HIL_MODE != HIL_MODE_DISABLED
//
//    case MAVLINK_MSG_ID_RADIO:
//    case MAVLINK_MSG_ID_RADIO_STATUS:       // MAV ID: 109
//    {
//        handle_radio_status(msg, copter.DataFlash, copter.should_log(MASK_LOG_PM));
//        break;
//    }
//
//    case MAVLINK_MSG_ID_LOG_REQUEST_DATA:
//    case MAVLINK_MSG_ID_LOG_ERASE:
//        copter.in_log_download = true;
//        // fallthru
//    case MAVLINK_MSG_ID_LOG_REQUEST_LIST:
//        if (!copter.in_mavlink_delay && !copter.motors.armed()) {
//            handle_log_message(msg, copter.DataFlash);
//        }
//        break;
//    case MAVLINK_MSG_ID_LOG_REQUEST_END:
//        copter.in_log_download = false;
//        if (!copter.in_mavlink_delay && !copter.motors.armed()) {
//            handle_log_message(msg, copter.DataFlash);
//        }
//        break;
//
//#if HAL_CPU_CLASS > HAL_CPU_CLASS_16
//    case MAVLINK_MSG_ID_SERIAL_CONTROL:
//        handle_serial_control(msg, copter.gps);
//        break;
//
//    case MAVLINK_MSG_ID_GPS_INJECT_DATA:
//        handle_gps_inject(msg, copter.gps);
//        result = MAV_RESULT_ACCEPTED;
//        break;
//
//#endif
//
//#if CAMERA == ENABLED
//    case MAVLINK_MSG_ID_DIGICAM_CONFIGURE:      // MAV ID: 202
//        break;
//
//    case MAVLINK_MSG_ID_DIGICAM_CONTROL:
//        copter.camera.control_msg(msg);
//        copter.log_picture();
//        break;
//#endif // CAMERA == ENABLED
//
//#if MOUNT == ENABLED
//    case MAVLINK_MSG_ID_MOUNT_CONFIGURE:        // MAV ID: 204
//        copter.camera_mount.configure_msg(msg);
//        break;
//
//    case MAVLINK_MSG_ID_MOUNT_CONTROL:
//        copter.camera_mount.control_msg(msg);
//        break;
//#endif // MOUNT == ENABLED
//
//    case MAVLINK_MSG_ID_TERRAIN_DATA:
//    case MAVLINK_MSG_ID_TERRAIN_CHECK:
//#if AP_TERRAIN_AVAILABLE
//        copter.terrain.handle_data(chan, msg);
//#endif
//        break;
//
//#if AC_RALLY == ENABLED
//    // receive a rally point from GCS and store in EEPROM
//    case MAVLINK_MSG_ID_RALLY_POINT: {
//        mavlink_rally_point_t packet;
//        mavlink_msg_rally_point_decode(msg, &packet);
//
//        if (packet.idx >= copter.rally.get_rally_total() ||
//            packet.idx >= copter.rally.get_rally_max()) {
//            send_text_P(SEVERITY_LOW,PSTR("bad rally point message ID"));
//            break;
//        }
//
//        if (packet.count != copter.rally.get_rally_total()) {
//            send_text_P(SEVERITY_LOW,PSTR("bad rally point message count"));
//            break;
//        }
//
//        RallyLocation rally_point;
//        rally_point.lat = packet.lat;
//        rally_point.lng = packet.lng;
//        rally_point.alt = packet.alt;
//        rally_point.break_alt = packet.break_alt;
//        rally_point.land_dir = packet.land_dir;
//        rally_point.flags = packet.flags;
//
//        if (!copter.rally.set_rally_point_with_index(packet.idx, rally_point)) {
//            send_text_P(SEVERITY_HIGH, PSTR("error setting rally point"));
//        }
//
//        break;
//    }
//
//    //send a rally point to the GCS
//    case MAVLINK_MSG_ID_RALLY_FETCH_POINT: {
//        //send_text_P(SEVERITY_HIGH, PSTR("## getting rally point in GCS_Mavlink.cpp 1")); // #### TEMP
//
//        mavlink_rally_fetch_point_t packet;
//        mavlink_msg_rally_fetch_point_decode(msg, &packet);
//
//        //send_text_P(SEVERITY_HIGH, PSTR("## getting rally point in GCS_Mavlink.cpp 2")); // #### TEMP
//
//        if (packet.idx > copter.rally.get_rally_total()) {
//            send_text_P(SEVERITY_LOW, PSTR("bad rally point index"));
//            break;
//        }
//
//        //send_text_P(SEVERITY_HIGH, PSTR("## getting rally point in GCS_Mavlink.cpp 3")); // #### TEMP
//
//        RallyLocation rally_point;
//        if (!copter.rally.get_rally_point_with_index(packet.idx, rally_point)) {
//           send_text_P(SEVERITY_LOW, PSTR("failed to set rally point"));
//           break;
//        }
//
//        //send_text_P(SEVERITY_HIGH, PSTR("## getting rally point in GCS_Mavlink.cpp 4")); // #### TEMP
//
//        mavlink_msg_rally_point_send_buf(msg,
//                                         chan, msg->sysid, msg->compid, packet.idx,
//                                         copter.rally.get_rally_total(), rally_point.lat, rally_point.lng,
//                                         rally_point.alt, rally_point.break_alt, rally_point.land_dir,
//                                         rally_point.flags);
//
//        //send_text_P(SEVERITY_HIGH, PSTR("## getting rally point in GCS_Mavlink.cpp 5")); // #### TEMP
//
//        break;
//    }
//#endif // AC_RALLY == ENABLED
//
//    case MAVLINK_MSG_ID_AUTOPILOT_VERSION_REQUEST:
//        copter.gcs[chan-MAVLINK_COMM_0].send_autopilot_version(FIRMWARE_VERSION);
//        break;
//
//    case MAVLINK_MSG_ID_LED_CONTROL:
//        // send message to Notify
//        AP_Notify::handle_led_control(msg);
//        break;

    }     // end switch
} // end handle mavlink


/*
 *  a delay() callback that processes MAVLink packets. We set this as the
 *  callback in long running library initialisation routines to allow
 *  MAVLink to process packets while waiting for the initialisation to
 *  complete
 */
void Copter::mavlink_delay_cb()
{

}

///*
// *  send a message on both GCS links
// */
//void Copter::gcs_send_message(enum ap_message id)
//{
//    for (uint8_t i=0; i<num_gcs; i++) {
//        if (gcs[i].initialised) {
//            gcs[i].send_message(id);
//        }
//    }
//}

/*
 *  send a mission item reached message and load the index before the send attempt in case it may get delayed
 */
void Copter::gcs_send_mission_item_reached_message(uint16_t mission_index)
{

}

/*
 *  send data streams in the given rate range on both links
 */
void Copter::gcs_data_stream_send(void)
{
//    for (uint8_t i=0; i<num_gcs; i++) {
//        if (gcs[i].initialised) {
//            gcs[i].data_stream_send();
//        }
//    }
}

/*
 *  look for incoming commands on the GCS links
 */
void Copter::gcs_check_input(void)
{
//    for (uint8_t i=0; i<num_gcs; i++) {
//        if (gcs[i].initialised) {
//#if CLI_ENABLED == ENABLED
//            gcs[i].update(g.cli_enabled==1?FUNCTOR_BIND_MEMBER(&Copter::run_cli, void, AP_HAL::UARTDriver *):NULL);
//#else
//            gcs[i].update(NULL);
//#endif
//        }
//    }
}

//void Copter::gcs_send_text_P(gcs_severity severity, const prog_char_t *str)
//{
////    for (uint8_t i=0; i<num_gcs; i++) {
////        if (gcs[i].initialised) {
////            gcs[i].send_text_P(severity, str);
////        }
////    }
//}

/*
 *  send a low priority formatted message to the GCS
 *  only one fits in the queue, so if you send more than one before the
 *  last one gets into the serial buffer then the old one will be lost
 */
//void Copter::gcs_send_text_fmt(const prog_char_t *fmt, ...)
//{
//    va_list arg_list;
//    gcs[0].pending_status.severity = (uint8_t)SEVERITY_LOW;
//    va_start(arg_list, fmt);
//    hal.util->vsnprintf_P((char *)gcs[0].pending_status.text,
//            sizeof(gcs[0].pending_status.text), fmt, arg_list);
//    va_end(arg_list);
//    gcs[0].send_message(MSG_STATUSTEXT);
//    for (uint8_t i=1; i<num_gcs; i++) {
//        if (gcs[i].initialised) {
//            gcs[i].pending_status = gcs[0].pending_status;
//            gcs[i].send_message(MSG_STATUSTEXT);
//        }
//    }
//}


void
GCS_MAVLINK::send_message(enum ap_message id)
{
    mavlink_send_message(chan,id, packet_drops);
}

// send a message using mavlink
void GCS_MAVLINK::mavlink_send_message(mavlink_channel_t chan, enum ap_message id, uint16_t packet_drops)
{
//    uint8_t i, nextid;
//    struct mavlink_queue *q = &mavlink_queue[(uint8_t)chan];
//
//    // see if we can send the deferred messages, if any
//    while (q->num_deferred_messages != 0) {
//        if (!mavlink_try_send_message(chan,
//                                      q->deferred_messages[q->next_deferred_message],
//                                      packet_drops)) {
//            break;
//        }
//        q->next_deferred_message++;
//        if (q->next_deferred_message == MAX_DEFERRED_MESSAGES) {
//            q->next_deferred_message = 0;
//        }
//        q->num_deferred_messages--;
//    }
//
//    if (id == MSG_RETRY_DEFERRED) {
//        return;
//    }
//
//    // this message id might already be deferred
//    for (i=0, nextid = q->next_deferred_message; i < q->num_deferred_messages; i++) {
//        if (q->deferred_messages[nextid] == id) {
//            // its already deferred, discard
//            return;
//        }
//        nextid++;
//        if (nextid == MAX_DEFERRED_MESSAGES) {
//            nextid = 0;
//        }
//    }
//
//    if (q->num_deferred_messages != 0 ||
//        !mavlink_try_send_message(chan, id, packet_drops)) {
//        // can't send it now, so defer it
//        if (q->num_deferred_messages == MAX_DEFERRED_MESSAGES) {
//            // the defer buffer is full, discard
//            return;
//        }
//        nextid = q->next_deferred_message + q->num_deferred_messages;
//        if (nextid >= MAX_DEFERRED_MESSAGES) {
//            nextid -= MAX_DEFERRED_MESSAGES;
//        }
//        q->deferred_messages[nextid] = id;
//        q->num_deferred_messages++;
//    }
}

#endif