/*
	WEATHER_MATH.H
	--------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef WEATHER_MATH_H_
#define WEATHER_MATH_H_

/*
	class WEATHER_MATH
	------------------
*/
class weather_math
{
protected:
	enum {SUNRISE = 0, SUNSET = 1};

public:
	enum {RISE = 1, STEADY = 0, FALL = 2};		// barometric state, used for Zambretti

protected:
	static float calculate_sunrise_sunset(int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings, int rise_or_set = SUNRISE);

public:
	static void sunrise(int *hour, int *min, int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings);
	static void sunset(int *hour, int *min, int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings);
	static double c_to_f(double temperature_in_c);
	static double f_to_c(double temperature_in_f);
	static double dewpoint(double temperature_in_c, double humidity);
	static double windchill(double temperature_in_c, double windspeed_in_ms);
	static double australian_apparent_temperature(double temperature_in_c, double humidity, double wind_in_ms);
	static double heat_index(double temperature_in_c, double humidity);
	static double apparent_temperature(double temperature_in_c, double humidity, double windspeed_in_ms);
	static double cloud_base(double temperature_in_c, double humidity);
	static double cloud_temperature(double temperature_in_c, double humidity);
	static long beaufort(double windspeed_in_ms);
	static char *weather_math::beaufort_name(long beaufort);
	static long zambretti(double z_hpa, long z_month, long z_wind, long z_trend, long z_north = true, double z_baro_top = 1050.0, double z_baro_bottom = 950.0);
	static char *weather_math::zambretti_name(long number);
};

#endif /* WEATHER_MATH_H_ */
