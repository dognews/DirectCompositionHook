#pragma once

void* GetUsermodeModuleExport(char* module_name, char* function_name) {
	HMODULE target_module = GetModuleHandleA(module_name);
	if (target_module == INVALID_HANDLE_VALUE)
		return NULL;

	void* function = GetProcAddress(target_module, function_name);
	return function;
}