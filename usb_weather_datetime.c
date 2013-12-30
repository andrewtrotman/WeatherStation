/*
	USB_WEATHER_DATETIME.C
	----------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#include <stdio.h>
#include "usb_weather_datetime.h"

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
