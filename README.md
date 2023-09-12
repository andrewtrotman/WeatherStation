The Weatherstation

Software to control the WH1080 compatible weather stations over the USB. Currently works for Windows and Linux (remember to sudo first).
There are two example apps. read_weather is a commnad line program that downloads data from the weather station and displays it in a console window.
Serve_weather, is a cgi-bin executable that interrogates the weather station and sends the data as a web page to an iPhone or iPad. It runs on either Windows or Linux (on a Raspberry Pi).
Also included is phase of the moon, sun rise and sun set.

Details

This is a cgi-bin program for Apache-2 running on a Raspberry Pi (or IIS on Windows).  It interfaces
between the computer and a WH1080 remote weather station.  Connect to it from your iPhone 4 and you
get a lovely disply of the data from the weather station.

How to build and install
1.  Install Apache 2

2.  git clone this repo

3.  make

4.  make install

5.  plug in the weather station

6.  connect to localhost/cgi-bin/serve_weather.app

Enjoy.

