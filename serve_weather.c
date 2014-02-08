/*
	SERVE_WEATHER.C
	---------------
	Copyright (c) 2013 Andrew Trotman
	Licensed BSD
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "usb_weather.h"
#include "usb_weather_datetime.h"
#include "usb_weather_fixed_block_1080.h"
#include "usb_weather_message.h"
#include "usb_weather_reading.h"
#include "weather_math.h"

/*
	Our current longitude, latitude, and height above sea level (needed for sunrise, sunset, and sea level pressure adjustment)
*/
double latitude = -45.879278;
double longitude = 170.487778;
double height_above_sea_level_in_m = 184;

/*
	for rendering historic readings
*/
enum {NONE = 0, OUTSIDE_TEMPERATURE = 0x01, INSIDE_TEMPERATURE = 0x02, OUTSIDE_HUMIDITY = 0x04, INSIDE_HUMIDITY = 0x08, RAINFALL = 0x10, WINDSPEED = 0x20, WINDGUST = 0x40, PRESSURE = 0x80, ALL = 0xFF};

/*
	These are the USB VID and PID of the weather station I've got
*/
#define USB_WEATHER_VID 0x1941			// Dream Link (in my case DIGITECH)
#define USB_WEATHER_PID 0x8021			// WH1080 Weather Station / USB Missile Launcher (in my case USB Wireless Weather Station)

/*
	Prototypes
*/
usb_weather_reading **render_historic_readings(usb_weather *station, uint32_t what_to_render);

/*
	TWO_DP()
	--------
*/
double two_dp(double value)
{
return ((long long)(value * 100.0)) / 100.0;
}

