#pragma once
#define print(string) (DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, string));
#define printf(string, other) (DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, string, other));

#include <ntifs.h>
#include <wdm.h>
#include <stdint.h>
#include <stdbool.h>
#include <windef.h>
#include <wingdi.h>

typedef uint64_t(__fastcall* NtDCompositionBeginFrame)(uint64_t, uint64_t, uint64_t);

// Windows Offsets
const uint32_t OFFSET_eprocess_object_table = 0x300; //EPROCESS->ObjectTable
const uint32_t OFFSET_eprocess_active_links = 0x1d8; //EPROCESS->ActiveProcessLinks
const uint32_t OFFSET_eprocess_image_name = 0x338; //EPROCESS->ImageFileName

// Driver Context
typedef struct _DRIVER_CONTEXT {
	NtDCompositionBeginFrame previous_drawfunc;
	PEPROCESS dwm;

	volatile uint8_t R;
	volatile uint8_t G;
	volatile uint8_t B;
} DRIVER_CONTEXT, *PDRIVER_CONTEXT;

PDRIVER_CONTEXT driver_context = NULL;