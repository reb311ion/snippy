#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <wininet.h>

#pragma comment (lib, "wininet.lib")

bool DownloadFile(char* szUrl, char* szPath) {
	HINTERNET hOpen = NULL;
	HINTERNET hFile = NULL;
	HANDLE hOut = NULL;
	char* lpBuffer = NULL;
	DWORD dwBytesRead = 0;
	DWORD dwBytesWritten = 0;

	hOpen = InternetOpenA("MyAgent", NULL, NULL, NULL, NULL);
	if (!hOpen) return false;

	hFile = InternetOpenUrlA(hOpen, szUrl, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, NULL);
	if (!hFile) {
		InternetCloseHandle(hOpen);
		return false;
	}

	hOut = CreateFileA(szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
	if (hOut == INVALID_HANDLE_VALUE) {
		InternetCloseHandle(hFile);
		InternetCloseHandle(hOpen);
		return false;
	}

	do {
		lpBuffer = new char[2000];
		ZeroMemory(lpBuffer, 2000);
		InternetReadFile(hFile, (LPVOID)lpBuffer, 2000, &dwBytesRead);
		WriteFile(hOut, &lpBuffer[0], dwBytesRead, &dwBytesWritten, NULL);
		delete[] lpBuffer;
		lpBuffer = NULL;
	} while (dwBytesRead);

	CloseHandle(hOut);
	InternetCloseHandle(hFile);
	InternetCloseHandle(hOpen);
	return true;
}

int main(int argc, char *argv[]) {
	DownloadFile(argv[1], argv[2]);
	return 0;
}