/*
	RENDER_HTML_HEAD_IPHONE()
	-------------------------
*/
void render_html_head_iphone(usb_weather *station, long readings)
{
char *msie;

puts("<html>");
puts("<head>");

/*
	Apple Webapp
*/
puts("<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">");
puts("<meta name=\"apple-mobile-web-app-title\" content=\"ScarbaWeather\">");
puts("<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"black\">");
puts("<link href=\"/startup.png\" media=\"(device-width: 320px) and (device-height: 480px) and (-webkit-device-pixel-ratio: 2)\" rel=\"apple-touch-startup-image\">");

//puts("<meta id=\"viewport\" name=\"viewport\" content=\"initial-scale=0.70\">");
puts("<meta id=\"viewport\" name=\"viewport\" content=\"initial-scale=0.1\">");

/*
	Google graphs
*/
if (readings != NONE)
	{
	puts("<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>");
	puts("<script type=\"text/javascript\">");
	puts("google.load(\"visualization\", \"1\", {packages:[\"corechart\"]});");
	render_historic_readings(station, readings);
	puts("</script>");
	}
/*
	render all links in the web app
*/
puts("<script>(function(a,b,c){if(c in b&&b[c]){var d,e=a.location,f=/^(a|html)$/i;a.addEventListener(\"click\",function(a){d=a.target;while(!f.test(d.nodeName))d=d.parentNode;\"href\"in d&&(d.href.indexOf(\"http\")||~d.href.indexOf(e.host))&&(a.preventDefault(),e.href=d.href)},!1)}})(document,window.navigator,\"standalone\")</script>");

/*
	resize to fit screen
*/
puts("<script type=\"text/javascript\">");
puts("if (window.screen.availHeight == 460)");			// iPhone 4
puts("	document.getElementById(\"viewport\").setAttribute(\"content\", \"initial-scale=0.32\");");
puts("else if (window.screen.availHeight == 1004)");	// iPad
puts("	document.getElementById(\"viewport\").setAttribute(\"content\", \"initial-scale=0.7\");");
puts("else");											// iPhone 5?
puts("	document.getElementById(\"viewport\").setAttribute(\"content\", \"initial-scale=0.3\");");
puts("</script>");

/*
	Fonts
*/
puts("<style>");
puts("@font-face {");
puts("    font-family: 'weather';");
puts("    src: url('/artill_clean_icons-webfont.eot');");
puts("    src: url('/artill_clean_icons-webfont.eot?#iefix') format('embedded-opentype'),");
puts("         url('/artill_clean_icons-webfont.woff') format('woff'),");
puts("         url('/artill_clean_icons-webfont.ttf') format('truetype'),");
puts("         url('/artill_clean_icons-webfont.svg#artill_clean_weather_iconsRg') format('svg');");
puts("    font-weight: normal;");
puts("    font-style: normal;");
puts("}");

puts("@font-face {");
puts("    font-family: 'moon_phasesregular';");
puts("    src: url('/moon_phases-webfont.eot');");
puts("    src: url('/moon_phases-webfont.eot?#iefix') format('embedded-opentype'),");
puts("         url('/moon_phases-webfont.woff') format('woff'),");
puts("         url('/moon_phases-webfont.ttf') format('truetype'),");
puts("         url('/moon_phases-webfont.svg#moon_phasesregular') format('svg');");
puts("    font-weight: normal;");
puts("    font-style: normal;");
puts("}");

/*
	Are we Microsoft Windows?
*/
if ((msie = getenv("HTTP_USER_AGENT")) != NULL)
	msie = strstr(msie, "MSIE");

puts("html, body");
puts("	{");
puts("	font-family:calibri,euphemiaucas;");
if (msie)
	puts("	font-size:18pt;");
else
	puts("	font-size:36pt;");
puts("	height:100%");
puts("	}");
puts("td");
puts("	{");
puts("	font-family:calibri,euphemiaucas;");
if (msie)
	puts("	font-size:18pt;");
else
	puts("	font-size:36pt;");
puts("	}");
puts(".medium");
puts("	{");
if (msie)
	puts("	font-size:20pt;");
else
	puts("	font-size:40pt;");
puts("	}");
puts(".huge");
puts("	{");
if (msie)
	puts("	font-size:40pt;");
else
	puts("	font-size:100pt;");
puts("	}");
puts(".megahuge");
puts("	{");
puts("	font-family:weather;");
if (msie)
	puts("	font-size:100pt;");
else
	puts("	font-size:380pt;");
puts("	}");
puts(".symbol");
puts("	{");
puts("	font-family:weather;");
if (msie)
	puts("	font-size:30pt;");
else
	puts("	font-size:60pt;");
puts("	}");
puts(".moon");
puts("	{");
puts("	font-family:moon_phasesregular;");

#ifdef NEVER
	puts("	filter:  progid:DXImageTransform.Microsoft.BasicImage(rotation=2);");  /* IE6,IE7 */
	puts("	ms-filter: \"progid:DXImageTransform.Microsoft.BasicImage(rotation=2)\";"); /* IE8 */
	puts("	-moz-transform: rotate(-180deg);");  /* FF3.5+ */
	puts("	-o-transform: rotate(-180deg);");  /* Opera 10.5 */
	puts("	-webkit-transform: rotate(-180deg);");  /* Safari 3.1+, Chrome */
	puts("	position: absolute; ");
#endif

if (msie)
	puts("	font-size:20pt;");
else
	puts("	font-size:40pt;");
puts("	}");
puts(".arrowfont");
puts("	{");
puts("	font-family:times;");
puts("	font-weight:bold;");
puts("	}");
puts(".space");
puts("	{");
if (msie)
	puts("	font-size:20px;");
else
	puts("	font-size:40px;");
puts("	}");
puts(".halfspace");
puts("	{");
if (msie)
	puts("	font-size:10px;");
else
	puts("	font-size:20px;");
puts("	}");

puts("A:link, A:visited, A:active, A:hover ");
puts("	{");
puts("	text-decoration: none;");
puts("	color: black;");
puts("	}");

puts("</style>");
puts("</head>");
}

/*
	RENDER_HTML_TAIL_IPHONE()
	-------------------------
*/
void render_html_tail_iphone(void)
{
puts("<body>");

printf("<table cellpadding=0 cellspacing=0 border=0 width=100%% style=\"position:absolute;bottom:0;\">");
printf("<tr>");
printf("<td align=center><span class=symbol><span class=huge><a href=%s?action=temperature>&#8216;</a></span></span></td>", getenv("SCRIPT_NAME"));
printf("<td align=center><span class=symbol><span class=huge><a href=%s?action=rain>&#77;</a></span></span></td>", getenv("SCRIPT_NAME"));
printf("<td align=center><span class=symbol><span class=huge><a href=%s?action=wind>&#69;</a></span></span></td>", getenv("SCRIPT_NAME"));
printf("<td align=center><span class=symbol><span class=huge><a href=%s?action=humidity>&#71;</a></span></span></td>", getenv("SCRIPT_NAME"));
printf("<td align=center><span class=symbol><span class=huge><a href=%s?action=pressure>&#88;</a></span></span></td>", getenv("SCRIPT_NAME"));
printf("<td align=center><span class=symbol><span class=huge><a href=%s>*</a></span></span></td>", getenv("SCRIPT_NAME"));
printf("</tr>");
printf("</table>");

puts("</body>");
puts("</html>");
}

/*
	RENDER_HISTORIC_READINGS_IPHONE()
	---------------------------------
*/
long render_historic_readings_iphone(usb_weather *station, long what_to_read)
{
render_html_head_iphone(station, what_to_read);

if (what_to_read & OUTSIDE_TEMPERATURE)
	{
	puts("<div style=\"font-size:60pt;\"><center>Outside Temperature</center></div>");
	puts("<table><tr><td><div id=\"chart_div_outdoor_temperature\" style=\"width: 1100px; height: 1250px;\"></div></td></tr></table>");
	}
if (what_to_read & INSIDE_TEMPERATURE)
	{
	puts("<div style=\"font-size:60pt;\"><center>Inside Temperature</center></div>");
	puts("<table><tr><td><div id=\"chart_div_indoor_temperature\" style=\"width: 1100px; height: 1250px;\"></div></td></tr></table>");
	}
if (what_to_read & OUTSIDE_HUMIDITY)
	{
	puts("<div style=\"font-size:60pt;\"><center>Outside Humidity</center></div>");
	puts("<table><tr><td><div id=\"chart_div_outdoor_humidity\" style=\"width: 1100px; height: 1250px;\"></div></td></tr></table>");
	}
if (what_to_read & INSIDE_HUMIDITY)
	{
	puts("<div style=\"font-size:60pt;\"><center>Inside Humidity</center></div>");
	puts("<table><tr><td><div id=\"chart_div_indoor_humidity\" style=\"width: 1100px; height: 1250px;\"></div></td></tr></table>");
	}
if (what_to_read & RAINFALL)
	{
	puts("<div style=\"font-size:60pt;\"><center>Rainfall</center></div>");
	puts("<table><tr><td><div id=\"chart_div_rain\" style=\"width: 1100px; height: 1250px;\"></div></td></tr></table>");
	}
if (what_to_read & WINDSPEED)
	{
	puts("<div style=\"font-size:60pt;\"><center>Windspeed</center></div>");
	puts("<table><tr><td><div id=\"chart_div_windspeed\" style=\"width: 1100px; height: 550px;\"></div></td></tr></table>");
	}
if (what_to_read & WINDGUST)
	{
	puts("<div style=\"font-size:60pt;\"><center>Wind Gusts</center></div>");
	puts("<table><tr><td><div id=\"chart_div_gust\" style=\"width: 1100px; height: 550px;\"></div></td></tr></table>");
	}
if (what_to_read & PRESSURE)
	{
	puts("<div style=\"font-size:60pt;\"><center>Absolute Pressure</center></div>");
	puts("<table><tr><td><div id=\"chart_div_pressure\" style=\"width: 1100px; height: 1250px;\"></div></td></tr></table>");
	}

render_html_tail_iphone();

return 0;
}

/*
	RENDER_CURRENT_READINGS_IPHONE()
	--------------------------------
*/
usb_weather_reading *render_current_readings_iphone(usb_weather *station)
{
static long z_to_font[] = {49, 49, 49, 65, 72, 65, 72, 76, 76, 72, 78, 86, 86, 84, 80, 76, 109, 71, 71, 83, 87, 80, 85, 81, 83, 81, 122};
usb_weather_reading *readings;
uint8_t year, month, day, hour, minute;
int sun_hour, sun_minute;
int daylight_savings, barometric_state;
long z_number;
usb_weather_fixed_block_1080 *fixed_block;
usb_weather_reading *deltas, *long_deltas, highs, lows, *got_high_low;
double wind_direction, barometric_delta, dew_point;
char *msie;
long mins_since_sunrise;

if ((readings = station->read_current_readings()) == NULL)
	exit(printf("Cannot read current readings\n"));

long_deltas = station->read_hourly_delta();				// "near-hour" deltas
deltas = station->interpolate_hourly_delta(long_deltas);	// interpolate "near-hour" deltas into hourly deltas
got_high_low = station->read_highs_and_lows(&highs, &lows);

fixed_block = station->read_fixed_block();

render_html_head_iphone(station, NONE);


puts("<body>");

puts("<table cellpadding=0 cellspacing=0 border=0 width=100%>");
/*
	Sunrise, 24-Hour low, Moon, 24-Hour high, and Sunset
*/
puts("<tr><td><table cellpadding=0 cellspacing=0 border=0 width=100%><tr>");
daylight_savings = weather_math::is_daylight_saving();
fixed_block->current_time.extract(&year, &month, &day, &hour, &minute);
weather_math::sunrise(&sun_hour, &sun_minute, year + 2000, month, day, latitude, longitude, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf("<td align=left><span class=\"symbol\">7</span>%02d:%02d</td>", sun_hour, sun_minute);

if (got_high_low)
	printf("<td align=center>%0.0f&deg;C</td>", lows.outdoor_temperature);

uint8_t moon_phase = (weather_math::phase_of_moon(2000 + year, month, day) / 30.0) * 26.0;
printf("<td align=center><span class=\"moon\">%c</span></td>", 'A' + moon_phase);

if (got_high_low)
	printf("<td align=center>%0.0f&deg;C</td>", highs.outdoor_temperature);

weather_math::sunset(&sun_hour, &sun_minute, year + 2000, month, day, latitude, longitude, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf("<td align=right>%02d:%02d<span class=\"symbol\">8</span></td></tr></table></td>", sun_hour, sun_minute);
puts("</tr>");

/*
	Whats the time now? 
*/
puts("<tr><td align=center>");
fixed_block->current_time.text_render();
puts("</b><br></td></tr>");
puts("</table>");


puts("<table cellpadding=0 cellspacing=0 border=0 width=100%>");
if (!readings->lost_communications)
	{
	/*
		Wind
	*/
	printf("<tr><td align=center class=\"huge\">%s</td></tr>", weather_math::wind_direction_name(readings->wind_direction));

	if (two_dp(readings->average_windspeed) == 0.00 && two_dp(readings->gust_windspeed) == 0.00)
		printf("<tr><td align=center class=\"medium\">%s</td></tr>", weather_math::beaufort_name(weather_math::beaufort(readings->average_windspeed)));
	else if (two_dp(readings->average_windspeed) == 0.00)
		printf("<tr><td align=center class=\"medium\">Gusts to %0.2fKn (%s)</td></tr>", weather_math::knots(readings->gust_windspeed), weather_math::beaufort_name(weather_math::beaufort(readings->average_windspeed)));
	else
		printf("<tr><td align=center class=\"medium\">%0.2fKn gusts to %0.2fKn (%s)</td></tr>", weather_math::knots(readings->average_windspeed), weather_math::knots(readings->gust_windspeed), weather_math::beaufort_name(weather_math::beaufort(readings->average_windspeed)));

	puts("<tr><td class=\"halfspace\">&nbsp;</td></tr>");

	/*
		Outdoor temperature, humidity, rainfall
	*/
	double apparent_temperature = weather_math::apparent_temperature(readings->outdoor_temperature, readings->outdoor_humidity, readings->average_windspeed);
	//double australian_apparent_temperature = weather_math::australian_apparent_temperature(readings->outdoor_temperature, readings->outdoor_humidity, readings->average_windspeed);

	puts("<tr><td align=center><center><table cellpadding=0 cellspacing=0 border=0>");
	puts("<tr><td align=right class=\"huge\">");
	printf("<span class=\"arrowfont\">%s</span>%0.2f", deltas->outdoor_temperature > 0 ? "&uarr;" : deltas->outdoor_temperature < 0 ? "&darr;" : "", readings->outdoor_temperature);
	printf("&deg;C&nbsp;</td><td align=left class=\"medium\">");
	printf("%0.2f", readings->outdoor_humidity);
	puts("%<br>");
	if (deltas->rain_counter_overflow)
		printf("full");
	else
		printf("<span class=\"medium\">%0.2fmm/h</span>", deltas->total_rain);
	puts("</td></tr>");
	printf("<tr><td><span class=\"medium\"><center>(Feels like:%0.0f&deg;C)<center></span></td>", apparent_temperature);
	if (got_high_low)
		printf("<td><span class=\"medium\">%0.2fmm/24h</span></td>", highs.total_rain - lows.total_rain);
	else
		puts("<td></td>");
	puts("</tr>");
	puts("</table></center></td></tr>");
	}
/*
	Pressure and Prediction
*/
if (readings->lost_communications)
	wind_direction = weather_math::NO_WIND;
else
	wind_direction = (readings->average_windspeed < 0.3) ? weather_math::NO_WIND : readings->wind_direction / 22.5;
double sealevel_pressure = weather_math::pressure_to_sea_level_pressure(readings->absolute_pressure, readings->outdoor_temperature, height_above_sea_level_in_m);

barometric_delta = (sealevel_pressure - weather_math::pressure_to_sea_level_pressure(readings->absolute_pressure + deltas->absolute_pressure, readings->outdoor_temperature, height_above_sea_level_in_m)) / 3.0;

z_number = weather_math::zambretti_pywws(sealevel_pressure, month, wind_direction, barometric_delta, false);

printf("<tr><td align=center class=\"megahuge\">&#%d;</td></tr>", z_to_font[z_number]);

int trend = weather_math::pressure_trend(deltas->absolute_pressure);
printf("<tr><td align=center><span class=\"arrowfont\">%*.*s</span>%0.2fhPa (%s)</td></tr>", abs(trend) * 6, abs(trend) * 6, trend > 0 ? "&uarr;&uarr;&uarr;&uarr;" : "&darr;&darr;&darr;&darr;", sealevel_pressure, weather_math::zambretti_name(z_number));

/*
	Inside temperature and humidity
*/
printf("<tr><td><table cellpadding=0 cellspacing=0 border=0 width=100%%><tr><td align=center class=\"medium\">");
if (!readings->lost_communications)
	{
	dew_point = weather_math::dewpoint(readings->outdoor_temperature, readings->outdoor_humidity);
	printf("Dewpoint:%0.0f&deg;C, ", dew_point);
	}
printf("Inside:%0.0f&deg;C, %0.0f%%</td></tr></table></td></tr>", readings->indoor_temperature, readings->indoor_humidity);
puts("</table>");

render_html_tail_iphone();

return readings;
}

/*
	RENDER_HTML_GRAPH()
	-------------------
*/
void render_html_graph(const char *name, const char *title, const char *x_title, const char *y_title, long elements, long *timeline, double *data, long mins_since_midnight)
{
long current, hours, mins, when;

printf("google.setOnLoadCallback(drawChart_%s);\n", name);

printf("function drawChart_%s()\n", name);
printf("{\n");

printf("var data = new google.visualization.DataTable();\n");

printf("data.addColumn('timeofday', 'Time');\n");
printf("data.addColumn('number', 'Yesterday');\n");
printf("data.addColumn('number', 'Today');\n");
for (current = 0; current < elements; current++)
	{
	if ((when = mins_since_midnight - timeline[current]) < 0)
		{
		when += (60 * 24);		// yesterday (or earlier)
		if (when > 0)			// we only care about yesterday (forget earlier)
			{
			hours = when / 60;
			mins = when % 60;
			printf("data.addRow([[%d, %d, 0, 0], %0.2f, null]);\n", hours, mins, data[current]);
			}
		}
	else
		{
		hours = when / 60;
		mins = when % 60;
		printf("data.addRow([[%d, %d, 0, 0], null, %0.2f]);\n", hours, mins, data[current]);
		}
	}

printf("var options =\n");
printf("	{\n");
printf("	title: '%s',\n", title);
printf("	fontName:\"euphemiaucas\",");
printf("	hAxis: {/*title: '%s', titleTextStyle: {fontSize: 45, italic:false}, */ textStyle: {fontSize: 29}},\n", x_title);
printf("	vAxis: {title: '%s', titleTextStyle: {fontSize: 45, italic:false}, textStyle: {fontSize: 29}},\n", y_title);
printf("	titleTextStyle: {fontSize:50, bold:false},\n");
printf("	lineWidth: 3,\n");
printf("	legend: 'none',\n");

puts("	chartArea: {top:14, height:'90%'},");

printf("	series: {0:{color: 'black', pointSize:1}, 1:{color: 'black', pointSize:8}}");
printf("	};\n");
printf("var chart = new google.visualization.ScatterChart(document.getElementById('chart_div_%s'));\n", name);
printf("chart.draw(data, options);\n");
printf("}\n\n");
}

/*
	RENDER_HISTORIC_READINGS()
	--------------------------
*/
usb_weather_reading **render_historic_readings(usb_weather *station, uint32_t what_to_render)
{
long sum;
long *timeline;
double *data;
usb_weather_reading **history;
long current, bucket;
uint32_t readings;
usb_weather_fixed_block_1080 *fixed_block;
uint8_t year, month, day, hour, minute;
long mins_since_midnight, readings_wanted;
double initial_rain;

fixed_block = station->read_fixed_block();
fixed_block->current_time.extract(&year, &month, &day, &hour, &minute);
mins_since_midnight = hour * 60 + minute;

readings_wanted = (mins_since_midnight + (60 * 24)) / fixed_block->read_period;

if ((history = station->read_all_readings(&readings, readings_wanted)) == NULL)
	exit(printf("Cannot read historic readings\n"));

timeline = new long [readings];
data = new double [readings];

/*
	The rainbucket can fill so we only have a valid reading if it isn't full and we have valid communications
*/
if ((what_to_render & RAINFALL) != 0)
	{
	sum = 0;
	for (current = readings - 1; current >= 1; current--)
		{
		if (!history[current]->lost_communications)
			{
			timeline[current] = sum;
			if (history[current]->total_rain == history[current - 1]->total_rain)
				data[current] = 0;
			else
				{
				/*
					Find the initial rain guage level (at the begining of the shower)
				*/
				initial_rain = history[0]->total_rain;
				for (bucket = current; bucket >= 1; bucket--)
					if (history[bucket]->total_rain == history[bucket - 1]->total_rain)
						{
						initial_rain = history[bucket]->total_rain;
						break;
						}
				/*
					Now compute this shower's amount
				*/
				data[current] = history[current]->total_rain - initial_rain;		// initial_rain is the cumulative rain (to date) *after* the shower
				}
			}
		sum += history[current]->delay;
		}
	render_html_graph("rain", "Rainfall", "Time", "Millimetres", readings, timeline, data, mins_since_midnight);
	}

/*
	Indoor temperature, indoor humidity, and pressure are from the base unit
	and so no communications errors can occur
*/
sum = 0;
for (current = readings - 1; current >= 0; current--)
	{
	timeline[current] = sum;
	sum += history[current]->delay;
	}

if ((what_to_render & INSIDE_TEMPERATURE) != 0)
	{
	for (current = readings - 1; current >= 0; current--)
		data[current] = history[current]->indoor_temperature;
	render_html_graph("indoor_temperature", "Inside Temperature", "Time", "Degrees C", readings, timeline, data, mins_since_midnight);
	}

if ((what_to_render & INSIDE_HUMIDITY) != 0)
	{
	for (current = readings - 1; current >= 0; current--)
		data[current] = history[current]->indoor_humidity;
	render_html_graph("indoor_humidity", "Inside Humidity", "Time", "Percent", readings, timeline, data, mins_since_midnight);
	}

if ((what_to_render & PRESSURE) != 0)
	{
	for (current = readings - 1; current >= 0; current--)
		data[current] = history[current]->absolute_pressure;
	render_html_graph("pressure", "Pressure", "Time", "Hectopascals", readings, timeline, data, mins_since_midnight);
	}

/*
	The remainder are from the remote unit and so an error can occur so we check the status word
*/
sum = 0;
for (current = readings - 1; current >= 0; current--)
	{
	if (!history[current]->lost_communications)
		timeline[current] = sum;
	sum += history[current]->delay;
	}

if ((what_to_render & OUTSIDE_TEMPERATURE) != 0)
	{
	for (current = readings - 1; current >= 0; current--)
		if (!history[current]->lost_communications)
			data[current] = history[current]->outdoor_temperature;
	render_html_graph("outdoor_temperature", "Outside Temperature", "Time", "Degrees C", readings, timeline, data, mins_since_midnight);
	}

if ((what_to_render & OUTSIDE_HUMIDITY) != 0)
	{
	for (current = readings - 1; current >= 0; current--)
		if (!history[current]->lost_communications)
			data[current] = history[current]->outdoor_humidity;;
	render_html_graph("outdoor_humidity", "Outside Humidity", "Time", "Percent", readings, timeline, data, mins_since_midnight);
	}

if ((what_to_render & WINDSPEED) != 0)
	{
	for (current = readings - 1; current >= 0; current--)
		if (!history[current]->lost_communications)
			data[current] = weather_math::knots(history[current]->average_windspeed);
	render_html_graph("windspeed", "Average Wind Speed", "Time", "Knots", readings, timeline, data, mins_since_midnight);
	}

if ((what_to_render & WINDGUST) != 0)
	{
	for (current = readings - 1; current >= 0; current--)
		if (!history[current]->lost_communications)
			data[current] = weather_math::knots(history[current]->gust_windspeed);
	render_html_graph("gust", "Wind Gust", "Time", "Knots", readings, timeline, data, mins_since_midnight);
	}

/*
	uint8_t  wind_direction;			// multiply by 22.5 to get degrees from north
*/

return history;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
usb_weather station;
usb_weather_reading *current = NULL;
usb_weather_reading **historic = NULL;
char *query_string;

puts("Content-type: text/html\n");

if (station.connect(USB_WEATHER_VID, USB_WEATHER_PID) == 0)
	{
	if ((query_string = getenv("QUERY_STRING")) != NULL)
		if (strstr(query_string, "temperature") != NULL)
			return render_historic_readings_iphone(&station, OUTSIDE_TEMPERATURE);
		else if (strstr(query_string, "wind") != NULL)
			return render_historic_readings_iphone(&station, WINDSPEED | WINDGUST);
		else if (strstr(query_string, "rain") != NULL)
			return render_historic_readings_iphone(&station, RAINFALL);
		else if (strstr(query_string, "humidity") != NULL)
			return render_historic_readings_iphone(&station, OUTSIDE_HUMIDITY);
		else if (strstr(query_string, "pressure") != NULL)
			return render_historic_readings_iphone(&station, PRESSURE);

	render_current_readings_iphone(&station);
	}
else
	{
	render_html_head_iphone(NULL, NONE);
	printf("Cannot find an attached weather station<br>");
	render_html_tail_iphone();
	}

return 0;
}
