#pragma once
#include "log.h"
#include "modul/Imports.h"
#define DPRINT(format, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, format, __VA_ARGS__)
extern void NTAPI ServerOtherThread(void*);

extern "C" {
	NTSTATUS DriverEntry(
		PDRIVER_OBJECT  driver_object,
		PUNICODE_STRING registry_path
	)
	{
		UNREFERENCED_PARAMETER(driver_object);
		UNREFERENCED_PARAMETER(registry_path);
		HANDLE ThreadHandleCreated = { 0 };
		const auto status = PsCreateSystemThread(
			&ThreadHandleCreated,
			GENERIC_ALL,
			nullptr,
			nullptr,
			nullptr,
			ServerOtherThread,
			nullptr
		);

		if (!NT_SUCCESS(status))
		{
			return STATUS_UNSUCCESSFUL;
		}
		ZwClose(ThreadHandleCreated);
		return STATUS_SUCCESS;
	}
}