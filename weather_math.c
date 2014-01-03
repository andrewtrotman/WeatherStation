/*
	WEATHER_MATH.C
	--------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#include <math.h>
#include "weather_math.h"


/*
	WEATHER_MATH::C_TO_F()
	----------------------
*/
double weather_math::c_to_f(double temperature_in_c)
{
return temperature_in_c * 9.0 / 5.0 + 32.0;
}

/*
	WEATHER_MATH::F_TO_C()
	----------------------
*/
double weather_math::f_to_c(double temperature_in_f)
{
return (temperature_in_f - 32.0) * 5.0 / 9.0;
}

/*
	WEATHER_MATH::DEWPOINT()
	------------------------
	from: http://wiki.crowe.co.nz/DewPoint%20Calculation.ashx
	which got it from "The Davis Vantage Pro Technicial Reference" which states:

	v = RH*0.01*6.112 * exp [(17.62*T)/(T + 243.12)],
	this equation will provide the vapor pressure value (in pressure units) where T is the air temperature in C
	and RH is the relative humidity.
	Now dewpoint, Td, can be found:
	Numerator = 243.12*(ln v) – 440.1
	Denominator = 19.43 – ln v
	Td = Numerator/Denominator

*/
double weather_math::dewpoint(double temperature_in_c, double humidity)
{
double vapour_pressure = humidity * 0.01 * 6.112 * exp((17.62 * temperature_in_c) / (temperature_in_c + 243.12)); 
double numerator = 243.12 * log(vapour_pressure) - 440.1; 
double denominator = 19.43 - (log(vapour_pressure)); 

return numerator / denominator;
}

/*
	WEATHER_MATH::WINDCHILL()
	-------------------------
	From "The Davis Vantage Pro Technicial Reference".  But using the Wikipedia version (as its in SI units)
	Tw = 13.12 + 0.6215 * T - 11.37 * v^0.16 + 0.3965 * T * v^0.16
	where T is in C and V is in km/h

	but, its only valid when the temperature is less then 10 C and the windspeed is greater than 4.8 km/h
*/
double weather_math::windchill(double temperature_in_c, double windspeed_in_ms)
{
double windspeed = windspeed_in_ms * 3.6;

if (temperature_in_c > 10)
	return temperature_in_c;
if (windspeed > 4.8)
	return 13.12 + 0.6215 * temperature_in_c - 11.37 * pow(windspeed, 0.16) + 0.3965 * temperature_in_c * pow(windspeed, 0.16);
else
	return temperature_in_c;
}


/*
	WEATHER_MATH::AUSTRALIAN_APPARENT_TEMPERATURE()
	-----------------------------------------------
*/
double weather_math::australian_apparent_temperature(double temperature_in_c, double humidity, double windspeed_in_ms)
{
double vapour_pressure;

vapour_pressure = (humidity / 100) * 6.105 * exp((17.27 * temperature_in_c) / (237.7 + temperature_in_c));

return temperature_in_c + 0.33 * vapour_pressure - 0.70 * windspeed_in_ms - 4.00;
}


/*
	WEATHER_MATH::HEAT_INDEX()
	--------------------------
	There are pleanty of sources, including Wikipedia article "
*/
double weather_math::heat_index(double temperature_in_c, double humidity)
{
double Tf = c_to_f(temperature_in_c);
double RH = humidity;
double hi;

hi =  -42.379 + 2.0490 * Tf + 10.143 * RH - 0.22476 * Tf * RH - 0.0068378 * Tf * Tf - 0.054817 * RH * RH + 0.0012287 * Tf * Tf * RH + 0.00085282 * RH * RH * Tf - 0.00000199 * Tf * Tf * RH * RH;

return f_to_c(hi);
}

