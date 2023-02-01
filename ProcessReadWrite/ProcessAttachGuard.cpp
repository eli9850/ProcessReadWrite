#include "ProcessAttachGuard.h"

#include <ntddk.h>


ProcessAttachGuard::ProcessAttachGuard() : m_apc_state{}, m_process(nullptr)
{
}

NTSTATUS ProcessAttachGuard::init(ULONG pid)
{
	const NTSTATUS status = PsLookupProcessByProcessId(UlongToHandle(pid), &m_process);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	KeStackAttachProcess(m_process, &m_apc_state);
	return STATUS_SUCCESS;
}


ProcessAttachGuard::~ProcessAttachGuard()
{
	__try
	{
		if (nullptr != m_process)
		{
			ObDereferenceObject(m_process);
			KeUnstackDetachProcess(&m_apc_state);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}
