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
//float localT = fmod(24 + calculateSunrise(2014, 1, 1, -45.8667, 170.5000, 12, 1, SUNSET), (float)24.0);
float localT = fmod(24 + calculate_sunrise_sunset(year, month, day, lat, lng, localOffset, daylightSavings, SUNSET), (float)24.0);
double hours, minutes;

minutes = modf(localT, &hours) * 60;

*hour = (int)hours;
*min = (int)minutes;
}
