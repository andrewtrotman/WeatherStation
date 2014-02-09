/*
	USB_WEATHER_CACHE.C
	-------------------
	Copyright (c) 2014 Andrew Trotman
	Licensed BSD
*/

#include <stdio.h>
#include <string.h>
#include "usb_weather_cache.h"

/*
	USB_WEATHER_CACHE::USB_WEATHER_CACHE()
	--------------------------------------
*/
usb_weather_cache::usb_weather_cache()
{
memset(memory_map, 0, sizeof(memory_map));
memset(have_read, 0, sizeof(have_read));
}

/*
	USB_WEATHER_CACHE::READ()
	-------------------------
*/
uint32_t usb_weather_cache::read(uint16_t address, void *result)
{
uint32_t got, here, sum = 0;
uint8_t buffer[32];

for (here = address; here < (uint32_t)address + 32; here++)
	sum += have_read[here];

if (sum == 32)
	{
	/*
		We've done this read before, so get the result out of the cache
	*/
	memcpy(result, memory_map + address, 32);
	return 32;
	}
else if (sum == 16)
	{
	/*
		We've done half this read before so adjust it to read all new material
	*/
	if (have_read[address])
		read(address + 16, buffer);
	else
		read(address - 16, buffer);

	return read(address, result);
	}
else
	{
	/*
		Entirely new read
	*/
	got = usb_weather::read(address, result);
	memcpy(memory_map + address, result, 32);
	memset(have_read + address, 1, 32);

	return got;
	}
}
