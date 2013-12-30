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
double speed;

puts(title);

printf("Time since last recording:%d mins\n", delay);
printf("Indoor Temperature       :%2.2fC\n", indoor_temperature / 10.0);
printf("Indoor Humidity          :%d%%\n", indoor_humidity);
printf("Pressure                 :%0.2fhPa\n", absolute_pressure / 10.0);

if (!(status & 0x40))		// if status & 0x40 then we've lost contact with the sensors
	{
	printf("Outdoor Humidity         :% %%\n", outdoor_humidity);
	printf("Outdoor Temperature      :%0.2fC\n", outdoor_temperature / 10.0);
	speed = average_windspeed_low | (windspeed_high & 0x0F);
	printf("Average windspeed        :%0.2fm/s\n", speed / 10.0);
	speed = gust_windspeed_low | ((windspeed_high >> 4) & 0x0F);
	printf("Gust windspeed           :%0.2fm/s\n", speed / 10.0);
	printf("Wind direction           :%0.2f degrees from North\n", wind_direction * 22.5);
	printf("Total Rain               :%0.2fmm\n", total_rain * 0.3);
#ifdef WH3080
	printf("Illuminance              ;%0.2f lux\n", ((light_high << 16) + (light << 8) + light_low) * 0.1);
	printf("UV                       :%d\n", uv);
#endif
	}

if (status & 0x80)
	printf("Rain counter overflow\n");
}

