#pragma once


struct IOCTL_READ_64
{
	ULONG pid;
	UINT64 address;
	ULONG size;
};

struct IOCTL_WRITE_64
{
	ULONG pid;
	UINT64 address;
	ULONG size;
};

#define READ_WRITE_DRIVER_DEVICE_READ 0x8000
#define READ_WRITE_DRIVER_DEVICE_WRITE 0x8001

#define IOCTL_READ_PROCESS CTL_CODE(READ_WRITE_DRIVER_DEVICE_READ, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_WRITE_PROCESS	 CTL_CODE(READ_WRITE_DRIVER_DEVICE_WRITE, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

