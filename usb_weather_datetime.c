/*
	USB_WEATHER_DATETIME.C
	----------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#include <stdio.h>
#include "usb_weather_datetime.h"

/*
	USB_WEATHER_DATETIME::BCD_TO_INT()
	----------------------------------
*/
uint8_t usb_weather_datetime::bcd_to_int(uint8_t bcd)
{
return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/*
	USB_WEATHER_DATETIME::EXTRACT()
	-------------------------------
*/
void usb_weather_datetime::extract(uint8_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute)
{
*year = bcd_to_int(this->year);
*month = bcd_to_int(this->month);
*day = bcd_to_int(this->day);
*hour = bcd_to_int(this->hour);
*minute = bcd_to_int(this->minute);
}

/*
	USB_WEATHER_DATETIME::PRINT_BCD()
	---------------------------------
*/
void usb_weather_datetime::print_bcd(uint8_t bcd)
{
printf("%d", bcd >> 4);
printf("%d", bcd & 0x0F);
}

/*
	USB_WEATHER_DATETIME::TEXT_RENDER()
	-----------------------------------
*/
void usb_weather_datetime::text_render(void)
{
print_bcd(hour);
printf(":");
print_bcd(minute);
printf(" on ");
print_bcd(day);
printf("/");
print_bcd(month);
printf("/20");
print_bcd(year);
}
