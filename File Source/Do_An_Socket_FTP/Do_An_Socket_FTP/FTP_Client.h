// Do_An_Socket_FTP.cpp : Defines the entry point for the console application.
//
#include "resource.h"
#include <iostream>
#include <string>
#include <time.h>
#include "Do_An_Socket_FTP.h"
#include <afxsock.h>
#include<cwchar>
#include<vector>
#include<conio.h>
#include<sys/stat.h>
#include <string.h>


#define S_ISDIR
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// The one and only application object
using namespace std;


#pragma once
#pragma once
class FTP_Client
{
private:
	CSocket dsocket;
	CSocket temp;
	char buf[BUFSIZ + 1];
	int tmpres, size, status;
	wchar_t ipServer[20];
	int port;
	char currentDirectoryPath[MAX_PATH];
	int portPASSIVE;
public:
	CSocket ClientSocket;
	int mode;//Defaut Active
	static int increase;
	bool timeout = TRUE;
public:
	void setTIMEOUT();
	void getFilesList(string filePath, string extension, vector<string> & returnFileName);
	bool processCmd(char *command, char *data);
	int sendPort(int show);

	bool connect(const wchar_t* ip);
	bool login();
	void ls();
	void dir();
	void dele(const char* fileName);
	void put(const char* fileName);
	void get(const char* fileName);
	void mdele(char* listFileName);
	void mput(char* listFileName);
	void mget(char* listFileName);
	void cd(char* newDirectoryPath);
	void lcd(char* newDirectoryPathClient);
	void mkdir(char* folderName);
	void rmdir(char* folderName);
	void pwd();
	bool dirExists(const char* newDirectoryPathClient);
	void passiveMode(int show);
	void quit();

	FTP_Client();
	~FTP_Client();
};

