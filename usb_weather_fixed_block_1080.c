/*
	USB_WEATHER_FIXED_BLOCK_1080.C
	------------------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#include <stdio.h>
#include "usb_weather_fixed_block_1080.h"

/*
	USB_WEATHER_FIXED_BLOCK_1080::PRINT_DATA()
	------------------------------------------
*/
void usb_weather_fixed_block_1080::print_data(const char *feature, const char *units, double min, usb_weather_datetime min_when, double alarm_low, double max, usb_weather_datetime max_when, double alarm_high, const char *spaces)
{
printf("Minimum %s   %s:%0.2f%s ", feature, spaces, min, units);
min_when.text_render();
puts("");
printf("Alarm %s low %s:%0.2f%s\n", feature, spaces, alarm_low, units);

printf("Maximum %s   %s:%02f%s", feature, spaces, max, units);
max_when.text_render();
puts("");
printf("Alarm %s high%s:%0.2f%s\n", feature, spaces, alarm_high, units);
}

/*
	USB_WEATHER_FIXED_BLOCK_1080::PRINT_DATA()
	------------------------------------------
*/
void usb_weather_fixed_block_1080::print_data(const char *feature, const char *units, double value,  usb_weather_datetime when)
{
printf("%s:%0.2f%s ", feature, value, units);
when.text_render();
printf("\n");
}

/*
	USB_WEATHER_FIXED_BLOCK_1080::TOGGLE()
	--------------------------------------
*/
const char *usb_weather_fixed_block_1080::toggle(uint16_t value, uint16_t bit)
{
static const char *on = "On";
static const char *off = "Off";

return (value & 1 << bit) ? on : off;
}