/*
	WEATHER_MATH::APPARENT_TEMPERATURE()
	------------------------------------
	according to: http://www.nws.noaa.gov/ndfd/definitions.htm
	we use the Heat Index when the temperature is above 80F (26.7C)
	we use the ambient air temperature beteeen 80F (26.7C) and 50F (10C)
	we use the wind chill is the temperature is at or below 50F (10C)
*/
double weather_math::apparent_temperature(double temperature_in_c, double humidity, double windspeed_in_ms)
{
if (temperature_in_c < 10)
	return windchill(temperature_in_c, windspeed_in_ms);
else if (temperature_in_c < 26.7)
	return temperature_in_c;
else
	return heat_index(temperature_in_c, humidity);
}

/*
	WEATHER_MATH::CLOUD_BASE()
	--------------------------
	return the height of the clouds as meters above ground level
*/
double weather_math::cloud_base(double temperature_in_c, double humidity)
{
return (temperature_in_c - dewpoint(temperature_in_c, humidity)) / 0.00802;
}

/*
	WEATHER_MATH::CLOUD_TEMPERATURE()
	---------------------------------
	return the temperature of the clouds
*/
double weather_math::cloud_temperature(double temperature_in_c, double humidity)
{
return -0.00984 * cloud_base(temperature_in_c, humidity) + temperature_in_c;
}

/*
	WEATHER_MATH::BEAUFORT()
	------------------------
*/
long weather_math::beaufort(double windspeed_in_ms)
{
if (windspeed_in_ms < 0.3)
	return 0;
if (windspeed_in_ms < 1.5)
	return 1;
if (windspeed_in_ms < 3.4)
	return 2;
if (windspeed_in_ms < 5.4)
	return 3;
if (windspeed_in_ms < 7.9)
	return 4;
if (windspeed_in_ms < 10.7)
	return 5;
if (windspeed_in_ms < 13.8)
	return 6;
if (windspeed_in_ms < 17.1)
	return 7;
if (windspeed_in_ms < 20.7)
	return 8;
if (windspeed_in_ms < 24.4)
	return 9;
if (windspeed_in_ms < 28.4)
	return 10;
if (windspeed_in_ms < 32.6)
	return 11;

return 12;
}

/*
	WEATHER_MATH::BEAUFORT_NAME()
	-----------------------------
*/
char *weather_math::beaufort_name(long beaufort)
{
static char *message[] =
	{
	"Calm",
	"Light Air",
	"Light Breeze",
	"Gentle Breeze",
	"Moderate Breeze",
	"Fresh Breeze",
	"Strong Breeze",
	"Moderate Gale",
	"Gale",
	"Strong Gale",
	"Storm",
	"Voilent Storm",
	"Hurricane"
	};

return message[beaufort];
}

