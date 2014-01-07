#
# Weather Station Makefile for Linux
#
# Copyright (c) 2013, 2014 Andrew Trotman
#

CC = $(CXX)
CFLAGS = $(CXXFLAGS) -g

#
all : read_weather.app serve_weather.app

read_weather.app : read_weather.c usb_weather_datetime.c usb_weather_reading.c usb_weather_fixed_block_1080.c usb_weather.c usb_weather_datetime.h usb_weather_reading.h usb_weather_fixed_block_1080.h usb_weather.h
	$(CC) $(CFLAGS) -o read_weather.app read_weather.c usb_weather_datetime.c usb_weather_reading.c usb_weather_fixed_block_1080.c usb_weather.c 

serve_weather.app : serve_weather.c usb_weather_datetime.c usb_weather_reading.c usb_weather_fixed_block_1080.c usb_weather.c weather_math.c usb_weather_datetime.h usb_weather_reading.h usb_weather_fixed_block_1080.h usb_weather.h weather_math.h
	$(CC) $(CFLAGS) -o serve_weather.app serve_weather.c usb_weather_datetime.c usb_weather_reading.c usb_weather_fixed_block_1080.c usb_weather.c weather_math.c 

clean:
	rm *.obj *.exe *.ilk *.pdb *.suo *.bak *.o read_weather.app serve_weather.app a.out
