#pragma once

#include <ntifs.h>
#include <ntddk.h>

UNICODE_STRING device_name = RTL_CONSTANT_STRING(L"\\Device\\memory_read_write");
UNICODE_STRING device_symbolic_link = RTL_CONSTANT_STRING(L"\\??\\memory_read_write");
