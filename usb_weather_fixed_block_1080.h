/*
	USB_WEATHER_FIXED_BLOCK_1080.H
	------------------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef USB_WEATHER_FIXED_BLOCK_1080_H_
#define USB_WEATHER_FIXED_BLOCK_1080_H_

#include "fundamental_types.h"
#include "usb_weather_datetime.h"

#pragma pack(1)

/*
	class USB_WEATHER_FIXED_BLOCK_1080
	----------------------------------
*/
class usb_weather_fixed_block_1080
{
public:
	uint16_t eeprom_init;					// should be 0xAA55
	uint8_t  reserved_1[14];
	uint8_t  read_period;					// minutes between each stored reading
	uint8_t  unit_settings_1;				// bit 0: indoor temperature: 0 = C, 1 = F | bit 1: outdoor temperature: 0 = C, 1 = F | bit 2: rain: 0 = mm, 1 = inch | bit 5: pressure: 1 = hPa | bit 6: pressure: 1 = inHg | bit 7: pressure: 1 = mmHg
	uint8_t  unit_settings_2;				// bit 0: wind speed: 1 = m/s | bit 1: wind speed: 1 = km/h | bit 2: wind speed: 1 = knot | bit 3: wind speed: 1 = m/h | bit 4: wind speed: 1 = bft
	uint8_t  display_options_1;				// bit 0: pressure: 0 = absolute, 1 = relative | bit 1: wind speed: 0 = average, 1 = gust | bit 2: time: 0 = 24 hour, 1 = 12 hour | bit 3: date: 0 = day-month-year, 1 = month-day-year | bit 4: time scale(?): 0 = 12 hour, 1 = 24 hour | bit 5: date: 1 = show year year | bit 6: date: 1 = show day name | bit 7: date: 1 = alarm time
	uint8_t  display_options_2;				// bit 0: outdoor temperature: 1 = temperature | bit 1: outdoor temperature: 1 = wind chill | bit 2: outdoor temperature: 1 = dew point | bit 3: rain: 1 = hour | bit 4: rain: 1 = day | bit 5: rain: 1 = week | bit 6: rain: 1 = month | bit 7: rain: 1 = total
	uint8_t  alarm_enable_1;				// bit 1: time | bit 2: wind direction | bit 4: indoor humidity low | bit 5: indoor humidity high | bit 6: outdoor humidity low | bit 7: outdoor humidity high
	uint8_t  alarm_enable_2;				// bit 0: wind average | bit 1: wind gust | bit 2: rain hourly | bit 3: rain daily | bit 4: absolute pressure low | bit 5: absolute pressure high | bit 6: relative pressure low | bit 7: relative pressure high
	uint8_t  alarm_enable_3;				// bit 0: indoor temperature low | bit 1: indoor temperature high | bit 2: outdoor temperature low | bit 3: outdoor temperature high | bit 4: wind chill low | bit 5: wind chill high | bit 6: dew point low | bit 7: dew point high
	int8_t   timezone;						// hours offset from Central European Time (in BCD)
	uint8_t  reserved_2;
	uint8_t  data_refresh;					// Computer writes 0xAA to indicate a change of settings. Weather station clears value to acknowledge.
	uint16_t data_count;					// number of stored readings
	uint8_t reserved_3;
	uint16_t current_position;				// Address of the stored reading currently being created. Starts at 256, rises to 65520 in steps of 16 (or 65516 in steps of 20 in the 3080), then loops back to 256. The data at this address is updated every 48 seconds or so, until the read period is reached. Then the address is incremented and the next record becomes current.
	uint16_t relative_pressure;				// Current relative (sea level) atmospheric pressure, multiply by 0.1 to get hPa.
	uint16_t absolute_pressure;				// Current absolute atmospheric pressure, multiply by 0.1 to get hPa.
	uint8_t reserved_4[7];
	usb_weather_datetime current_time;
	uint8_t  alarm_indoor_humidity_high;
	uint8_t  alarm_indoor_humidity_low;
	uint16_t alarm_indoor_temperature_high;	// Multiply by 0.1 to get C.
	uint16_t alarm_indoor_temperature_low;	// Multiply by 0.1 to get C.
	uint8_t  alarm_outdoor_humidity_high;
	uint8_t  alarm_outdoor_humidity_low;
	uint16_t alarm_outdoor_temperature_high;// Multiply by 0.1 to get C.
	uint16_t alarm_outdoor_temperature_low;	// Multiply by 0.1 to get C.
	uint16_t alarm_windchill_temperature_high;			// Multiply by 0.1 to get C.
	uint16_t alarm_windchill_temperature_low;			// Multiply by 0.1 to get C.
	uint16_t alarm_dewpoint_temperature_high;			// Multiply by 0.1 to get C.
	uint16_t alarm_dewpoint_temperature_low;			// Multiply by 0.1 to get C.
	uint16_t alarm_absolute_pressure_high;	// Multiply by 0.1 to get hPa.
	uint16_t alarm_absolute_pressure_low;	// Multiply by 0.1 to get hPa.
	uint16_t alarm_relative_pressure_high;	// Multiply by 0.1 to get hPa.
	uint16_t alarm_relative_pressure_low;	// Multiply by 0.1 to get hPa.
	uint8_t	 alarm_average_windspeed_beaufort;
	uint16_t alarm_average_windspeed_ms;	// Multiply by 0.1 to get m/s.
	uint8_t  alarm_gust_windspeed_beaufort;
	uint16_t alarm_gust_windspeed_ms;		// Multiply by 0.1 to get m/s.
	uint8_t  alarm_wind_direction;			// Multiply by 22.5 to get degrees from north.
	uint16_t alarm_rain_hourly;				// Multiply by 0.1 to get mm.
	uint16_t alarm_rain_daily;				// Multiply by 0.1 to get mm.
	uint16_t alarm_time;					// BCD Hour & minute.
	uint8_t reserved_5[9];
	uint8_t maximum_indoor_humidity;
	uint8_t minimum_indoor_humidity;
	uint8_t maximum_outdoor_humidity;
	uint8_t minimum_outdoor_humidity;
	uint16_t maximum_indoor_temperature;	// Multiply by 0.1 to get C.
	uint16_t minimum_indoor_temperature;	// Multiply by 0.1 to get C.
	uint16_t maximum_outdoor_temperature;	// Multiply by 0.1 to get C.
	uint16_t minimum_outdoor_temperature;	// Multiply by 0.1 to get C.
	uint16_t maximum_windchill_temperature;	// Multiply by 0.1 to get C.
	uint16_t minimum_windchill_temperature;	// Multiply by 0.1 to get C.
	uint16_t maximum_dewpoint_temperature;	// Multiply by 0.1 to get C.
	uint16_t minimum_dewpoint_temperature;	// Multiply by 0.1 to get C.
	uint16_t maximum_absolute_pressure;		// Multiply by 0.1 to get hPa.
	uint16_t minimum_absolute_pressure;		// Multiply by 0.1 to get hPa.
	uint16_t maximum_relative_pressure;		// Multiply by 0.1 to get hPa.
	uint16_t minimum_relative_pressure;		// Multiply by 0.1 to get hPa.
	uint16_t maximum_average_windspeed;		// Multiply by 0.1 to get m/s.
	uint16_t maximum_gust_windspeed;		// Multiply by 0.1 to get m/s.
	uint16_t maximum_rain_hourly;			// Multiply by 0.1 to get mm.
	uint16_t maximum_rain_daily;			// Multiply by 0.1 to get mm.
	uint16_t maximum_rain_weekly;			// Multiply by 0.1 to get mm.
	uint16_t maximum_rain_monthly;			// Multiply by 0.1 to get mm.
	uint16_t maximum_rain_total;			// Multiply by 0.1 to get mm.
	uint8_t high_nibble_rainfall;			// high nibble - month rainfall, low nibble - total rainfall
	usb_weather_datetime maximum_indoor_humidity_when;
	usb_weather_datetime minimum_indoor_humidity_when;
	usb_weather_datetime maximum_outdoor_humidity_when;
	usb_weather_datetime minimum_outdoor_humidity_when;
	usb_weather_datetime maximum_indoor_temperature_when;
	usb_weather_datetime minimum_indoor_temperature_when;
	usb_weather_datetime maximum_outdoor_temperature_when;
	usb_weather_datetime minimum_outdoor_temperature_when;
	usb_weather_datetime maximum_windchill_temperature_when;
	usb_weather_datetime minimum_windchill_temperature_when;
	usb_weather_datetime maximum_dewpoint_temperature_when;
	usb_weather_datetime minimum_dewpoint_temperature_when;
	usb_weather_datetime maximum_absolute_pressure_when;
	usb_weather_datetime minimum_absolute_pressure_when;
	usb_weather_datetime maximum_relative_pressure_when;
	usb_weather_datetime minimum_relative_pressure_when;
	usb_weather_datetime maximum_average_windspeed_when;
	usb_weather_datetime maximum_gust_windspeed_when;
	usb_weather_datetime maximum_rain_hourly_when;
	usb_weather_datetime maximum_rain_daily_when;
	usb_weather_datetime maximum_rain_weekly_when;
	usb_weather_datetime maximum_rain_monthly_when;
	usb_weather_datetime maximum_rain_total_when;

private:
	void print_data(const char *feature, const char *units, double min, usb_weather_datetime min_when, double alarm_low, double max, usb_weather_datetime max_when, double alarm_high, const char *spaces = "");
	void print_data(const char *feature, const char *units, double value,  usb_weather_datetime when);
	const char *toggle(uint16_t value, uint16_t bit);
	
public:
	void text_render(void);
} ;
#pragma pack()

#endif /* USB_WEATHER_FIXED_BLOCK_1080_H_ */
