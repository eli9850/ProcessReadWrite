#include <iostream>
#include <Windows.h>

int main()
{
	std::cout << std::hex << GetCurrentProcessId() << std::endl;
	int a = 0x12345678;
	while (a == 0x12345678)
	{
		std::cout << &a << std::endl;
		Sleep(1000);
	}
	std::cout << a << std::endl;
}
