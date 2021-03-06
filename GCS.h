/*
 * GCS.h
 *
 *  Created on: 2017-9-18
 *      Author: wangbo
 */

#ifndef GCS_H_
#define GCS_H_

#include "copter.h"
#ifdef LINUX_OS
//#include <FastSerial.h>
//#include <AP_Common.h>
//#include <GCS_MAVLink.h>
//#include <GPS.h>
//#include <Stream.h>
#include <stdint.h>
#include "AP_Param.h"
#include "Parameters.h"

///
/// @class	GCS
/// @brief	Class describing the interface between the APM code
///			proper and the GCS implementation.
///
/// GCS' are currently implemented inside the sketch and as such have
/// access to all global state.  The sketch should not, however, call GCS
/// internal functions - all calls to the GCS should be routed through
/// this interface (or functions explicitly exposed by a subclass).
///
class GCS_Class
{
public:

	/// Startup initialisation.
	///
	/// This routine performs any one-off initialisation required before
	/// GCS messages are exchanged.
	///
	/// @note The stream is expected to be set up and configured for the
	///       correct bitrate before ::init is called.
	///
	/// @note The stream is currently BetterStream so that we can use the _P
	///	      methods; this may change if Arduino adds them to Print.
	///
	/// @param	port		The stream over which messages are exchanged.
//	///
//	void		init(FastSerial *port) {
//        _port = port;
//        initialised = true;
//    }

	/// Update GCS state.
	///
	/// This may involve checking for received bytes on the stream,
	/// or sending additional periodic messages.
	void		update(void) {}

	/// Send a message with a single numeric parameter.
	///
	/// This may be a standalone message, or the GCS driver may
	/// have its own way of locating additional parameters to send.
	///
	/// @param	id			ID of the message to send.
	/// @param	param		Explicit message parameter.
	///
//	void		send_message(enum ap_message id) {}

	/// Send a text message.
	///
	/// @param	severity	A value describing the importance of the message.
	/// @param	str			The text to be sent.
	///
//	void		send_text(gcs_severity severity, const char *str) {}

	/// Send a text message with a PSTR()
	///
	/// @param	severity	A value describing the importance of the message.
	/// @param	str			The text to be sent.
	///
//	void		send_text(gcs_severity severity, const prog_char_t *str) {}

    // test if frequency within range requested for loop
    // used by data_stream_send
    static bool freqLoopMatch(uint16_t freq, uint16_t freqMin, uint16_t freqMax)
    {
        if (freq != 0 && freq >= freqMin && freq < freqMax) return true;
        else return false;
    }

    // send streams which match frequency range
    void data_stream_send(uint16_t freqMin, uint16_t freqMax);

    //void    data_stream_send(void);
 	//void    queued_param_send();
 	void    queued_waypoint_send();



    // set to true if this GCS link is active
    bool initialised;

protected:
	/// The stream we are communicating over
//	FastSerial			*_port;
};

//
// GCS class definitions.
//
// These are here so that we can declare the GCS object early in the sketch
// and then reference it statically rather than via a pointer.
//

///
/// @class	GCS_MAVLINK
/// @brief	The mavlink protocol for qgroundcontrol
///
class GCS_MAVLINK : public GCS_Class
{
public:
//	GCS_MAVLINK(AP_Var::Key key);
	GCS_MAVLINK(){}
	void    update(void);
	//void	init(FastSerial *port);
	void	init(char *port);//这个应该是有串口来进行初始化的，我先用char*代替吧，反正用那个channnel 在最后发送的地方改一下就可以了
//	void	send_message(enum ap_message id);
	//void	send_text(gcs_severity severity, const char *str);
//	void	send_text(gcs_severity severity, const prog_char_t *str);
    void    data_stream_send(uint16_t freqMin, uint16_t freqMax);
	void    queued_param_send();
	void    queued_waypoint_send();

	//20170921
	bool try_send_message(enum ap_message id);
	void data_stream_send(void);
	void send_message(enum ap_message id);
	void mavlink_send_message(mavlink_channel_t chan, enum ap_message id, uint16_t packet_drops);
	bool mavlink_try_send_message(mavlink_channel_t chan, enum ap_message id, uint16_t packet_drops);
	//void try_send_message(enum ap_message id);

