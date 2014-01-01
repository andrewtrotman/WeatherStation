/*
	USB_WEATHER_DATETIME.H
	----------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef USB_WEATHER_DATETIME_H_
#define USB_WEATHER_DATETIME_H_

#include "fundamental_types.h"

/*
	class USB_WEATHER_DATETIME
	--------------------------
*/
#pragma pack(1)
class usb_weather_datetime
{
public:
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;

public:
	static void print_bcd(uint8_t bcd);
	static uint8_t bcd_to_int(uint8_t bcd);
	void extract(uint8_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute);
	void text_render(void);
} ;
#pragma pack()

#endif /* USB_WEATHER_DATETIME_H_ */
