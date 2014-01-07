/*
	Linux USB tests
	Copyright (c) 2014 Andrew Trotman

	NOTE: remeber to "sudo" this program to run it
*/
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>

typedef int HANDLE;

HANDLE usb_open(uint16_t vid, uint16_t pid)
{
char message_buffer[32];
struct hidraw_devinfo device;
long id;
int file;

for (id = 0; id < 1024; id++)
	{
	sprintf(message_buffer, "/dev/hidraw%ld", id);
	if ((file = open(message_buffer, O_RDWR)) == -1)
		return -1;
	else if (ioctl(file, HIDIOCGRAWINFO, &device) >= 0 && (uint16_t)device.vendor == (uint16_t)vid && (uint16_t)device.product == (uint16_t)pid)
		return file;
	else
		close(file);
	}
return -1;
}

void usb_close(HANDLE device)
{
close(device);
}

long usb_write(HANDLE hDevice, uint8_t *buffer, long bytes)
{
return write(hDevice, buffer, 9);
}

long usb_read(HANDLE hDevice, uint8_t *buffer, long bytes)
{
long remaining, got;
uint8_t *into;

into = buffer;
remaining = bytes;
while (remaining > 0)
	{
	if ((got = read(hDevice, into, remaining)) < 0)
		return got;

	into += got;
	remaining -= got;
	}

return bytes;
}


int main(int argc, char **argv)
{
HANDLE hDevice;
char buffer[256];
int i, value;

if ((hDevice = usb_open(0x1941, 0x8021)) != -1)

/* Send a Report to the Device */
buffer[0] = 0x00;
buffer[1] = 0xA1;
buffer[2] = 0x00;
buffer[3] = 0x00;
buffer[4] = 0x20;
buffer[5] = 0xA1;
buffer[6] = 0x00;
buffer[7] = 0x00;
buffer[8] = 0x20;

if ((value = usb_write(hDevice, buffer, 9)) == 9)
	if ((value = usb_read(hDevice, buffer, 32)) == 32)
		{
		for (i = 0; i < value; i++)
			printf("%02X ", buffer[i]);
		puts("\n");
		}

usb_close(hDevice);

return 0;
}

