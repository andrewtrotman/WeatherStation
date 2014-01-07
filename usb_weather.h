/*
	USB_WEATHER.H
	-------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef USB_WEATHER_H_
#define USB_WEATHER_H_

#ifdef _MSC_VER
	#include <Windows.h>
#else
	typedef long long HANDLE;
#endif

#include "fundamental_types.h"
#include "usb_weather_fixed_block_1080.h"
#include "usb_weather_reading.h"

/*
	class USB_WEATHER
	-----------------
*/
class usb_weather
{
private:
	HANDLE hDevice;
	usb_weather_fixed_block_1080 *fixed_block;

protected:
	uint32_t read(uint16_t address, void *result);

public:
	usb_weather();
	virtual ~usb_weather();
	uint32_t connect(uint32_t vid, uint32_t pid);

	usb_weather_reading *read_reading(uint16_t address);
	usb_weather_fixed_block_1080 *read_fixed_block(void);
	usb_weather_reading *read_current_readings(void);
	usb_weather_reading *read_previous_readings(void);
	usb_weather_reading *read_hourly_delta(void);
	usb_weather_reading **read_all_readings(uint32_t *readings);
} ;

#endif /* USB_WEATHER_H_ */
