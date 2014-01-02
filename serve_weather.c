/*
	SERVE_WEATHER.C
	---------------
	Copyright (c) 2013 Andrew Trotman
	Licensed BSD
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "usb_weather.h"
#include "usb_weather_datetime.h"
#include "usb_weather_fixed_block_1080.h"
#include "usb_weather_message.h"
#include "usb_weather_reading.h"
#include "weather_math.h"

#define MODE_IPHONE 0
#define MODE_IPAD 1
#define MODE_PC 2

static const int mode = MODE_IPHONE;
double lat = -45.8667;
double lng = 170.5000;

/*
	These are the USB VID and PID of the weather station I've got
*/
#define USB_WEATHER_VID 0x1941			// Dream Link (in my case DIGITECH)
#define USB_WEATHER_PID 0x8021			// WH1080 Weather Station / USB Missile Launcher (in my case USB Wireless Weather Station)


/*
	KNOTS()
	-------
*/
double knots(double m_per_second)
{
return m_per_second * 1.943844492457398;
}

/*
	WIND_DIRECTION()
	----------------
*/
char *wind_direction(double angle)
{
switch ((int)(angle / 22.5))
	{
	case 0:
		return "Northerly";
	case 1:
		return "NNE";
	case 2:
		return "Northeasterly";
	case 3:
		return "NEE";
	case 4:
		return "Easterly";
	case 5:
		return "SEE";
	case 6:
		return "Southeasterly";
	case 7:
		return "SSE";
	case 8:
		return "Southerly";
	case 9:
		return "SSW";
	case 10:
		return "Southwesterly";
	case 11:
		return "WSW";
	case 12:
		return "Westerly";
	case 13:
		return "WNW";
	case 14:
		return "Northwesterly";
	case 15:
		return "NNW";
	}
return "No Wind";
}

