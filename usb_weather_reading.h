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
#pragma pack(1)
class usb_weather_reading
{
public:
	uint8_t  delay;						// minutes since last reading
	uint8_t  indoor_humidity;
	uint16_t indoor_temperature;		// multiply by 0.1 to get degrees C
	uint8_t  outdoor_humidity;
	uint16_t outdoor_temperature;		// multiply by 0.1 to get degrees C
	uint16_t absolute_pressure;			// multiply by 0.1 to get hPa
	uint8_t  average_windspeed_low;		// multiply by 0.1 to get m/s
	uint8_t  gust_windspeed_low;		// multiply by 0.1 to get m/s
	uint8_t  windspeed_high;			// low 4 bits are average windspees, high 4 bits are gust windspeed
	uint8_t  wind_direction;			// multiply by 22.5 to get degrees from north
	uint16_t total_rain;				// multiply by 0.3 to get mm
	uint8_t  status;					// bit 7 = rain counter overflow.  bit 6 = lost contact with sensors
	/*
		The WH3080 additionally has these 4 bytes (light level and UV level)
	*/
	uint8_t light_low;					// 3 byte integer
	uint8_t light;
	uint8_t light_high;
	uint8_t uv;							// UV level
	/*
		Pack the structure out to 32 bytes
	*/
	uint8_t packing[12];

public:
	void text_render(char *title = "");
} ;
#pragma pack()


#endif /* USB_WEATHER_READING_H_ */
