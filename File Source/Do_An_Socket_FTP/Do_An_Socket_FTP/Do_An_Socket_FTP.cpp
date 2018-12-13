// Do_An_Socket_FTP.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include"FTP_Client.h"

// The one and only application object
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
CWinApp theApp;

char* getCommand(char* arr) //lay ma lenh tu 1 doan lenh ma nguoi dung nhap vao: vd: get abc.txt -> lay lenh get
{
	char* result = new char[10];
	char* temp = new char[100];
	strcpy(temp, arr);
	return strtok(temp, " ");
}
char* getFileName(char* arr)
{
	char* temp = new char[100];
	strcpy(temp, arr);
	strtok(temp, " ");
	return strtok(NULL, "\0");
}

int FTP_Client::increase = 1;
void countTime(clock_t &start, bool& res, bool& done) {
	float time = 0;
	res = true;
	while (!done && time < 120) {
		time = (clock() - start) / CLOCKS_PER_SEC;
	}
	if (!done)
		res = false;
}
int main(int argc, char* argv[])
{
	int nRetCode = 0;

		HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			FTP_Client a;
			wchar_t ip[20];
			mbstowcs(ip, argv[1], 20); //Chuyển từ  char* -> wchar_t*
			if (!a.connect(ip))
			{
				//printf("Can't connect to server %s\n", argv[1]);
				exit(0);
			}
			//wchar_t ip[20] ;
			//wcsncpy(ip, L"192.168.88.128", 20);
			//a.connect(ip);
			//a.connect(L"192.168.43.3");//Đây là ip FTP server mà Client muốn kết nối đến
			bool login;
			login = a.login();

			char input[100];
			char* command;
			char* data;
			std::cin.ignore();
			do
			{
				printf("ftp> ");
				
				clock_t begin = clock();
				bool ok = true, done = false;
				thread t(&countTime, ref(begin), ref(ok), ref(done));
				
				gets_s(input);
				done = true;
				t.join();
				if (stricmp(input, "quit") == 0 || stricmp(input, "exit") == 0)
				{
					printf("221\n");
					exit(0);
				}
				if (!login)
				{
					printf("530 Please login with USER and PASS.\n");
				}
				else
				{
					if (strlen(input) > 0)
					{
						command = getCommand(input);
						//

						data = getFileName(input);
						if (!ok) 
						{
							a.setTIMEOUT();
						}
						if (a.processCmd(command, data))
							break;

					}
				}

			} while (1);

			a.ClientSocket.Close();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}