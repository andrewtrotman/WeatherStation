/*
	USB_WEATHER_CACHE.H
	-------------------
	Copyright (c) 2014 Andrew Trotman
	Licensed BSD
*/
#ifndef USB_WEATHER_CACHE_H_
#define USB_WEATHER_CACHE_H_

#include "usb_weather.h"

/*
	class USB_WEATHER_CACHE
	-----------------------
*/
class usb_weather_cache : public usb_weather
{
private:
	uint8_t memory_map[0x10000];
	uint8_t have_read[0x10000];

public:
	usb_weather_cache();
	virtual ~usb_weather_cache() {}

	virtual uint32_t read(uint16_t address, void *result);
};

#endif /* USB_WEATHER_CACHE_H_ */