/*
	WEATHER_MATH::CALCULATE_SUNRISE_SUNSET()
	----------------------------------------
	Original (calculateSunrise()) from:
		http://stackoverflow.com/questions/7064531/sunrise-sunset-times-in-c
	With changes to optionally compute sunrise and sunset
*/
float weather_math::calculate_sunrise_sunset(int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings, int rise_or_set)
{
static const float PI = 3.1415926;
static const float ZENITH =  -0.83;

/*
	localOffset will be <0 for western hemisphere and >0 for eastern hemisphere
	daylightSavings should be 1 if it is in effect during the summer otherwise it should be 0
*/

//1. first calculate the day of the year
float N1 = floor(275 * month / 9.0);
float N2 = floor((month + 9) / 12.0);
float N3 = (1 + floor((year - 4 * floor(year / 4.0) + 2) / 3.0));
float N = N1 - (N2 * N3) + day - 30;

//2. convert the longitude to hour value and calculate an approximate time
float lngHour = lng / 15.0;   

float t;
if (!rise_or_set)
	t = N + ((6 - lngHour) / 24);  //if rising time is desired:
else
	t = N + ((18 - lngHour) / 24);  //if setting time is desired:

//3. calculate the Sun's mean anomaly  
float M = (0.9856 * t) - 3.289;

//4. calculate the Sun's true longitude
float L = fmod(M + (1.916 * sin((PI / 180) * M)) + (0.020 * sin(2 *(PI / 180) * M)) + 282.634, 360.0);

//5a. calculate the Sun's right ascension   
float RA = fmod(180 / PI * atan(0.91764 * tan((PI / 180) * L)), 360.0);

//5b. right ascension value needs to be in the same quadrant as L  
float Lquadrant = floor(L / 90) * 90;
float RAquadrant = floor(RA / 90) * 90;
RA = RA + (Lquadrant - RAquadrant);

//5c. right ascension value needs to be converted into hours  
RA = RA / 15;

//6. calculate the Sun's declination
float sinDec = 0.39782 * sin((PI / 180) * L);
float cosDec = cos(asin(sinDec));

//7a. calculate the Sun's local hour angle
float cosH = (sin((PI /180) * ZENITH) - (sinDec * sin((PI / 180) * lat))) / (cosDec * cos((PI / 180) * lat));

/*  
	if (cosH > 1) 
		the sun never rises on this location (on the specified date)
	if (cosH < -1)
		the sun never sets on this location (on the specified date)
*/

//7b. finish calculating H and convert into hours
float H;

if (!rise_or_set)
	H = 360 - (180 / PI) * acos(cosH);  //  if if rising time is desired:
else
	H = (180 / PI) * acos(cosH);		// if setting time is desired

H = H / 15;

//8. calculate local mean time of rising/setting   
float T = H + RA - (0.06571 * t) - 6.622;

//9. adjust back to UTC
float UT = fmod(T - lngHour, (float)24.0);

//10. convert UT value to local time zone of latitude/longitude
return UT + localOffset + daylightSavings;
}


/*
	WEATHER_MATH::SUNRISE()
	-----------------------
*/
void weather_math::sunrise(int *hour, int *min, int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings)
{
float localT = fmod(24 + calculate_sunrise_sunset(year, month, day, lat, lng, localOffset, daylightSavings, SUNRISE), (float)24.0);
double hours, minutes;

minutes = modf(localT, &hours) * 60;

*hour = (int)hours;
*min = (int)minutes;
}

/*
	WEATHER_MATH::SUNSET()
	----------------------
*/
void weather_math::sunset(int *hour, int *min, int year, int month, int day, float lat, float lng, int localOffset, int daylightSavings)
{
float localT = fmod(24 + calculate_sunrise_sunset(year, month, day, lat, lng, localOffset, daylightSavings, SUNSET), (float)24.0);
double hours, minutes;

minutes = modf(localT, &hours) * 60;

*hour = (int)hours;
*min = (int)minutes;
}

