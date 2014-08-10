/*
	USB_WEATHER.C
	-------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#include <float.h>
#include <errno.h>
#include <stdlib.h>
#include "usb_weather.h"
#include "usb_weather_message.h"
#include "usb_weather_reading_raw.h"

#ifdef _MSC_VER
	extern "C"
	{
	#include <hidsdi.h>
	#include <hidpi.h>
	#include <SetupAPI.h>
	}
#else
	#include <linux/hidraw.h>
	#include <sys/ioctl.h>
	#include <sys/file.h>
	#include <fcntl.h>	
	#include <stdio.h>
	#include <unistd.h>
#endif
#include <string.h>

/*
	DECODE()
	--------
	Take a two byte integer read from the base unit and turn it into a uint16_t by ensuring the
	correct byte order decoding is done

	The Raspberry Pi and Intel are both Little Endian so no decoding is necesary!
*/
inline uint16_t decode(uint16_t value)
{
#ifdef _MSC_VER
	return value;
#else
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		return (value >> 8) | (value << 8);
	#else
		return value;
	#endif
#endif
}

/*
	DECODE()
	--------
*/
inline int16_t decode(int16_t value)
{
return (int16_t)decode((uint16_t)value);
}

/*
	DECODE_TEMPERATURE()
	--------------------
*/
inline int16_t decode_temperature(int16_t value)
{
uint16_t temperature = decode((uint16_t)value);

return temperature < 0x8000 ? temperature : -(temperature - 0x8000);
}

/*
	USB_WEATHER::USB_WEATHER()
	--------------------------
*/
usb_weather::usb_weather()
{
hDevice = INVALID_HANDLE_VALUE;
fixed_block = NULL;
}

/*
	USB_WEATHER::~USB_WEATHER()
	---------------------------
*/
usb_weather::~usb_weather()
{
if (hDevice != INVALID_HANDLE_VALUE)
	{
	#ifdef _MSC_VER
		CloseHandle(hDevice);
	#else
		flock(hDevice, LOCK_UN);
		close(hDevice);
	#endif
	}
delete fixed_block;
}

