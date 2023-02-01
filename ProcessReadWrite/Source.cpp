#include <ntifs.h>
#include <ntddk.h>
#include "ProcessAttachGuard.h"
#include "ReadWriteCommon.h"
#include "Consts.h"


VOID driver_unload(IN PDRIVER_OBJECT driver_object)
{
	// We don't need to check anything here because if we get here,
	// that's mean that the driver entry routine finish successfully
	KdPrint(("Memory Read Write Unload\n"));
	IoDeleteSymbolicLink(&device_symbolic_link);
	IoDeleteDevice(driver_object->DeviceObject);
}

NTSTATUS driver_create_close(_In_ PDEVICE_OBJECT, _In_ PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS read_command(PIO_STACK_LOCATION stack, PIRP irp)
{
	if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(IOCTL_READ_64))
	{
		return STATUS_BUFFER_TOO_SMALL;
	}
	auto* data = static_cast<IOCTL_READ_64*>(irp->AssociatedIrp.SystemBuffer);
	if (nullptr == data)
	{
		return STATUS_BAD_DATA;
	}
	if (stack->Parameters.DeviceIoControl.OutputBufferLength != data->size)
	{
		return STATUS_INVALID_PARAMETER;
	}

	ProcessAttachGuard process_attach_guard;
	process_attach_guard.init(data->pid);

	NTSTATUS status = STATUS_SUCCESS;
	__try
	{
		RtlCopyMemory(irp->AssociatedIrp.SystemBuffer, reinterpret_cast<void*>(data->address),
		              data->size);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_ACCESS_DENIED;
	}


	irp->IoStatus.Status = status;
	irp->IoStatus.Information = data->size;

	return status;
}

NTSTATUS write_command(PIO_STACK_LOCATION stack, PIRP irp)
{
	if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(IOCTL_WRITE_64))
	{
		return STATUS_BUFFER_TOO_SMALL;
	}
	auto* data = static_cast<IOCTL_WRITE_64*>(irp->AssociatedIrp.SystemBuffer);
	if (nullptr == data)
	{
		return STATUS_INVALID_PARAMETER;
	}
	if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(IOCTL_WRITE_64) + data->size)
	{
		return STATUS_BUFFER_TOO_SMALL;
	}
	const auto* data_to_write =
		&(static_cast<unsigned char*>(irp->AssociatedIrp.SystemBuffer)[sizeof(IOCTL_WRITE_64)]);


	ProcessAttachGuard process_attach_guard;
	process_attach_guard.init(data->pid);

	NTSTATUS status = STATUS_SUCCESS;

	__try
	{
		RtlCopyMemory(reinterpret_cast<void*>(data->address), data_to_write, data->size);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_ACCESS_DENIED;
	}


	irp->IoStatus.Status = status;
	irp->IoStatus.Information = 0;

	return status;
}

NTSTATUS driver_device_control(PDEVICE_OBJECT, PIRP irp)
{
	auto stack = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status;

	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_READ_PROCESS:
		KdPrint(("Recive IOCTL Read\n"));

		status = read_command(stack, irp);

		break;
	case IOCTL_WRITE_PROCESS:
		KdPrint(("Recive IOCTL Write\n"));

		status = write_command(stack, irp);

		break;
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
	}

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING)
{
	KdPrint(("Memory Read Write DriverEntry\n"));

	driver_object->DriverUnload = driver_unload;

	driver_object->MajorFunction[IRP_MJ_CLOSE] = driver_create_close;
	driver_object->MajorFunction[IRP_MJ_CREATE] = driver_create_close;

	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driver_device_control;

	PDEVICE_OBJECT device_object;
	NTSTATUS status;

	status = IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE,
	                        &device_object);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Failed to create device"));
		return status;
	}

	status = IoCreateSymbolicLink(&device_symbolic_link, &device_name);

	if (!NT_SUCCESS(status))
	{
		KdPrint(("Failed to create symbolic link to device"));
		IoDeleteDevice(device_object);
		return status;
	}

	KdPrint(("read write driver entery finish successfully"));

	return STATUS_SUCCESS;
}
