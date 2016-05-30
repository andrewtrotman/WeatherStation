/*
	USB_WEATHER_DATETIME.H
	----------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef USB_WEATHER_DATETIME_H_
#define USB_WEATHER_DATETIME_H_

#include <iostream>
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
	void extract(uint8_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute) const;
	void text_render(void);
} ;

/*
	OPERATOR<<()
	------------
*/
inline std::ostream& operator<<(std::ostream& stream, const usb_weather_datetime &object)
{
uint8_t year, month, day, hour, minute;

object.extract(&year, &month, &day, &hour, &minute);

stream << (int)hour << ':' << (int)minute << " on " << (int)day << "/" << (int)month << "/20" << (int)year;

return stream;
}
#pragma pack()

#endif /* USB_WEATHER_DATETIME_H_ */
