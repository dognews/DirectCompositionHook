#pragma once

void* GetKernelModuleExport(LPCWSTR module_name, LPCSTR routine_name) {
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, L"PsLoadedModuleList");
	PLIST_ENTRY module_list = (PLIST_ENTRY)MmGetSystemRoutineAddress(&name);

	if (!module_list)
		return NULL;

	for (PLIST_ENTRY link = module_list; link != module_list->Blink; link = link->Flink) {
		LDR_DATA_TABLE_ENTRY* entry = CONTAINING_RECORD(link, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		UNICODE_STRING name;
		RtlInitUnicodeString(&name, module_name);

		if (RtlEqualUnicodeString(&entry->BaseDllName, &name, TRUE)) {
			return (entry->DllBase) ? RtlFindExportedRoutineByName(entry->DllBase, routine_name) : NULL;
		}
	}
}

NTSTATUS HookFunction(uint32_t function_table_offset, uint32_t function_offset, void** previous_function, void* new_function) {
	W32GetSessionState GetW32SessionState = (W32GetSessionState)GetKernelModuleExport(L"win32k.sys", "W32GetSessionState");
	uint64_t function_table = *(uint64_t*)(*(uint64_t*)(GetW32SessionState() + 136) + function_table_offset);
	uint64_t* function_pointer = (uint64_t*)(function_table + function_offset);

	if (!function_pointer || !*function_pointer)
		return STATUS_UNSUCCESSFUL;

	*previous_function = *function_pointer;
	InterlockedExchangePointer(function_pointer, new_function);

	return STATUS_SUCCESS;
}

inline bool ProcessIsZombie(PEPROCESS process) {
	uint64_t object_table = *(uint64_t*)((uint64_t)process + OFFSET_eprocess_object_table);

	if (!object_table) {
		return true;
	}
	
	return false;
}

PEPROCESS FindProcessByName(const char* process_name) {
	PEPROCESS sys_process = PsInitialSystemProcess;
	PEPROCESS cur_entry = sys_process;

	do {
		if (strstr((char*)((uintptr_t)cur_entry + OFFSET_eprocess_image_name), process_name)) {
			if (!ProcessIsZombie(cur_entry)) {
				ObReferenceObject(cur_entry);
				return cur_entry;
			}
		}

		PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+OFFSET_eprocess_active_links);
		cur_entry = (PEPROCESS)((uintptr_t)list->Flink - OFFSET_eprocess_active_links);

	} while (cur_entry != sys_process);

	return NULL;
}

NTSTATUS InitalizeDriverContext(PDRIVER_CONTEXT* context) {
	PHYSICAL_ADDRESS size;
	size.QuadPart = MAXULONG64;
	
	PDRIVER_CONTEXT pool = (PDRIVER_CONTEXT)MmAllocateContiguousMemory(sizeof(DRIVER_CONTEXT), size);
	if (pool == NULL)
		return STATUS_UNSUCCESSFUL;

	pool->R = 255;
	pool->G = 255;
	pool->B = 255;
	pool->previous_drawfunc = NULL;
	pool->dwm = FindProcessByName("dwm.exe");
	*context = pool;

	return STATUS_SUCCESS;
}

// Drawing POC
typedef HBRUSH(*NtGdiCreateSolidBrush_T)(COLORREF cr, HBRUSH hbr);
typedef HDC(*NtUserGetDCEx_T)(HWND hwnd, HANDLE region, ULONG flags);
typedef HBRUSH(*NtGdiSelectBrush_T)(HDC hdc, HBRUSH hbrush);
typedef int (*NtUserReleaseDC_T)(HDC hdc);
typedef BOOL(*NtGdiPatBlt_T)(HDC hdcDest, INT x, INT y, INT cx, INT cy, DWORD dwRop);
typedef BOOL(*NtGdiDeleteObjectApp_T)(HANDLE hobj);

NtGdiSelectBrush_T NtGdiSelectBrush = NULL;
NtGdiPatBlt_T NtGdiPatBlt = NULL;
NtUserGetDCEx_T NtUserGetDCEx = NULL;
NtGdiCreateSolidBrush_T NtGdiCreateSolidBrush = NULL;
NtUserReleaseDC_T NtUserReleaseDC = NULL;
NtGdiDeleteObjectApp_T NtGdiDeleteObjectApp = NULL;


void FrameRect(HDC hdc, BOX box, HBRUSH hbr, int thickness) {
	HBRUSH oldbrush = NtGdiSelectBrush(hdc, hbr);

	NtGdiPatBlt(hdc, box.left, box.top, thickness, box.bottom - box.top, PATCOPY);
	NtGdiPatBlt(hdc, box.right - thickness, box.top, thickness, box.bottom - box.top, PATCOPY);
	NtGdiPatBlt(hdc, box.left, box.top, box.right - box.left, thickness, PATCOPY);
	NtGdiPatBlt(hdc, box.left, box.bottom - thickness, box.right - box.left, thickness, PATCOPY);

	if (oldbrush)
		NtGdiSelectBrush(hdc, oldbrush);
}

void DrawRect(BOX player_box, uint8_t r, uint8_t g, uint8_t b) {
	HDC hdc = NtUserGetDCEx(NULL, 0, 1);
	if (!hdc)
		return;

	HBRUSH brush = NtGdiCreateSolidBrush(RGB(r, g, b), NULL);
	if (!brush) {
		NtUserReleaseDC(hdc);
		return;
	}

	FrameRect(hdc, player_box, brush, 1);

	NtUserReleaseDC(hdc);
	NtGdiDeleteObjectApp(brush);
}

void InitalizeWin32() {
	NtGdiCreateSolidBrush = (NtGdiCreateSolidBrush_T)GetKernelModuleExport(L"win32kfull.sys", "NtGdiCreateSolidBrush");
	NtUserReleaseDC = (NtUserReleaseDC_T)GetKernelModuleExport(L"win32kbase.sys", "NtUserReleaseDC");
	NtUserGetDCEx = (NtUserGetDCEx_T)GetKernelModuleExport(L"win32kfull.sys", "NtUserGetDCEx");
	NtGdiSelectBrush = (NtGdiSelectBrush_T)GetKernelModuleExport(L"win32kbase.sys", "GreSelectBrush");
	NtGdiPatBlt = (NtGdiPatBlt_T)GetKernelModuleExport(L"win32kfull.sys", "NtGdiPatBlt");
	NtGdiDeleteObjectApp = (NtGdiDeleteObjectApp_T)GetKernelModuleExport(L"win32kbase.sys", "NtGdiDeleteObjectApp");
}