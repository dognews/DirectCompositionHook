#pragma once
#define CALLING_PROCESS (uint64_t)IoGetCurrentProcess()

uint64_t DrawingLoop(uint64_t a1, uint64_t a2, uint64_t a3) {
	if (driver_context->dwm != CALLING_PROCESS)
		return driver_context->previous_drawfunc(a1, a2, a3);
	
	DrawRect((BOX) { 50, 50, 250, 150 }, driver_context->R, driver_context->G, driver_context->B);

	return driver_context->previous_drawfunc(a1, a2, a3);
}