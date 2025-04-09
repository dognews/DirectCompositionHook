#include "imports.h"
#include "win32.h"

int main() {
	ConvertToWin32Thread();

	NtUserEnableWindowGDIScaledDpiMessage hooked_kernel_function = (NtUserEnableWindowGDIScaledDpiMessage)GetUsermodeModuleExport("win32u.dll", "NtUserEnableWindowGDIScaledDpiMessage");
	if (hooked_kernel_function == NULL) {
		puts("Failed to find function");
		int _ = getchar();
		return 1;
	}

	while (true) {
		USERMODE_REQUEST request;

		system("cls");
		printf("Input R: ");
		scanf_s("%d", &request.R);
		printf("Input G: ");
		scanf_s("%d", &request.G);
		printf("Input B: ");
		scanf_s("%d", &request.B);
		
		if (request.R >= 0 || request.R <= 255 || request.G >= 0 || request.G <= 255 || request.B >= 0 || request.B <= 255) {
			system("cls");
			printf("RGB Color\n\nR: %d, G: %d, B: %d\n", request.R, request.G, request.B);
			printf("Updated Driver Color\n\nPress Any Key To Continue...");
			hooked_kernel_function(&request, REQUEST_KEY);
		}

		getch();
	}

	return 0;
}