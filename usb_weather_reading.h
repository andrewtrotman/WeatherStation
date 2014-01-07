/*
	USB_WEATHER_READING.H
	---------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef USB_WEATHER_READING_H_
#define USB_WEATHER_READING_H_

#include "fundamental_types.h"

/*
	class USB_WEATHER_READING
	-------------------------
*/
class usb_weather_reading
{
public:
	/*
		The station keeps writing the current values to the same area of
		memory every 48 seconds, until the logging interval comes around,
		then it moves on to the next memory location, leaving the last
		values it wrote as the logger data.
		(see:http://sandaysoft.com/forum/viewtopic.php?f=13&t=10524)
	*/
	uint8_t  delay;						// minutes since last recording
	double indoor_humidity;				// percent
	double indoor_temperature;			// degrees C
	double outdoor_humidity;			// percent
	double outdoor_temperature;			// degrees C
	double absolute_pressure;			// hPa
	double average_windspeed;			// m/s
	double gust_windspeed;				// m/s
	double wind_direction;				// degrees from north
	double total_rain;					// mm
	uint8_t rain_counter_overflow;		// true or false
	uint8_t lost_communications;		// true or false

public:
	void text_render(const char *title = "");
} ;

#endif /* USB_WEATHER_READING_H_ */