/*
	WEATHER_MATH::ZAMBRETTI()
	-------------------------
	Implementaton of the Zambretti weather forcast algorithm.  This code is based on the
	Python version of the JavaScript implementation.  The important details are:

	# honeysucklecottage.me.uk - Python port of beteljuice
	# javascript forecaster. Comes with no warranty of any kind.
	#
	# Further tweaking / Pythonification by Jim Easterbrook
	#
	# beteljuice.com - near enough Zambretti Algorhithm
	# June 2008 - v1.0
	# tweak added so decision # can be output
	#
	# Negretti and Zambras 'slide rule' is supposed to be better than 90% accurate
	# for a local forecast upto 12 hrs, it is most accurate in the temperate zones and about 09:00  hrs local solar time.
	# I hope I have been able to 'tweak it' a little better ;-)
	#
	# This code is free to use and redistribute as long as NO CHARGE is EVER made for its use or output

	Parameters:
		z_hpa is Sea Level Adjusted (Relative) barometer in hPa or mB
		z_month is current month as a number between 1 to 12
		z_wind is integer 0 to 15. 0 = N, 1 = NNE, 2 = NE, ... , 15 = NNW
		z_trend is barometer trend: (RISE, STEADY, FALL)
		z_north - in northern hemisphere, default true
		z_baro_top - upper range of barometer, default 1050
		z_baro_bottom - lower range of barometer, default 950
	Returns:
		the position on the wheel (0..26) where 26 is for overflow and underflow.
*/
long weather_math::zambretti(double z_hpa, long z_month, long z_wind, long z_trend, long z_north, double z_baro_top, double z_baro_bottom)
{
size_t choice;

/*
	Equivalents of Zambretti 'dial window' letters A - Z (with "26" added for overflow and underflow)
*/
static long rise_options[]   = {25, 25, 25, 24, 24, 19, 16, 12, 11,  9,  8,  6,  5,  2, 1, 1, 0, 0, 0, 0, 0, 0, 26};
static long steady_options[] = {25, 25, 25, 25, 25, 25, 23, 23, 22, 18, 15, 13, 10,  4, 1, 1, 0, 0, 0, 0, 0, 0, 26};
static long fall_options[]   = {25, 25, 25, 25, 25, 25, 25, 25, 23, 23, 21, 20, 17, 14, 7, 3, 1, 1, 1, 0, 0, 0, 26};

/*
	Equivelant of the huge cast in the beteljuice.com version
*/
static long wind_scale[] = {6.0, 5.0, 5.0, 2.0, -0.5, -2.0, -5.0, -8.5, -12.0, -10.0, -6.0, -4.5, -3.0, -0.5, 1.5, 3.0};

/*
	How far through the scale are we?
*/
double z_option = (double)(z_hpa - z_baro_bottom) / (double)(z_baro_top - z_baro_bottom);

/*
	Are we northern or southen hemisphere
*/
if (!z_north)
	z_wind = (z_wind + 8) % 16;		// southern hemisphere so add 180 degrees to the wind direction

/*
	Add the wind effect
*/
z_option += wind_scale[z_wind] / 100.0;

/*
	Add the summer effect (if we're in summer)
*/
if (z_north == (z_month >= 4 && z_month <= 9))
	if (z_trend == RISE)
		z_option += 7.0 / 100.0;
	else if (z_trend == FALL)
		z_option -= 7.0 / 100.0;

choice = floor(z_option * 22.0);

/*
	Check for overflow and underflow of the tables
*/
if (choice < 0 || choice > 21)
	choice = 22;

/*
	Return the message string
*/
if (z_trend == RISE)
	return rise_options[choice];
else if (z_trend == FALL)
	return fall_options[choice];
else					// Steady
	return steady_options[choice];
}

/*
	WEATHER_MATH::ZAMBRETTI_NAME()
	------------------------------
*/
char *weather_math::zambretti_name(long number)
{
/*
	Equivelant of the Zambretti message wheel
*/
static char *forecast[] = 
	{
	"Settled fine", 						// 49
	"Fine weather", 						// 49
	"Becoming fine", 						// 49
	"Fine, becoming less settled", 			// 65
	"Fine, possible showers", 				// 72
	"Fairly fine, improving", 				// 65
	"Fairly fine, possible showers early", 	// 72
	"Fairly fine, showery later", 			// 76
	"Showery early, improving", 			// 76
	"Changeable, mending", 					// 72
	"Fairly fine, showers likely", 			// 78
	"Rather unsettled clearing later", 		// 86
	"Unsettled, probably improving", 		// 86
	"Showery, bright intervals", 			// 84
	"Showery, becoming less settled", 		// 80
	"Changeable, some rain", 				// 76
	"Unsettled, short fine intervals", 		// 109
	"Unsettled, rain later", 				// 71
	"Unsettled, some rain", 				// 71
	"Mostly very unsettled", 				// 83
	"Occasional rain, worsening", 			// 87
	"Rain at times, very unsettled", 		// 80
	"Rain at frequent intervals", 			// 85
	"Rain, very unsettled", 				// 81
	"Stormy, may improve", 					// 83
	"Stormy, much rain",					// 81
	"Truely exceptional weather"			// 122  	// used for overflow or underflow... not a Zambretti code
	};

return forecast[number];
}
