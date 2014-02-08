#
# Weather Station Makefile for Linux
#
# Copyright (c) 2013, 2014 Andrew Trotman
#

CC = $(CXX)
CFLAGS = $(CXXFLAGS) -g -pthread

#
all : read_weather.app serve_weather.app

%.o:%.c
	$(CC) $(CFLAGS) -c $<

OBJECTS = 				\
	usb_weather_datetime.o 		\
	usb_weather_reading.o 		\
	usb_weather_fixed_block_1080.o 	\
	usb_weather.o 			\
	weather_math.o 


read_weather.app : read_weather.c $(OBJECTS)
	$(CC) $(CFLAGS) -o read_weather.app read_weather.c $(OBJECTS)

serve_weather.app : serve_weather.c $(OBJECTS)
	$(CC) $(CFLAGS) -o serve_weather.app serve_weather.c $(OBJECTS)

install:
	sudo cp serve_weather.app /usr/lib/cgi-bin/serve_weather.app
	sudo chmod +s /usr/lib/cgi-bin/serve_weather.app
	sudo cp artill_clean_icons-webfont.* /var/www
	sudo cp moon_phases-webfont.* /var/www
	sudo cp apple-touch-icon.png /var/www
	sudo cp startup.jpg /var/www
	sudo cp background.jpg /var/www

clean:
	rm *.o read_weather.app serve_weather.app 