	  // NOTE! The streams enum below and the
	// set of AP_Int16 stream rates _must_ be
	// kept in the same order
	enum streams {STREAM_RAW_SENSORS,
				  STREAM_EXTENDED_STATUS,
				  STREAM_RC_CHANNELS,
				  STREAM_RAW_CONTROLLER,
				  STREAM_POSITION,
				  STREAM_EXTRA1,
				  STREAM_EXTRA2,
				  STREAM_EXTRA3,
				  STREAM_PARAMS,
				  NUM_STREAMS};

	// see if we should send a stream now. Called at 50Hz
	bool stream_trigger(enum streams stream_num);
	void 	handleMessage(mavlink_message_t * msg);
//private:
//	void 	handleMessage(mavlink_message_t * msg);

	/// Perform queued sending operations
	///

	/// Perform queued sending operations
	///
	AP_Param   *_queued_parameter;                  ///< next parameter to be sent in queue
	enum ap_var_type _queued_parameter_type;        ///< type of the next parameter
	AP_Param::ParamToken	_queued_parameter_token;///AP_Param token for next() call
	uint16_t    _queued_parameter_index;            ///< next queued parameter's index
	uint16_t    _queued_parameter_count;            ///< saved count of parameters for queued send


	/*
	 * 20171002我不按照apm的AP_Param来写了，怎么建立了参数的数组param_all
	 */
	T_PARAM   *_queued_parameter_wang;                  ///< next parameter to be sent in queue




	//AP_Var      *_queued_parameter;                 ///< next parameter to be sent in queue
//	uint16_t    _queued_parameter_index;            ///< next queued parameter's index
//    uint16_t    _queued_parameter_count;            ///< saved count of parameters for queued send

	/// Count the number of reportable parameters.
	///
	/// Not all parameters can be reported via MAVlink.  We count the number that are
	/// so that we can report to a GCS the number of parameters it should expect when it
	/// requests the full set.
	///
	/// @return         The number of reportable parameters.
	///
    uint16_t    _count_parameters();                ///< count reportable parameters

    uint16_t    _parameter_count;                   ///< cache of reportable parameters
    //AP_Var      *_find_parameter(uint16_t index);   ///< find a reportable parameter by index

	mavlink_channel_t chan;
    uint16_t packet_drops;

#if CLI_ENABLED == ENABLED
    // this allows us to detect the user wanting the CLI to start
    uint8_t crlf_count;
#endif

	// waypoints
	uint16_t waypoint_request_i; // request index
	uint16_t waypoint_dest_sysid; // where to send requests
	uint16_t waypoint_dest_compid; // "
	bool waypoint_sending; // currently in send process
	bool waypoint_receiving; // currently receiving
	uint16_t waypoint_count;
	uint32_t waypoint_timelast_send; // milliseconds
	uint32_t waypoint_timelast_receive; // milliseconds
	uint32_t waypoint_timelast_request; // milliseconds
	uint16_t waypoint_send_timeout; // milliseconds
	uint16_t waypoint_receive_timeout; // milliseconds

	/*
	 * 20171002这下面的序列不能随便改，决定了发送各个类型包的频率，与bool GCS_MAVLINK::stream_trigger(enum streams stream_num)函数有关
	 */
	// data stream rates
	//AP_Var_group	    _group;
	int16_t streamRateRawSensors;
	int16_t streamRateExtendedStatus;
	int16_t streamRateRCChannels;
	int16_t streamRateRawController;
	int16_t streamRatePosition;
	int16_t streamRateExtra1;
	int16_t streamRateExtra2;
	int16_t streamRateExtra3;

    // number of 50Hz ticks until we next send this stream
    uint8_t stream_ticks[NUM_STREAMS];

    // number of extra ticks to add to slow things down for the radio
    uint8_t stream_slowdown;

    // true when we have received at least 1 MAVLink packet
    bool mavlink_active;

    // saveable rate of each stream
       int16_t        streamRates[NUM_STREAMS];

};



#endif




#endif /* GCS_H_ */
