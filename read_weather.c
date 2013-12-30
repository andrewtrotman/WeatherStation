/*
	READ_WEATHER.C
	--------------
	Copyright (c) 2012-2013 Andrew Trotman
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
	Program parameters
*/
long print_fixed_block = false;
long print_history = false;

/*
	MANAGE_WEATHER_STATION()
	------------------------
*/
void manage_weather_station(usb_weather *station)
{
usb_weather_reading *reading;
usb_weather_fixed_block_1080 *block;
uint16_t address;
uint16_t current;

/*
	Get the "fixed block" which also contains the base address of the current reading
	and the number of historic readings held in the base unit
*/
if ((block = station->read_fixed_block()) == NULL)
	exit(printf("Cannot read from base station\n"));

/*
	Print the fixed-block (decode the first 256 bytes of memory (0x00-0xFF))
*/
if (print_fixed_block)
	{
	block->text_render();
	puts("");
	}
/*
	Print the historic readings
*/
if (print_history)
	{
	address = (block->current_position - 0x100) - ((block->data_count - 1) * 16);
	address += 0x100;

	puts("HISTORIC READINGS");
	printf("-----------------");

	for (current = 0; current < block->data_count - 1; current++)
		{
		reading = station->read_reading(address);
		reading->text_render();
		delete reading;

		address += 16;
		if (address  < 0x100)
			address = 0x100;			// wrap around to 0x100
		}
	}

/*
	Print the current readings
*/
address = block->current_position;

if ((reading = station->read_reading(address)) == NULL)
	exit(printf("Can't get a reading from the weather station\n"));

reading->text_render("\nCURRENT READINGS\n----------------");
delete reading;
}

/*
	HELP()
	------
*/
void help(void)
{
puts("COMMAND LINE ARGUMENTS");
puts("----------------------");
puts("-?                            : display this message");
puts("-base                         : display the statistics held in the base unit (the fixed-block)");
puts("-short                        : display the current readings only [default]");
puts("-history                      : display historic readings");
puts("");
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
usb_weather *station;
long parameter;

for (parameter = 1; parameter < argc; parameter++)
	{
	if (strcmp(argv[parameter], "-?") == 0)
		{
		help();
		return 0;
		}
	else if (strcmp(argv[parameter], "-base") == 0)
		print_fixed_block = true;
	else if (strcmp(argv[parameter], "-short") == 0)
		print_history = false;
	else if (strcmp(argv[parameter], "-history") == 0)
		print_history = true;
	else
		{
		help();
		return 0;
		}
	}

station = new usb_weather;
if (station->connect(USB_WEATHER_VID, USB_WEATHER_PID) == 0)
	manage_weather_station(station);
else
	puts("Cannot find an attached weather station");
delete station;

return 0;
}