#ifdef _MSC_VER
	/*
		USB_WEATHER::CONNECT()
		----------------------
	*/
	uint32_t usb_weather::connect(uint32_t vid, uint32_t pid)
	{
	GUID guid;
	HDEVINFO hDevInfo;
	SP_DEVICE_INTERFACE_DATA devInfoData;
	long current;
	DWORD required;
	SP_DEVICE_INTERFACE_DETAIL_DATA *detail_data = NULL;
	HIDD_ATTRIBUTES attributes;
	long found = false;

	/*
		Get the GUID of the Windows HID class so that we can identify HID devices
	*/
	HidD_GetHidGuid(&guid);

	/*
		Get the list of HID devices
	*/
	if ((hDevInfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE)) == INVALID_HANDLE_VALUE)
		return 1;		// puts("SetupDiGetClassDevs() failure - can't connect to weather station");

	/*
		Iterate through the list of HID devices
	*/
	devInfoData.cbSize = sizeof(devInfoData);
	for (current = 0; SetupDiEnumDeviceInterfaces(hDevInfo, 0, &guid, current, &devInfoData); current++)
		{
		required = 0;
		/*
			Call SetupDiGetDeviceInterfaceDetail() to get the length of the device name then call it again to get the name itself
		*/
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &required, NULL);

		/*
			Allocate space for the name of the HID device
		*/
		free(detail_data);
		detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(required);
		detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);	// should be 5

		/*
			Get the name and details of the HID device
		*/
		if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detail_data, required, NULL, NULL))
			{
			if ((hDevice = CreateFile(detail_data->DevicePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
				{
				COMMTIMEOUTS timeout = {1000, 0, 1000, 0, 1000};		// reads and writes timeout in one second

				SetCommTimeouts(hDevice, &timeout);
				attributes.Size = sizeof(attributes);
				HidD_GetAttributes(hDevice, &attributes);
				if (attributes.VendorID == vid && attributes.ProductID == pid)
					{
					HidD_FlushQueue(hDevice);
					if (read_fixed_block() != NULL)
						{
						found = true;
						break;
						}
					}
				CloseHandle(hDevice);
				}
			}
		}

	/*
		Clean up the memory allocated when we got the device list
	*/
	SetupDiDestroyDeviceInfoList(hDevInfo);
	free(detail_data);

	/*
		Return an error code (or 0 for success)
	*/
	if (found)
		return 0;
	else
		{
		hDevice = INVALID_HANDLE_VALUE;
		return 2;	// puts("Cannot find an attached weather station");
		}
	}
#else
	/*
		Linux versions of the USB methods
	*/

	uint8_t hid_report_number = 0;

	/*
		USB_WEATHER::CONNECT()
		----------------------
	*/
	uint32_t usb_weather::connect(uint32_t vid, uint32_t pid)
	{
	char message_buffer[32];
	struct hidraw_devinfo device;
	long id;
	int file, got;

	hDevice = INVALID_HANDLE_VALUE;
	for (id = 0; id < 1024; id++)
		{
		sprintf(message_buffer, "/dev/hidraw%ld", id);
		if ((file = open(message_buffer, O_RDWR)) == -1)
			return 1;		// can't connect to the weather station
		else 
			{
			do
				{
				if ((got = flock(file, LOCK_EX | LOCK_NB)) != 0)
					{
					if (errno == EWOULDBLOCK)
						return 4;			// would block (another process is using the device)
					}
				}
			while (got != 0);
			if (ioctl(file, HIDIOCGRAWINFO, &device) >= 0 && (uint16_t)device.vendor == (uint16_t)vid && (uint16_t)device.product == (uint16_t)pid)
				{
				hDevice = file;
				if (read_fixed_block() == NULL)
					return 3;	// cannot read the fixed block.
				return 0;
				}
			flock(file, LOCK_UN);
			close(file);
			}
		}
	return 2;				// Can't find an attached weather station
	}

	/*
		READFILE()
		----------
	*/
	long ReadFile(HANDLE hDevice, void *buffer, DWORD bytes_to_read, DWORD *bytes_read, void *ignore)
	{
	DWORD remaining, got;
	uint8_t *into;

	into = (uint8_t *)buffer;

	/*
		For compatibility with Windows we write the report number at the start and return one too many characters
	*/
	*into++ = hid_report_number;
	*bytes_read = 1;

	remaining = bytes_to_read;
	while (remaining > 0)
		{
		if ((got = read(hDevice, into, remaining)) < 0)
			{
			*bytes_read += bytes_to_read;
			return false;
			}

		into += got;
		remaining -= got;
		}

	*bytes_read += bytes_to_read;
	return true;
	}

	/*
		HIDD_SETOUTPUTREPORT()
		----------------------
	*/
	long HidD_SetOutputReport(HANDLE hDevice, void *message, DWORD message_length)
	{
	hid_report_number = *(uint8_t *)message;
	return write(hDevice, message, message_length) == message_length;
	}

#endif

/*
	USB_WEATHER::READ()
	-------------------
*/
uint32_t usb_weather::read(uint16_t address, void *result)
{
uint32_t bytes = 32;
unsigned char recieve_buffer[64];
uint8_t *into = (uint8_t *)result;
usb_weather_message message;
DWORD dwBytes = 0, dwBytesToRead;
long long remaining;

message.zero = 0;
message.report_id = message.aux_report_id = 0xa1;
message.address_high = message.aux_address_high = (address >> 8) & 0xFF;
message.address_low = message.aux_address_low = address & 0xFF; 
message.end_of_message = message.aux_end_of_message = 0x20;

/*
	Transmit the read request to the weather station
*/
if (HidD_SetOutputReport(hDevice, &message, sizeof(message)))
	{
	/*
		Recieve the reply
	*/
	remaining = bytes;
	while (remaining > 0)
		{
#ifdef _MSC_VER
		dwBytesToRead = bytes;
#else
		dwBytesToRead = remaining;
#endif
		if (!ReadFile(hDevice, recieve_buffer, dwBytesToRead, &dwBytes, NULL))
			return 0;

		dwBytes -= 1;	// skip over the report ID
		memcpy(into, recieve_buffer + 1, (size_t)dwBytes);
		into += dwBytes;
		remaining -= dwBytes;
		}
	return bytes;		// success
	}

return 0;		// failure
}

/*
	USB_WEATHER::READ_READING()
	---------------------------
*/
usb_weather_reading *usb_weather::read_reading(uint16_t address)
{
usb_weather_reading *answer;
usb_weather_reading_raw buffer;
long trial;
static const long MAX_TRIALS = 3;			// maximum number of attempts to read before timeout

trial = 0;
while ((read(address, (uint8_t*)&buffer) == 0) && (trial < MAX_TRIALS))
	trial++;
if (trial == MAX_TRIALS)
	return NULL;							// failed to read from device so timeout

/*
	Convert it into human usable units
*/
answer = new usb_weather_reading;
answer->delay = buffer.delay;
answer->indoor_humidity = buffer.indoor_humidity;
answer->indoor_temperature = decode_temperature(buffer.indoor_temperature) / 10.0;
answer->outdoor_humidity = buffer.outdoor_humidity;
answer->outdoor_temperature = decode_temperature(buffer.outdoor_temperature) / 10.0;
answer->absolute_pressure = decode(buffer.absolute_pressure) / 10.0;
answer->average_windspeed = (buffer.average_windspeed_low | (buffer.windspeed_high & 0x0F)) / 10.0;
answer->gust_windspeed = (buffer.gust_windspeed_low | ((buffer.windspeed_high >> 4) & 0x0F)) / 10.0;
answer->wind_direction = buffer.wind_direction * 22.5;
answer->total_rain = decode(buffer.total_rain) * 0.3;
answer->rain_counter_overflow = buffer.status & 0x80 ? true : false;
answer->lost_communications = buffer.status & 0x40 ? true : false;

/*
	Pass it back to the caller
*/
return answer;
}

/*
	USB_WEATHER::READ_FIXED_BLOCK()
	-------------------------------
*/
usb_weather_fixed_block_1080 *usb_weather::read_fixed_block(void)
{
uint8_t *into;
uint16_t address = 0;

/*
	If we've already read it then don't read it again
*/
if (fixed_block != NULL)
	return fixed_block;

/*
	Allocate memory for it
*/
fixed_block = new usb_weather_fixed_block_1080;

/*
	Read into it... in 32-byte chunks
*/
into = (uint8_t *)fixed_block;
while (address < sizeof(*fixed_block))
	{
	if (read(address, into) == 0)
		return NULL;
	into += sizeof(usb_weather_reading_raw);
	address += sizeof(usb_weather_reading_raw);
	}

return fixed_block;
}

/*
	USB_WEATHER::READ_ALL_READINGS()
	--------------------------------
	read no more that max_readings, but historically back as far as possible otherwise
*/
usb_weather_reading **usb_weather::read_all_readings(uint32_t *readings, int32_t max_readings)
{
usb_weather_reading **history;
uint16_t address;
uint32_t current;

/*
	Do we have communications?
*/
if (fixed_block == NULL)
	{
	*readings = 0;
	return NULL;
	}

/*
	How many readings to get
*/
if (max_readings < 0)
	max_readings = fixed_block->data_count;
else
	max_readings = fixed_block->data_count < max_readings ? fixed_block->data_count : max_readings;

/*
	Get the base address of the first reading
*/
address = 0x100 + ((fixed_block->current_position - 0x100) - ((max_readings - 1) * 16));

/*
	Allocate space
*/
history = new usb_weather_reading *[max_readings];

/*
	Download each reading
*/
for (current = 0; current < max_readings; current++)
	{
	history[current] = read_reading(address);
	if ((address += 16)  < 0x100)
		address = 0x100;			// wrap around to 0x100
	}

/*
	Pass the results back to the caller
*/
*readings = max_readings;
return history;
}

/*
	USB_WEATHER::READ_CURRENT_READINGS()
	------------------------------------
*/
usb_weather_reading *usb_weather::read_current_readings(void)
{
/*
	Make sure we have the address we need
*/
if (fixed_block == NULL)
	return NULL;

/*
	Get the reading
*/
return read_reading(fixed_block->current_position);
}

/*
	USB_WEATHER::READ_PREVIOUS_READINGS()
	-------------------------------------
*/
usb_weather_reading *usb_weather::read_previous_readings(void)
{
uint16_t address;

/*
	Make sure we have the address we need
*/
if (fixed_block == NULL)
	return NULL;

if (fixed_block->data_count <= 1)
	return NULL;		// Can't read the previous reading because there isn't one!

/*
	Get the reading
*/
if ((address = fixed_block->current_position - 16) < 0x100)
	address = 0x10000 - sizeof(usb_weather_reading_raw);

return read_reading(address);
}

/*
	USB_WEATHER::READ_HOURLY_DELTA()
	--------------------------------
	Get the change over the previous hour, if we can't then we set lost_communications to true
	As the base station might not be taking readings very often (e.g. every half hour) and we want the hourly
	data, we're a bit stuck.  So we'll return an object that contains the correct data for the shortest time
	greater than 59 minutes.  To linearly interpolate that data (to one hour) call interpolate_hourly_data()
*/
usb_weather_reading *usb_weather::read_hourly_delta(void)
{
uint16_t max_reads, time, address;
usb_weather_reading *now, *previous;
uint8_t rain_overflow, finish;

/*
	Do we have communications?
*/
if (fixed_block == NULL)
	return NULL;

address = fixed_block->current_position;
max_reads = fixed_block->data_count;
if ((previous = now = read_reading(address)) == NULL)
	return NULL;

rain_overflow = now->rain_counter_overflow;
time = now->delay;
max_reads--;
finish = max_reads <= 0;
while (!finish)
	{
	if ((address -= 16) < 0x100)
		address = 0x10000 - sizeof(usb_weather_reading_raw);
	if ((previous = read_reading(address)) == NULL)
		{
		delete now;
		return NULL;
		}
	rain_overflow = rain_overflow || previous->rain_counter_overflow;
	time += previous->delay;
	max_reads--;
	if (max_reads <= 0)
		finish = true;
	if (time > 60 && !previous->lost_communications)
		finish = true;
	else
		delete previous;
	}

now->delay = time;
now->indoor_humidity -= previous->indoor_humidity;
now->indoor_temperature -= previous->indoor_temperature;
now->outdoor_humidity -= previous->outdoor_humidity;
now->outdoor_temperature -= previous->outdoor_temperature;
now->absolute_pressure -= previous->absolute_pressure;
now->average_windspeed -= previous->average_windspeed;
now->gust_windspeed -= previous->gust_windspeed;
now->wind_direction -= (int)previous->wind_direction % 360;
now->total_rain -= previous->total_rain;
now->rain_counter_overflow = rain_overflow;
now->lost_communications = max_reads < 0;					// not lost_communications, more "did we manage to read an hour's worth?"

return now;
}

/*
	USB_WEATHER::INTERPOLATE_HOURLY_DELTA()
	---------------------------------------
*/
usb_weather_reading *usb_weather::interpolate_hourly_delta(usb_weather_reading *delta)
{
usb_weather_reading *hourly_delta;

hourly_delta = new usb_weather_reading;

hourly_delta->delay = 60;
hourly_delta->indoor_humidity = (delta->indoor_humidity / delta->delay) * 60;
hourly_delta->indoor_temperature = (delta->indoor_temperature / delta->delay) * 60;
hourly_delta->outdoor_humidity = (delta->outdoor_humidity / delta->delay) * 60;
hourly_delta->outdoor_temperature = (delta->outdoor_temperature / delta->delay) * 60;
hourly_delta->absolute_pressure = (delta->absolute_pressure / delta->delay) * 60;
hourly_delta->average_windspeed = (delta->average_windspeed / delta->delay) * 60;
hourly_delta->gust_windspeed = (delta->gust_windspeed  / delta->delay) * 60;
hourly_delta->wind_direction = delta->wind_direction;
hourly_delta->total_rain = (delta->total_rain / delta->delay) * 60;
hourly_delta->rain_counter_overflow = delta->rain_counter_overflow;
hourly_delta->lost_communications = delta->lost_communications;

return hourly_delta;
}

/*
	USB_WEATHER::READ_HIGHS_AND_LOWS()
	----------------------------------
*/
usb_weather_reading *usb_weather::read_highs_and_lows(usb_weather_reading *highs, usb_weather_reading *lows, uint32_t since_minutes_ago)
{
uint16_t max_reads, time, address;
usb_weather_reading *reading;
uint8_t rain_overflow, lost_communications, finish;

lows->indoor_humidity = lows->outdoor_humidity = lows->indoor_temperature = lows->outdoor_temperature = lows->absolute_pressure = lows->average_windspeed = lows->gust_windspeed = lows->total_rain = DBL_MAX;
highs->indoor_humidity = highs->outdoor_humidity = highs->indoor_temperature = highs->outdoor_temperature = highs->absolute_pressure = highs->average_windspeed = highs->gust_windspeed = highs->total_rain = DBL_MIN;

/*
	Do we have communications?
*/
if (fixed_block == NULL)
	return NULL;

rain_overflow = false;
lost_communications = false;
address = fixed_block->current_position;
max_reads = fixed_block->data_count;
time = 0;
reading = NULL;
finish = false;

while (!finish)
	{
	delete reading;

	if ((reading = read_reading(address)) == NULL)
		return NULL;

	time += reading->delay;
#define COMPUTE_MAX_MIN(x) ((highs->x = reading->x > highs->x ? reading->x : highs->x), (lows->x = reading->x < lows->x ? reading->x : lows->x))
	COMPUTE_MAX_MIN(indoor_humidity);
	COMPUTE_MAX_MIN(indoor_temperature);
	COMPUTE_MAX_MIN(outdoor_humidity);
	COMPUTE_MAX_MIN(outdoor_temperature);
	COMPUTE_MAX_MIN(absolute_pressure);
	COMPUTE_MAX_MIN(average_windspeed);
	COMPUTE_MAX_MIN(gust_windspeed);
	COMPUTE_MAX_MIN(total_rain);
#undef COMPUTE_MAX_MIN
	rain_overflow = rain_overflow || reading->rain_counter_overflow;
	lost_communications = lost_communications || reading->lost_communications;

	max_reads--;
	if (max_reads <= 0)
		finish = true;
	if (time > 60 * 24)
		finish = true;

	if ((address -= 16) < 0x100)
		address = 0x10000 - sizeof(usb_weather_reading_raw);
	}

highs->delay = lows->delay = time;
highs->rain_counter_overflow = lows->rain_counter_overflow = rain_overflow;
highs->lost_communications = lows->lost_communications = lost_communications;

return highs;
}
