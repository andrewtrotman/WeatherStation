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
	enum {NO_WIND = -1}; // for when there is no wind

protected:
	static float calculate_sunrise_sunset(int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings, int rise_or_set = SUNRISE);
	static long julday(long year, long month, long day);

public:
	static void sunrise(int *hour, int *min, int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings);
	static void sunset(int *hour, int *min, int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings);
	static double c_to_f(double temperature_in_c);
	static double f_to_c(double temperature_in_f);
	static double weather_math::c_to_k(double temperature_in_c);
	static double dewpoint(double temperature_in_c, double humidity);
	static double windchill(double temperature_in_c, double windspeed_in_ms);
	static double australian_apparent_temperature(double temperature_in_c, double humidity, double wind_in_ms);
	static double heat_index(double temperature_in_c, double humidity);
	static double apparent_temperature(double temperature_in_c, double humidity, double windspeed_in_ms);
	static double cloud_base(double temperature_in_c, double humidity);
	static double cloud_temperature(double temperature_in_c, double humidity);
	static long beaufort(double windspeed_in_ms);
	static const char *beaufort_name(long beaufort);
	static long zambretti(double z_hpa, long z_month, long z_wind, long z_trend, long z_north = true, double z_baro_top = 1050, double z_baro_bottom = 950);
	static long zambretti_pywws(double z_hpa, long z_month, long z_wind, long z_trend, long z_north = true, double z_baro_top = 1050, double z_baro_bottom = 950);
	static const char *zambretti_name(long number);
	static long pressure_trend(double hourly_pressure_change_in_hPa);
	static const char *pressure_trend_name(long change);
	static long phase_of_moon(long year, long month, long day);
	static double knots(double m_per_second);
	static long is_daylight_saving(void);		// return true if humans are currently in daylight savings "mode"
	static double pressure_to_sea_level_pressure(double pressure_in_hpa, double temperature_in_c, double height_above_sea_level_in_m);
	static const char *wind_direction_name(double angle);
};

#endif /* WEATHER_MATH_H_ */

