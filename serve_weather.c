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

/*
	These are the USB VID and PID of the weather station I've got
*/
#define USB_WEATHER_VID 0x1941			// Dream Link (in my case DIGITECH)
#define USB_WEATHER_PID 0x8021			// WH1080 Weather Station / USB Missile Launcher (in my case USB Wireless Weather Station)

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
}

/*
	RENDER_HTML_FOOTER()
	--------------------
*/
void render_html_footer(void)
{
puts("</script>");
puts("</head>");
puts("<body>");
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

printf("google.setOnLoadCallback(drawChart_%s);", name);

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
printf("}");
}

/*
	KNOTS()
	-------
*/
double knots(double m_per_second)
{
return m_per_second * 1.943844492457398;
}

/*
	DRAW_WEATHER_STATION()
	----------------------
*/
void manage_weather_station(usb_weather *station)
{
long sum;
long *timeline;
double *data, speed;
usb_weather_reading **history;
usb_weather_fixed_block_1080 *block;
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

render_html_header();

for (current = readings - 1; current >= 0; current--)
	data[current] = history[current]->indoor_temperature / 10.0;
render_html_graph("indoor_temperature", "Indoor Temperature", "Minutes Ago", "Degrees C", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	data[current] = history[current]->indoor_humidity;
render_html_graph("indoor_humidity", "Indoor Humidity", "Minutes Ago", "Percent", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	data[current] = history[current]->absolute_pressure / 10.0;
render_html_graph("pressure", "Pressure", "Minutes Ago", "Hectopascals", readings, timeline, data);


/*
	The remainder are from the remote unit and so an error can occur so we check the status word
*/
sum = 0;
for (current = readings - 1; current >= 0; current--)
	{
	if (!(history[current]->status & 0x40))		// if status & 0x40 then we've lost contact with the sensors
		timeline[current] = sum;
	sum += history[current]->delay;
	}

for (current = readings - 1; current >= 0; current--)
	if (!(history[current]->status & 0x40))		// if status & 0x40 then we've lost contact with the sensors
		data[current] = history[current]->outdoor_temperature / 10.0;
render_html_graph("outdoor_temperature", "Outdoor Temperature", "Minutes Ago", "Degrees C", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	if (!(history[current]->status & 0x40))		// if status & 0x40 then we've lost contact with the sensors
		data[current] = history[current]->outdoor_humidity;;
render_html_graph("outdoor_humidity", "Outdoor Humidity", "Minutes Ago", "Percent", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	{
	speed = history[current]->average_windspeed_low | (history[current]->windspeed_high & 0x0F);
	if (!(history[current]->status & 0x40))		// if status & 0x40 then we've lost contact with the sensors
		data[current] = knots(speed / 10.0);
	}
render_html_graph("windspeed", "Wind Speed", "Minutes Ago", "Knots", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	{
	speed = history[current]->gust_windspeed_low | ((history[current]->windspeed_high >> 4) & 0x0F);
	if (!(history[current]->status & 0x40))		// if status & 0x40 then we've lost contact with the sensors
		data[current] = knots(speed / 10.0);
	}
render_html_graph("gust", "Wind Gust", "Minutes Ago", "Knots", readings, timeline, data);

for (current = readings - 1; current >= 0; current--)
	if (history[current]->status == 0)			// else we've either lost contact or the "bucket" is full
		data[current] = history[current]->total_rain * 0.3;
render_html_graph("rain", "Rainfall", "Minutes Ago", "millimetres", readings, timeline, data);

/*
	uint8_t  wind_direction;			// multiply by 22.5 to get degrees from north
*/

render_html_footer();
}


/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
usb_weather station;

if (station.connect(USB_WEATHER_VID, USB_WEATHER_PID) == 0)
	manage_weather_station(&station);
else
	exit(printf("Cannot find an attached weather station\n"));

return 0;
}
