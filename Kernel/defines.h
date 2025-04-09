#pragma once

// Driver
typedef bool(__fastcall* NtUserEnableWindowGDIScaledDpiMessage)(uint64_t, int);
typedef uint64_t(__stdcall* W32GetSessionState)();

typedef struct _BOX {
	uint16_t left;
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
} BOX;

// PE
typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

extern NTKERNELAPI PVOID NTAPI RtlFindExportedRoutineByName(_In_ PVOID ImageBase, _In_ PCCH RoutineName);