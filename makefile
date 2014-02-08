#
# Weather Station Makefile
#
# Copyright (c) 2013 Andrew Trotman
#

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

CC = @cl
CFLAGS = /nologo /Zi /I$(DDK_INCLUDE)

.c.obj :
	$(CC) $(CFLAGS) /c /Tp $<

all : read_weather.exe serve_weather.exe

OBJECTS = 								\
	usb_weather_datetime.obj 			\
	usb_weather_reading.obj 			\
	usb_weather_fixed_block_1080.obj	\
	usb_weather.obj						\
	weather_math.obj					\
	mutex.obj


read_weather.exe : read_weather.c $(OBJECTS)
	$(CC) $(CFLAGS) /Tp read_weather.c $(OBJECTS) /X -I$(DDK_INCLUDE) $(DDK_LIB)

serve_weather.exe : serve_weather.c
	$(CC) $(CFLAGS) /Tp serve_weather.c $(OBJECTS) /X -I$(DDK_INCLUDE) $(DDK_LIB)

clean:
	del *.obj *.exe *.ilk *.pdb *.suo *.bak
