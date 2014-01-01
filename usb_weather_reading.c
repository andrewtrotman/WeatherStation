/*
	USB_WEATHER_READING.C
	---------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#include <stdio.h>
#include "usb_weather_reading.h"

/*
	DUMP_WEATHER()
	--------------
*/
void usb_weather_reading::text_render(char *title)
{
puts(title);

printf("Time since last recording:%d mins\n", delay);
printf("Indoor Temperature       :%2.2fC\n", indoor_temperature);
printf("Indoor Humidity          :%d%%\n", indoor_humidity);
printf("Pressure                 :%0.2fhPa\n", absolute_pressure);

if (!lost_communications)
	{
	printf("Outdoor Humidity         :% %%\n", outdoor_humidity);
	printf("Outdoor Temperature      :%0.2fC\n", outdoor_temperature);
	printf("Average windspeed        :%0.2fm/s\n", average_windspeed);
	printf("Gust windspeed           :%0.2fm/s\n", gust_windspeed);
	printf("Wind direction           :%0.2f degrees from North\n", wind_direction);
	printf("Total Rain               :%0.2fmm\n", total_rain);
	}

if (rain_counter_overflow)
	printf("Rain counter overflow\n");
}

