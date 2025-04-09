#pragma once

#include "stdio.h"
#include "stdint.h"
#include "Windows.h"
#include "stdbool.h"
#define ConvertToWin32Thread() GetTopWindow(0x0);
#define REQUEST_KEY 0xDA

typedef bool(__fastcall* NtUserEnableWindowGDIScaledDpiMessage)(uint64_t, int);
typedef struct _USERMODE_REQUEST {
	uint8_t R;
	uint8_t G;
	uint8_t B;
} USERMODE_REQUEST, * PUSERMODE_REQUEST;