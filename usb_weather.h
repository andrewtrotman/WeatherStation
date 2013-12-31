/*
	USB_WEATHER.H
	-------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef USB_WEATHER_H_
#define USB_WEATHER_H_

#include <Windows.h>

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

protected:
	uint32_t read(uint16_t address, void *result);

public:
	usb_weather();
	virtual ~usb_weather();
	uint32_t connect(uint32_t vid, uint32_t pid);

	usb_weather_reading *read_reading(uint16_t address);
	usb_weather_fixed_block_1080 *read_fixed_block(void);
	usb_weather_reading **read_all_readings(uint32_t *readings);
} ;

#endif /* USB_WEATHER_H_ */