/*
	USB_WEATHER_FIXED_BLOCK_1080::TEXT_RENDER()
	-------------------------------------------
*/
void usb_weather_fixed_block_1080::text_render(void)
{
puts("GENERAL");
puts("-------");
printf("EEPROM init code   :%04X (should be AA55)\n", eeprom_init);
printf("Read Period        :%d minutes\n", read_period);
printf("Timezone           :%d hours from CET\n", timezone);
printf("Valid readings     :%d\n", data_count);
printf("Most recent reading:0x%04X\n", current_position);
printf("Current time       :");
current_time.text_render();
printf("\n");
printf("Alarm time         :");
usb_weather_datetime::print_bcd(alarm_time >> 8);
printf(":");
usb_weather_datetime::print_bcd(alarm_time & 0xFF);
puts("");

puts("\nALARMS");
puts("------");
printf("Time                    :%s\n", toggle(alarm_enable_1, 1));
printf("Wind direction          :%s\n", toggle(alarm_enable_1, 2));
printf("Indoor humidity low     :%s\n", toggle(alarm_enable_1, 4));
printf("Indoor humidity high    :%s\n", toggle(alarm_enable_1, 5));
printf("Outdoor humidity low    :%s\n", toggle(alarm_enable_1, 6));
printf("Outdoor humidity high   :%s\n", toggle(alarm_enable_1, 7));
printf("Wind average            :%s\n", toggle(alarm_enable_2, 0));
printf("Wind gust               :%s\n", toggle(alarm_enable_2, 1));
printf("Rain hourly             :%s\n", toggle(alarm_enable_2, 2));
printf("Rain daily              :%s\n", toggle(alarm_enable_2, 3));
printf("Absolute pressure low   :%s\n", toggle(alarm_enable_2, 4));
printf("Absolute pressure high  :%s\n", toggle(alarm_enable_2, 5));
printf("Relative pressure low   :%s\n", toggle(alarm_enable_2, 6));
printf("Relative pressure high  :%s\n", toggle(alarm_enable_2, 7));
printf("Indoor temperature low  :%s\n", toggle(alarm_enable_3, 0));
printf("Indoor temperature high :%s\n", toggle(alarm_enable_3, 1));
printf("Outdoor temperature low :%s\n", toggle(alarm_enable_3, 2));
printf("Outdoor temperature high:%s\n", toggle(alarm_enable_3, 3));
printf("Wind chill low          :%s\n", toggle(alarm_enable_3, 4));
printf("Wind chill high         :%s\n", toggle(alarm_enable_3, 5));
printf("Dew point low           :%s\n", toggle(alarm_enable_3, 6));
printf("Dew point high          :%s\n", toggle(alarm_enable_3, 7));

puts("\nBASE UNIT DISPLAY OPTIONS");
puts("-------------------------");
printf("Pressure           :%s\n", (display_options_1 & (1 << 0)) ? "Relative" : "Absolute");
printf("Windspeed          :%s\n", (display_options_1 & (1 << 1)) ? "Gust" : "Average");
printf("Time               :%s\n", (display_options_1 & (1 << 2)) ? "12 hour" : "24 Hour");
printf("Date               :%s\n", (display_options_1 & (1 << 3)) ? "MDY" : "DMY");
printf("Time scale         :%s\n", (display_options_1 & (1 << 4)) ? "24 Hour" : "12 Hour");
printf("Show year          :%s\n", toggle(display_options_1, 5));
printf("Show day name      :%s\n", toggle(display_options_1, 6));
printf("Show alarm time    :%s\n", toggle(display_options_1, 7));

printf("Outdoor temperature:%s\n",
	(display_options_2 & (1 << 0)) ? "Temperature" :
	(display_options_2 & (1 << 1)) ? "Wind chill" :
	(display_options_2 & (1 << 2)) ? "Dew point" : "");

printf("Rain               :%s\n",
	(display_options_2 & (1 << 3)) ? "Hour" :
	(display_options_2 & (1 << 4)) ? "Day" :
	(display_options_2 & (1 << 5)) ? "Month" :
	(display_options_2 & (1 << 6)) ? "Week" :
	(display_options_2 & (1 << 7)) ? "Total" : "");

printf("Indoor temperature :%s\n", (unit_settings_1 & (1 << 0)) ? "F" : "C");
printf("Outdoor temperature:%s\n", (unit_settings_1 & (1 << 1)) ? "F" : "C");
printf("Rain               :%s\n", (unit_settings_1 & (1 << 2)) ? "Inch" : "mm");

printf("Pressure           :%s\n",
	(unit_settings_1 & (1 << 5)) ? "hPa" :
	(unit_settings_1 & (1 << 6)) ? "inHg" :
	(unit_settings_1 & (1 << 7)) ? "mmHg" : "");
	
printf("Wind speed         :%s\n",
	(unit_settings_2 & (1 << 0)) ? "m/s" :
	(unit_settings_2 & (1 << 1)) ? "km/h" :
	(unit_settings_2 & (1 << 2)) ? "knot" :
	(unit_settings_2 & (1 << 3)) ? "m/h" :
	(unit_settings_2 & (1 << 4)) ? "btf" : "");


puts("\nTEMPERATURE");
puts("-----------");
print_data("indoor temperature", "C", minimum_indoor_temperature / 10, minimum_indoor_temperature_when, alarm_indoor_temperature_low / 10, maximum_indoor_temperature / 10, maximum_indoor_temperature_when, alarm_indoor_temperature_high / 10, "   ");
print_data("outdoor temperature", "C", minimum_outdoor_temperature / 10, minimum_outdoor_temperature_when, alarm_outdoor_temperature_low / 10, maximum_outdoor_temperature / 10, maximum_outdoor_temperature_when, alarm_outdoor_temperature_high / 10, "  ");
print_data("windchill temperature", "C", minimum_windchill_temperature / 10, minimum_windchill_temperature_when, alarm_windchill_temperature_low / 10, maximum_windchill_temperature / 10, maximum_windchill_temperature_when, alarm_windchill_temperature_high / 10);
print_data("dewpoint temperature", "C", minimum_dewpoint_temperature / 10, minimum_dewpoint_temperature_when, alarm_dewpoint_temperature_low / 10, maximum_dewpoint_temperature / 10, maximum_dewpoint_temperature_when, alarm_dewpoint_temperature_high / 10, " ");

puts("\nHUMIDITY");
puts("--------");
print_data("indoor humidity", "%", minimum_indoor_humidity, minimum_indoor_humidity_when, alarm_indoor_humidity_low, maximum_indoor_humidity, maximum_indoor_humidity_when, alarm_indoor_humidity_high, " ");
print_data("outdoor humidity", "%", minimum_outdoor_humidity, minimum_outdoor_humidity_when, alarm_outdoor_humidity_low, maximum_outdoor_humidity, maximum_outdoor_humidity_when, alarm_outdoor_humidity_high);

puts("\nPRESSURE");
puts("--------");
printf("Relative pressure           :%0.2f hPa\n", relative_pressure / 10.0);
printf("Absolute pressure           :%0.2f hPa\n", absolute_pressure / 10.0);
print_data("absolute pressure", "hPa", minimum_absolute_pressure / 10, minimum_absolute_pressure_when, alarm_absolute_pressure_low / 10, maximum_absolute_pressure / 10, maximum_absolute_pressure_when, alarm_absolute_pressure_high / 10);
print_data("relative pressure", "hPa", minimum_relative_pressure / 10, minimum_relative_pressure_when, alarm_relative_pressure_low / 10, maximum_relative_pressure / 10, maximum_relative_pressure_when, alarm_relative_pressure_high / 10);

puts("\nRAINFALL");
puts("--------");
printf("Maximum hourly :%0.2fmm ", maximum_rain_hourly / 10.0);
maximum_rain_hourly_when.text_render();
puts("");

printf("Maximum daily  :%0.2fmm ", maximum_rain_daily / 10.0);
maximum_rain_daily_when.text_render();
puts("");

printf("Maximum weekly :%0.2fmm ", maximum_rain_weekly / 10.0);
maximum_rain_weekly_when.text_render();
puts("");

printf("Maximum monthly:%0.2fmm ", (((high_nibble_rainfall >> 4) << 16) | maximum_rain_monthly) / 10.0);
maximum_rain_monthly_when.text_render();
puts("");

printf("Maximum total  :%0.2fmm ", ((high_nibble_rainfall & 0x0F) << 16 | maximum_rain_total) / 10.0);
maximum_rain_total_when.text_render();
puts("");

printf("Alarm hourly   :%0.2fmm\n", alarm_rain_hourly);
printf("Alarm daily    :%0.2fmm\n", alarm_rain_daily);

puts("\nWIND");
puts("----");
print_data("Maximum Average windspeed", "m/s", maximum_average_windspeed / 10, maximum_average_windspeed_when);
print_data("Maximum Gust windspeed   ", "m/s", maximum_gust_windspeed / 10, maximum_gust_windspeed_when);
printf("Alarm average Beaufort   :%d\n", alarm_average_windspeed_beaufort);
printf("Alarm average windspeed  :%0.2fm/s\n", alarm_average_windspeed_ms / 10.0);
printf("Alarm gust Beaufort      :%d\n", alarm_gust_windspeed_beaufort);
printf("Alarm gust windspeed     :%0.2fm/s\n", alarm_gust_windspeed_ms / 10.0);
printf("Alarm wind direction     :%0.2f degrees\n", alarm_wind_direction * 22.5);
}
