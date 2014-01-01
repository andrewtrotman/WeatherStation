/*
	USB_WEATHER.C
	-------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#include "usb_weather.h"
#include "usb_weather_message.h"
#include "usb_weather_reading_raw.h"

extern "C"
{
#include <hidsdi.h>
#include <hidpi.h>
#include <SetupAPI.h>
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
	CloseHandle(hDevice);
delete fixed_block;
}

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

/*
	USB_WEATHER::READ()
	-------------------
*/
uint32_t usb_weather::read(uint16_t address, void *result)
{
uint32_t bytes = 32;
unsigned char recieve_buffer[32];
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
		dwBytesToRead = sizeof(recieve_buffer);
		if (!ReadFile(hDevice, recieve_buffer, dwBytesToRead, &dwBytes, NULL))
			return 0;

		dwBytes -= 1;	// skip over the report ID
		memcpy(into, recieve_buffer + 1, (size_t)(dwBytes > remaining ? remaining : dwBytes));

		into += dwBytes > remaining ? remaining : dwBytes;
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

/*
	Get the data
*/
if (read(address, (uint8_t*)&buffer) == 0)
	return NULL;

/*
	Convert it into human usable units
*/
answer = new usb_weather_reading;
answer->delay = buffer.delay;
answer->indoor_humidity = buffer.indoor_humidity;
answer->indoor_temperature = buffer.indoor_temperature / 10.0;
answer->outdoor_humidity = buffer.outdoor_humidity;
answer->outdoor_temperature = buffer.outdoor_temperature / 10.0;
answer->absolute_pressure = buffer.absolute_pressure / 10.0;
answer->average_windspeed = (buffer.average_windspeed_low | (buffer.windspeed_high & 0x0F)) / 10.0;
answer->gust_windspeed = (buffer.gust_windspeed_low | ((buffer.windspeed_high >> 4) & 0x0F)) / 10.0;
answer->wind_direction = buffer.wind_direction * 22.5;
answer->total_rain = buffer.total_rain * 0.3;
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
*/
usb_weather_reading **usb_weather::read_all_readings(uint32_t *readings)
{
usb_weather_reading **history;
uint16_t address;
uint32_t current;

/*
	Do we have communications?
*/
if (fixed_block == NULL)
	return NULL;

/*
	Get the base address of the first reading
*/
address = (fixed_block->current_position - 0x100) - ((fixed_block->data_count - 1) * 16);
address += 0x100;

/*
	Allocate space
*/
history = new usb_weather_reading *[fixed_block->data_count];

/*
	Download each reading
*/
for (current = 0; current < fixed_block->data_count; current++)
	{
	history[current] = read_reading(address);
	if ((address += 16)  < 0x100)
		address = 0x100;			// wrap around to 0x100
	}

/*
	Pass the results back to the caller
*/
*readings = fixed_block->data_count;
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

