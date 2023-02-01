#pragma once

#include <ntifs.h>


class ProcessAttachGuard final
{
public:
	ProcessAttachGuard();
	NTSTATUS init(ULONG pid);

	ProcessAttachGuard(const ProcessAttachGuard&) = delete;
	ProcessAttachGuard(ProcessAttachGuard&&) = delete;
	ProcessAttachGuard operator=(const ProcessAttachGuard&) = delete;
	ProcessAttachGuard operator=(ProcessAttachGuard&&) = delete;

	~ProcessAttachGuard();


private:
	KAPC_STATE m_apc_state;
	PEPROCESS m_process;
};
