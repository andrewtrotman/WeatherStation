/*
	USB_WEATHER_MESSAGE.H
	---------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef USB_WEATHER_MESSAGE_H_
#define USB_WEATHER_MESSAGE_H_

#include "fundamental_types.h"

/*
	class USB_WEATHER_MESSAGE
	-------------------------
*/
#pragma pack(1)
class usb_weather_message
{
public:
	uint8_t zero;
	uint8_t report_id;						// 0xA1 to read from device
	uint8_t address_high;					// msb of address
	uint8_t address_low;						// lsb of address
	uint8_t end_of_message;					// 0x20
	uint8_t aux_report_id;					// 0xA1
	uint8_t aux_address_high;					// msb of address
	uint8_t aux_address_low;					// lbs of address
	uint8_t aux_end_of_message;				// 0x20
} ;
#pragma pack()

#endif /* USB_WEATHER_MESSAGE_H_ */
