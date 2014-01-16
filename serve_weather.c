/*
	SERVE_WEATHER.C
	---------------
	Copyright (c) 2013 Andrew Trotman
	Licensed BSD
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "usb_weather.h"
#include "usb_weather_datetime.h"
#include "usb_weather_fixed_block_1080.h"
#include "usb_weather_message.h"
#include "usb_weather_reading.h"
#include "weather_math.h"

#define MODE_IPHONE 0
#define MODE_PC 1

static const int mode = MODE_IPHONE;

double latitude = -45.879278;
double longitude = 170.487778;
double height_above_sea_level_in_m = 184;

/*
	These are the USB VID and PID of the weather station I've got
*/
#define USB_WEATHER_VID 0x1941			// Dream Link (in my case DIGITECH)
#define USB_WEATHER_PID 0x8021			// WH1080 Weather Station / USB Missile Launcher (in my case USB Wireless Weather Station)


/*
	TWO_DP()
	--------
*/
double two_dp(double value)
{
return ((long long)(value * 100.0)) / 100.0;
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
usb_weather_reading *deltas, *long_deltas;
double wind_direction, barometric_delta, dew_point;

if ((readings = station->read_current_readings()) == NULL)
	exit(printf("Cannot read current readings\n"));

long_deltas = station->read_hourly_delta();				// "near-hour" deltas
deltas = station->interpolate_hourly_delta(long_deltas);	// interpolate "near-hour" deltas into hourly deltas

fixed_block = station->read_fixed_block();

puts("<html>");
puts("<head>");

puts("<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">");
puts("<meta name=\"apple-mobile-web-app-title\" content=\"ScarbaWeather\">");
puts("<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"black\">");
puts("<link rel=\"apple-touch-startup-image\" href=\"/startup.png\">");

//puts("<meta id=\"viewport\" name=\"viewport\" content=\"initial-scale=0.70\">");
puts("<meta id=\"viewport\" name=\"viewport\" content=\"initial-scale=0.1\">");

puts("<script>");

puts("if (window.screen.availHeight == 460)");			// iPhone 4
puts("	document.getElementById(\"viewport\").setAttribute(\"content\", \"initial-scale=0.32\");");
puts("else if (window.screen.availHeight == 1004)");	// iPad
puts("	document.getElementById(\"viewport\").setAttribute(\"content\", \"initial-scale=0.7\");");
puts("else");											// iPhone 5?
puts("	document.getElementById(\"viewport\").setAttribute(\"content\", \"initial-scale=0.3\");");

puts("</script>");

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

puts("html, body");
puts("	{");
puts("	font-family:calibri,euphemiaucas;");
puts("	font-size:36pt;");
puts("	height:100%");
puts("	}");
puts("td");
puts("	{");
puts("	font-family:calibri,euphemiaucas;");
puts("	font-size:36pt;");
puts("	}");
puts(".medium");
puts("	{");
puts("	font-size:40pt;");
puts("	}");
puts(".huge");
puts("	{");
puts("	font-size:100pt;");
puts("	}");
puts(".megahuge");
puts("	{");
puts("	font-family:weather;");
puts("	font-size:380pt");
puts("	}");
puts(".symbol");
puts("	{");
puts("	font-family:weather;");
puts("	font-size:60pt;");
puts("	}");
puts(".moon");
puts("	{");
puts("	font-family:moon_phasesregular;");
puts("	font-size:40pt;");
puts("	}");
puts(".arrowfont");
puts("	{");
puts("	font-family:times;");
puts("	font-weight:bold;");
puts("	}");
puts(".space");
puts("	{");
puts("	line-height:40px;");
puts("	}");
puts(".halfspace");
puts("	{");
puts("	line-height:20px;");
puts("	}");

puts("A:link, A:visited, A:active, A:hover ");
puts("	{");
puts("	text-decoration: none;");
puts("	color: black;");
puts("	}");

puts("</style>");
puts("</head>");

puts("<body>");
puts("<table cellpadding=0 cellspacing=0 border=0 width=100%>");

/*
	Sunrise, Moon, and Sunset
*/
puts("<tr><td><table cellpadding=0 cellspacing=0 border=0 width=100%><tr><td align=left><span class=\"symbol\">7</span>");
daylight_savings = weather_math::is_daylight_saving();
fixed_block->current_time.extract(&year, &month, &day, &hour, &minute);
weather_math::sunrise(&sun_hour, &sun_minute, year + 2000, month, day, latitude, longitude, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf("%02d:%02d", sun_hour, sun_minute);

uint8_t moon_phase = (weather_math::phase_of_moon(2000 + year, month, day) / 29.0) * 26.0;
printf("</td><td align=center><span class=\"moon\">%c</span></td><td align=right>", 'A' + moon_phase);

weather_math::sunset(&sun_hour, &sun_minute, year + 2000, month, day, latitude, longitude, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf(" %02d:%02d", sun_hour, sun_minute);
puts("<span class=\"symbol\">8</span></td></tr></table></td></tr>");

/*
	Whats the time now? 
*/
puts("<tr><td align=center>");
fixed_block->current_time.text_render();
puts("</b><br></td></tr>");

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



	puts("<tr><td class=\"space\">&nbsp;</td></tr>");

	/*
		Outdoor temperature, humidity, rainfall
	*/
	double apparent_temperature = weather_math::apparent_temperature(readings->outdoor_temperature, readings->outdoor_humidity, readings->average_windspeed);
	//double australian_apparent_temperature = weather_math::australian_apparent_temperature(readings->outdoor_temperature, readings->outdoor_humidity, readings->average_windspeed);

	puts("<tr><td align=center><center><table cellpadding=0 cellspacing=0 border=0><tr><td align=right class=\"huge\">");
	printf("<span class=\"arrowfont\">%s</span>%0.2f", deltas->outdoor_temperature > 0 ? "&uarr;" : deltas->outdoor_temperature < 0 ? "&darr;" : "", readings->outdoor_temperature);
	printf("&deg;C&nbsp;</td><td align=left class=\"medium\">");
	printf("%0.2f", readings->outdoor_humidity);
	puts("%<br>");
	if (deltas->rain_counter_overflow)
		printf("full");
	else
		printf("%0.2fmm/h", deltas->total_rain);
	puts("</td></tr>");
	printf("<tr><td><span class=\"medium\"><center>(Feels like:%0.0f&deg;C)<center></span></td><td></td></tr>", apparent_temperature);
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
printf("<tr><td align=center class=\"tiny\"><span class=\"arrowfont\">%*.*s</span>%0.2fhPa (%s)</td></tr>", abs(trend) * 6, abs(trend) * 6, trend > 0 ? "&uarr;&uarr;&uarr;&uarr;" : "&darr;&darr;&darr;&darr;", sealevel_pressure, weather_math::zambretti_name(z_number));
printf("<tr><td class=\"halfspace\">&nbsp;</td></tr>");

/*
	Inside temperature and humidity
*/
printf("<tr><td align=center class=\"medium\">(");

if (!readings->lost_communications)
	{
	dew_point = weather_math::dewpoint(readings->outdoor_temperature, readings->outdoor_humidity);
	printf("Dewpoint:%0.0f&deg;C, ", dew_point);
	}

printf("Inside:%0.0f&deg;C %0.0f%%) <span class=symbol><span class=huge><a href=%s>*</a></span></span></td></tr>", readings->indoor_temperature, readings->indoor_humidity, getenv("SCRIPT_NAME"));
puts("</table>");


/*
	Done
*/
puts("</body>");
puts("</html>");

return readings;
}

/*
	RENDER_HTML_HEADER()
	--------------------
*/
void render_html_header(void)
{
puts("<html>");
puts("<head>");
puts("<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>");
puts("<script type=\"text/javascript\">");
puts("google.load(\"visualization\", \"1\", {packages:[\"corechart\"]});");
puts("google.load(\"visualization\", \"1\", {packages:[\"gauge\"]});");
}

/*
	RENDER_HTML_FOOTER()
	--------------------
*/
void render_html_footer(usb_weather *station, usb_weather_fixed_block_1080 *fixed_block, usb_weather_reading *current, usb_weather_reading **historic)
{
uint8_t year, month, day, hour, minute;
int sun_hour, sun_minute;
int daylight_savings;

puts("</script>");
puts("</head>");
puts("<body>");
puts("<b>Current Readings</b><br>");
printf("<b><font size=-1>");
fixed_block->current_time.text_render();
printf(":", current->delay);
printf(" (%ld minutes ago)</font></b><br>", current->delay);

daylight_savings = weather_math::is_daylight_saving();

fixed_block->current_time.extract(&year, &month, &day, &hour, &minute);
weather_math::sunrise(&sun_hour, &sun_minute, year + 2000, month, day, latitude, longitude, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf("<b><font size=-1>Sun Rise: %d:%d", sun_hour, sun_minute);

weather_math::sunset(&sun_hour, &sun_minute, year + 2000, month, day, latitude, longitude, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf(" Set: %d:%d</font></b><br>", sun_hour, sun_minute);

puts("<table><tr>");
puts("<td><div id='current_temperature'></div></td>");
puts("<td><div id='current_humidity'></div></td>");
puts("<td><div id='current_wind'></div></td>");
puts("<td><div id='current_pressure'></div></td>");
puts("<td><div id='current_rain'></div></td>");
puts("</tr></table>");
puts("<hr>");
puts("<b>Historic Readings</b><br>");
puts("<table>");
puts("<tr><td><div id=\"chart_div_outdoor_temperature\" style=\"width: 450px; height: 250px;\"></div></td><td><div id=\"chart_div_indoor_temperature\" style=\"width: 450px; height: 250px;\"></div></tr>");
puts("<tr><td><div id=\"chart_div_outdoor_humidity\" style=\"width: 450px; height: 250px;\"></div></td><td><div id=\"chart_div_indoor_humidity\" style=\"width: 450px; height: 250px;\"></div></tr>");
puts("<tr><td><div id=\"chart_div_windspeed\" style=\"width: 450px; height: 250px;\"></div></td><td><div id=\"chart_div_gust\" style=\"width: 450px; height: 250px;\"></div></tr>");
puts("<tr><td><div id=\"chart_div_pressure\" style=\"width: 450px; height: 250px;\"></div></td><td><div id=\"chart_div_rain\" style=\"width: 450px; height: 250px;\"></div></tr>");
puts("</table>");
puts("</body>");
puts("</html>");
}

/*
	RENDER_HTML_GRAPH()
	-------------------
*/
void render_html_graph(const char *name, const char *title, const char *x_title, const char *y_title, long elements, long *timeline, double *data)
{
long current;

printf("google.setOnLoadCallback(drawChart_%s);\n", name);

printf("function drawChart_%s()\n", name);
printf("{\n");

printf("var data = google.visualization.arrayToDataTable([['Minutes', 'Data']");
for (current = 0; current < elements; current++)
	printf(", [%d, %0.2f]", timeline[current], data[current]);
printf("]);\n");

printf("var options =\n");
printf("	{\n");
printf("	title: '%s',\n", title);
printf("	hAxis: {title: '%s', direction: -1},\n", x_title);
printf("	vAxis: {title: '%s'},\n", y_title);
printf("	lineWidth: 1,\n");
printf("	legend: 'none'\n");
printf("	};\n");
printf("var chart = new google.visualization.ScatterChart(document.getElementById('chart_div_%s'));\n", name);
printf("chart.draw(data, options);\n");
printf("}\n\n");
}

/*
	RENDER_CURRENT_READINGS()
	-------------------------
*/
usb_weather_reading *render_current_readings(usb_weather *station)
{
usb_weather_reading *readings;
uint32_t width = 400, height = 120;

readings = station->read_current_readings();
if (readings == NULL)
	exit(printf("Cannot read current readings\n"));

puts("google.setOnLoadCallback(draw_current_stats);");
puts("function draw_current_stats()");
puts("{");

/*
	Indoor and Outdoor temperature
*/
printf("var temperature = google.visualization.arrayToDataTable([['Label', 'Value']");
printf(", ['Out Temp C', %0.2f]", readings->outdoor_temperature);
printf(", ['In Temp C', %0.2f]", readings->indoor_temperature);
puts("]);");

puts("var temperature_options =");
puts("	{");
printf("	width: %ld,\n", width);
printf("	height: %ld,\n", height);
puts("	min: -10,");
puts("	max: 50,");
puts("	redFrom: -10,");
puts("	redTo: 0,");
puts("	majorTicks: ['-10', '0', '10', '20', '30', '40', '50'],");
puts("	minorTicks: 2");
puts("	};");
puts("var chart = new google.visualization.Gauge(document.getElementById('current_temperature'));");
puts("chart.draw(temperature, temperature_options);");

/*
	Indoor and Outdoor humidity
*/
printf("var humidity = google.visualization.arrayToDataTable([['Label', 'Value']");
printf(", ['Out Humid %%', %0.2f]", readings->outdoor_humidity);
printf(", ['In Humid %%', %0.2f]", readings->indoor_humidity);
puts("]);");

puts("var humidity_options =");
puts("	{");
printf("	width: %ld,\n", width);
printf("	height: %ld,\n", height);
puts("	min: 0,");
puts("	max: 100,");
puts("	majorTicks: ['0', '20', '40', '60', '80', '100'],");
puts("	minorTicks: 4");
puts("	};");
puts("var chart = new google.visualization.Gauge(document.getElementById('current_humidity'));");
puts("chart.draw(humidity, humidity_options);");

/*
	Absolute Pressure
*/
printf("var pressure = google.visualization.arrayToDataTable([['Label', 'Value']");
printf(", ['Abs Press hPa', %0.2f]", readings->absolute_pressure);
puts("]);");

puts("var pressure_options =");
puts("	{");
printf("	width: %ld,\n", width);
printf("	height: %ld,\n", height);
puts("	min: 950,");
puts("	max: 1100,");
puts("	majorTicks: ['950', '1000', '1050', '1100'],");
puts("	minorTicks: 6");
puts("	};");
puts("var chart = new google.visualization.Gauge(document.getElementById('current_pressure'));");
puts("chart.draw(pressure, pressure_options);");

/*
	Wind
*/
printf("var wind = google.visualization.arrayToDataTable([['Label', 'Value']");
printf(", ['Wind kn', %0.2f]", weather_math::knots(readings->average_windspeed));
puts("]);");

puts("var wind_options =");
puts("	{");
printf("	width: %ld,\n", width);
printf("	height: %ld,\n", height);
puts("	min: 0,");
printf("	redFrom: %0.2f,\n", weather_math::knots(readings->average_windspeed));
printf("	redTo: %0.2f\n", weather_math::knots(readings->gust_windspeed));
puts("	};");
puts("var chart = new google.visualization.Gauge(document.getElementById('current_wind'));");
puts("chart.draw(wind, wind_options);");

/*
	Rainfall
*/
printf("var rain = google.visualization.arrayToDataTable([['Label', 'Value']");
printf(", ['Rain mm', %0.2f]", readings->total_rain);
puts("]);");

puts("var rain_options =");
puts("	{");
printf("	width: %ld,\n", width);
printf("	height: %ld,\n", height);
puts("	min: 0");
puts("	};");
puts("var chart = new google.visualization.Gauge(document.getElementById('current_rain'));");
puts("chart.draw(rain, rain_options);");

puts("}");

return readings;
}

/*
	RENDER_HISTORIC_READINGS()
	--------------------------
*/
usb_weather_reading **render_historic_readings(usb_weather *station)
{
long sum;
long *timeline;
double *data;
usb_weather_reading **history;
int32_t current;
uint32_t readings;

if ((history = station->read_all_readings(&readings)) == NULL)
	exit(printf("Cannot read historic readings\n"));

timeline = new long [readings];
data = new double [readings];

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

for (current = readings - 1; current >= 0; current--)
	data[current] = history[current]->indoor_temperature;
render_html_graph("indoor_temperature", "Indoor Temperature", "Minutes Ago", "Degrees C", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	data[current] = history[current]->indoor_humidity;
render_html_graph("indoor_humidity", "Indoor Humidity", "Minutes Ago", "Percent", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	data[current] = history[current]->absolute_pressure;
render_html_graph("pressure", "Pressure", "Minutes Ago", "Hectopascals", readings, timeline, data);

/*
	The rainbucket can fill so we only have a valid reading if it isn't full and we have valid communications
*/
sum = 0;
for (current = readings - 1; current >= 0; current--)
	{
	if (!history[current]->lost_communications)
		{
		timeline[current] = sum;
		data[current] = history[current]->total_rain;
		}
	sum += history[current]->delay;
	}
render_html_graph("rain", "Cumulative Rainfall", "Minutes Ago", "millimetres", readings, timeline, data);

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

for (current = readings - 1; current >= 0; current--)
	if (!history[current]->lost_communications)
		data[current] = history[current]->outdoor_temperature;
render_html_graph("outdoor_temperature", "Outdoor Temperature", "Minutes Ago", "Degrees C", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	if (!history[current]->lost_communications)
		data[current] = history[current]->outdoor_humidity;;
render_html_graph("outdoor_humidity", "Outdoor Humidity", "Minutes Ago", "Percent", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	if (!history[current]->lost_communications)
		data[current] = weather_math::knots(history[current]->average_windspeed);
render_html_graph("windspeed", "Wind Speed", "Minutes Ago", "Knots", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	if (!history[current]->lost_communications)
		data[current] = weather_math::knots(history[current]->gust_windspeed);
render_html_graph("gust", "Wind Gust", "Minutes Ago", "Knots", readings, timeline, data);

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

puts("Content-type: text/html\n");

if (station.connect(USB_WEATHER_VID, USB_WEATHER_PID) == 0)
	{
	if (mode == MODE_IPHONE)
		current = render_current_readings_iphone(&station);
	else
		{
		render_html_header();
		current = render_current_readings(&station);
		historic = render_historic_readings(&station);
		render_html_footer(&station, station.read_fixed_block(), current, historic);
		}
	}
else
	exit(printf("Cannot find an attached weather station\n"));

return 0;
}
