#
# Weather Station Makefile
#
# Copyright (c) 2013 Andrew Trotman
#

#
#	Set up operating specific tools
#

CXX = $(CXX) -nologo
#
# Host specific stuff
#

EXT = exe

#
#	Which version of the Window DDK do we have?
#
VER_8 = "C:\Program Files (x86)\Windows Kits\8.0\"
!IF "$(WINDOWSSDKDIR)" == $(VER_8)
DDK_DIR = $(WINDOWSSDKDIR)\Include
DDK_INCLUDE = "$(DDK_DIR)\shared" -I"$(DDK_DIR)\km" -I"$(DDK_DIR)\km\crt" -I"$(DDK_DIR)\um"
DDK_LIB = "$(WINDOWSSDKDIR)\Lib\win8\um\x64\setupapi.lib" "$(WINDOWSSDKDIR)\Lib\win8\um\x64\hid.lib"
!ELSE
DDK_DIR = "c:\WinDDK\7600.16385.1\inc"
DDK_INCLUDE = $(DDK_DIR)\ddk -I$(DDK_DIR)\api -I$(DDK_DIR)\crt
DDK_LIB = c:\WinDDK\7600.16385.1\lib\wxp\i386\setupapi.lib c:\WinDDK\7600.16385.1\lib\wxp\i386\hid.lib
!ENDIF

all : read_weather.exe serve_weather.exe

read_weather.exe : read_weather.c usb_weather_datetime.c usb_weather_reading.c usb_weather_fixed_block_1080.c usb_weather.c usb_weather_datetime.h usb_weather_reading.h usb_weather_fixed_block_1080.h usb_weather.h
	cl /Zi /I$(DDK_INCLUDE) /Tp read_weather.c /Tp usb_weather_datetime.c /Tp usb_weather_reading.c /Tp usb_weather_fixed_block_1080.c /Tp usb_weather.c /X -I$(DDK_INCLUDE) $(DDK_LIB)

serve_weather.exe : serve_weather.c usb_weather_datetime.c usb_weather_reading.c usb_weather_fixed_block_1080.c usb_weather.c weather_math.c usb_weather_datetime.h usb_weather_reading.h usb_weather_fixed_block_1080.h usb_weather.h weather_math.h
	cl /Zi /I$(DDK_INCLUDE) /Tp serve_weather.c /Tp usb_weather_datetime.c /Tp usb_weather_reading.c /Tp usb_weather_fixed_block_1080.c /Tp usb_weather.c /Tp weather_math.c /X -I$(DDK_INCLUDE) $(DDK_LIB)

clean:
	del *.obj *.exe *.ilk *.pdb *.suo *.bak
