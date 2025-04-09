#pragma once

// Usermode Request
typedef struct _USERMODE_REQUEST {
	uint8_t R;
	uint8_t G;
	uint8_t B;
} USERMODE_REQUEST, *PUSERMODE_REQUEST;

NtUserEnableWindowGDIScaledDpiMessage previous_requestfunc = NULL;

bool __fastcall RequestHandler(uint64_t p1, int request_key) {
	if (request_key != 0xDA)
		return previous_requestfunc(p1, request_key);

	PUSERMODE_REQUEST request = p1;

	InterlockedExchange8(&driver_context->R, request->R);
	InterlockedExchange8(&driver_context->G, request->G);
	InterlockedExchange8(&driver_context->B, request->B);

	return true;
}