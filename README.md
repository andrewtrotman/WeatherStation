The Weatherstation

This is a cgi-bin program for Apache-2 running on a Raspberry Pi (or IIS on Windows).  It interfaces
between the computer and a WH1080 remote weather station.  Connect to it from your iPhone 4 and you
get a lovely disply of the data from the weather station.

How to build and install
1.  Install Apache 2

2.  hg clone https://andrewtrotman@bitbucket.org/andrewtrotman/weatherstation

3.  make

4.  make install

5.  plug in the weather station

6.  connect to localhost/cgi-bin/serve_weather.app

Enjoy.