/*
	RENDER_CURRENT_READINGS_IPHONE()
	--------------------------------
*/
usb_weather_reading *render_current_readings_iphone(usb_weather *station)
{
usb_weather_reading *readings;
uint8_t year, month, day, hour, minute;
int sun_hour, sun_minute;
int daylight_savings;
TIME_ZONE_INFORMATION timezone_information;
usb_weather_fixed_block_1080 *fixed_block;

readings = station->read_current_readings();
if (readings == NULL)
	exit(printf("Cannot read current readings\n"));

fixed_block = station->read_fixed_block();

puts("<html>");
puts("<style>");
puts("@font-face");
puts("	{");
puts("	font-family: 'MeteoconsRegular';");
puts("	src: url('meteocons-webfont.eot');");
puts("	src: url('meteocons-webfont.eot?#iefix') format('embedded-opentype'),");
puts("		 url('meteocons-webfont.woff') format('woff'),");
puts("		 url('meteocons-webfont.ttf') format('truetype'),");
puts("		 url('meteocons-webfont.svg#MeteoconsRegular') format('svg');");
puts("	font-weight: normal;");
puts("	font-style: normal;");
puts("	}");
puts("body, td");
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
puts("	font-size:120pt;");
puts("	}");
puts(".megahuge");
puts("	{");
puts("	font-size:160pt");
puts("	}");
puts(".symbol");
puts("	{");
puts("	font-family:MeteoconsRegular;");
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
puts("</style>");

puts("<head></head>");

puts("<body>");
puts("<table cellpadding=0 cellspacing=0 border=0 width=100%>");

/*
	Sunrise and Sunset
*/
puts("<tr><td><table cellpadding=0 cellspacing=0 border=0 width=100%><tr><td align=left><span class=\"symbol\">B</span>");

daylight_savings = (GetTimeZoneInformation(&timezone_information) == TIME_ZONE_ID_DAYLIGHT) ? 1 : 0;
fixed_block->current_time.extract(&year, &month, &day, &hour, &minute);
weather_math::sunrise(&sun_hour, &sun_minute, year + 2000, month, day, lat, lng, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf("%d:%d", sun_hour, sun_minute);
puts("</td><td align=center><span class=\"symbol\">C</span></td><td align=right>");
weather_math::sunset(&sun_hour, &sun_minute, year + 2000, month, day, lat, lng, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf(" %d:%d", sun_hour, sun_minute);
puts("<span class=\"symbol\">A</span></td></tr></table></td></tr>");

/*
	When the readings were taken
*/
puts("<tr><td align=center>");
fixed_block->current_time.text_render();
printf(",", readings->delay);
printf(" %ld minute%s ago</b><br>", readings->delay, readings->delay == 1 ? "" : "s");
puts("</td></tr>");

/*
	Wind
*/
printf("<tr><td align=center class=\"huge\">%s</td></tr>", wind_direction(readings->wind_direction));
printf("<tr><td align=center class=\"medium\">%0.2fKn gusts to %0.2fKn (%s)</td></tr>", knots(readings->average_windspeed), knots(readings->gust_windspeed), weather_math::beaufort_name(weather_math::beaufort(readings->average_windspeed)));
puts("<tr><td class=\"halfspace\">&nbsp;</td></tr>");

/*
	Outdoor temperature, humidity, rainfall
*/
puts("<tr><td align=center><center><table cellpadding=0 cellspacing=0 border=0><tr><td align=right class=\"huge\">");
printf("%0.2f", readings->outdoor_temperature);
puts("&deg;C&nbsp;</td><td align=left class=\"medium\">");
printf("%0.2f", readings->outdoor_humidity);
puts("%<br>");
printf("%0.2f", readings->total_rain);
puts("mm</td></tr></table></center></td></tr>");
puts("<tr><td class=\"space\">&nbsp;</td></tr>");

/*
	Prediction
*/
puts("<tr><td align=center class=\"megahuge\"><span class=\"symbol\">R</span></td></tr>");
puts("<tr><td class=\"halfspace\">&nbsp;</td></tr>");

/*
	Pressure
*/
printf("<tr><td align=center class=\"medium\">%0.2fhPa <span class=\"arrowfont\">&uarr;</span></td></tr>", readings->absolute_pressure);
printf("<tr><td class=\"halfspace\">&nbsp;</td></tr>");

/*
	Inside temperature and humidity
*/
double dew_point = weather_math::dewpoint(readings->outdoor_temperature, readings->outdoor_humidity);
double apparent_temperature = weather_math::apparent_temperature(readings->outdoor_temperature, readings->outdoor_humidity, readings->average_windspeed);
//double apparent_temperature = weather_math::australian_apparent_temperature(readings->outdoor_temperature, readings->outdoor_humidity, readings->average_windspeed);

printf("<tr><td align=center class=\"medium\">Feels like %0.0f&deg;C, Dewpoint:%0.0f&deg;C, Inside:%0.0f&deg;C %0.0f%%)</td></tr>", apparent_temperature, dew_point, readings->indoor_temperature, readings->indoor_humidity);
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
TIME_ZONE_INFORMATION timezone_information;

puts("</script>");
puts("</head>");
puts("<body>");
puts("<b>Current Readings</b><br>");
printf("<b><font size=-1>");
fixed_block->current_time.text_render();
printf(":", current->delay);
printf(" (%ld minutes ago)</font></b><br>", current->delay);

daylight_savings = (GetTimeZoneInformation(&timezone_information) == TIME_ZONE_ID_DAYLIGHT) ? 1 : 0;

fixed_block->current_time.extract(&year, &month, &day, &hour, &minute);
weather_math::sunrise(&sun_hour, &sun_minute, year + 2000, month, day, lat, lng, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
printf("<b><font size=-1>Sun Rise: %d:%d", sun_hour, sun_minute);

weather_math::sunset(&sun_hour, &sun_minute, year + 2000, month, day, lat, lng, usb_weather_datetime::bcd_to_int(fixed_block->timezone), daylight_savings);
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
void render_html_graph(char *name, char *title, char *x_title, char *y_title, long elements, long *timeline, double *data)
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
printf(", ['Wind kn', %0.2f]", knots(readings->average_windspeed));
puts("]);");

puts("var wind_options =");
puts("	{");
printf("	width: %ld,\n", width);
printf("	height: %ld,\n", height);
puts("	min: 0,");
printf("	redFrom: %0.2f,\n", knots(readings->average_windspeed));
printf("	redTo: %0.2f\n", knots(readings->gust_windspeed));
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
	sum += history[current]->delay;
	timeline[current] = sum;
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
	sum += history[current]->delay;
	if (!history[current]->lost_communications)
		{
		timeline[current] = sum;
		data[current] = history[current]->total_rain;
		}
	}
render_html_graph("rain", "Cumulative Rainfall", "Minutes Ago", "millimetres", readings, timeline, data);

/*
	The remainder are from the remote unit and so an error can occur so we check the status word
*/
sum = 0;
for (current = readings - 1; current >= 0; current--)
	{
	sum += history[current]->delay;
	if (!history[current]->lost_communications)
		timeline[current] = sum;
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
		data[current] = knots(history[current]->average_windspeed);
render_html_graph("windspeed", "Wind Speed", "Minutes Ago", "Knots", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	if (!history[current]->lost_communications)
		data[current] = knots(history[current]->gust_windspeed);
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
