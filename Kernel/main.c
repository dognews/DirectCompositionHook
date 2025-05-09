#include "imports.h"
#include "defines.h"
#include "win32.h"
#include "request.h"
#include "drawing.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	// Initalize Driver Context
	if (!NT_SUCCESS(InitalizeDriverContext(&driver_context))) {
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	// Setup request handler
	if (!NT_SUCCESS(HookFunction(336LL, 760LL, &previous_requestfunc, &RequestHandler))) {
		ObDereferenceObject(driver_context->dwm);
		MmFreeContiguousMemory(driver_context);

		return STATUS_FAILED_DRIVER_ENTRY;
	}
	
	// Hook DirectComposition Present
	if (!NT_SUCCESS(HookFunction(144LL, 16LL, &driver_context->previous_drawfunc, &DrawingLoop))) {
		ObDereferenceObject(driver_context->dwm);
		MmFreeContiguousMemory(driver_context);

		return STATUS_FAILED_DRIVER_ENTRY;
	}

	// Initalize Win32 Exports
	InitalizeWin32();
	return STATUS_SUCCESS;
}
