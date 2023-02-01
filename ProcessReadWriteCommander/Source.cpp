#include <iostream>
#include <vector>
#include <memory>
#include <Windows.h>
#include "../ProcessReadWrite/ReadWriteCommon.h"

constexpr int IO_OPERATION_INDEX = 1;
constexpr int PID_INDEX = 2;
constexpr int ADDRESS_INDEX = 3;
constexpr int SIZE_INDEX = 4;
constexpr int DATA_INDEX = 4;

using HandleGuard = std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(&CloseHandle)>;

void Error(const char* message)
{
	printf("%s (error=%d)\n", message, GetLastError());
}

void handle_read(const std::vector<std::string>& params)
{
	HandleGuard hDevice(CreateFile(L"\\\\.\\memory_read_write", FILE_READ_ACCESS | FILE_WRITE_ACCESS, 0, nullptr,
	                               OPEN_EXISTING, 0, nullptr), &CloseHandle);
	if (hDevice.get() == INVALID_HANDLE_VALUE)
	{
		Error("Failed to open device");
		return;
	}

	unsigned long size = std::strtoul(params[SIZE_INDEX].c_str(), nullptr, 10);

	IOCTL_READ_64 data{
		std::strtoul(params[PID_INDEX].c_str(), nullptr, 16),
		std::strtoull(params[ADDRESS_INDEX].c_str(), nullptr, 16),
		size
	};

	char* result = new char[size];
	DWORD returned;

	BOOL success = DeviceIoControl(hDevice.get(), IOCTL_READ_PROCESS,
	                               &data, sizeof data,
	                               result, size, &returned, nullptr);
	if (success)
	{
		for (size_t i = 0; i < sizeof(int); i++)
		{
			std::cout << std::hex << int(unsigned char(result[i])) << std::endl;
		}
	}
	else
	{
		Error("IOCTL failed");
	}
	delete[] result;
}

void handle_write(const std::vector<std::string>& params)
{
	HandleGuard hDevice(CreateFile(L"\\\\.\\memory_read_write", FILE_READ_ACCESS | FILE_WRITE_ACCESS, 0, nullptr,
		OPEN_EXISTING, 0, nullptr), &CloseHandle);
	if (hDevice.get() == INVALID_HANDLE_VALUE)
	{
		Error("Failed to open device");
		return;
	}

	const unsigned long size = params[DATA_INDEX].size();
	
	IOCTL_WRITE_64 data{
		std::strtoul(params[PID_INDEX].c_str(), nullptr, 16),
		std::strtoull(params[ADDRESS_INDEX].c_str(), nullptr, 16),
		size
	};

	char* data_to_send = new char[sizeof(IOCTL_WRITE_64) + size];
	memcpy(data_to_send, &data, sizeof(IOCTL_WRITE_64));

	memcpy(data_to_send + sizeof(IOCTL_WRITE_64), params[DATA_INDEX].c_str(), size);

	DWORD returned;
	BOOL success = DeviceIoControl(hDevice.get(), IOCTL_WRITE_PROCESS,
	                               data_to_send, sizeof(IOCTL_WRITE_64) + size,
	                               nullptr, 0, &returned, nullptr);
	if (!success)
	{
		Error("IOCTL failed");
	}
	delete[] data_to_send;
}

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		std::cout << "Invalid parameters" << std::endl;
		return 0;
	}
	std::vector<std::string> program_args(argv, argv + argc);

	if (program_args[IO_OPERATION_INDEX] == "read")
	{
		handle_read(program_args);
	}
	else if (program_args[IO_OPERATION_INDEX] == "write")
	{
		handle_write(program_args);
	}
	else
	{
		std::cout << "The first parameter must be read or write" << std::endl;
		return 0;
	}
}
