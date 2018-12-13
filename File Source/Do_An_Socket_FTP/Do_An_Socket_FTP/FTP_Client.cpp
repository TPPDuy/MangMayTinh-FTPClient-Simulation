#include "stdafx.h"
#include "FTP_Client.h"
void FTP_Client::setTIMEOUT()
{
	timeout = FALSE;
}
void FTP_Client::getFilesList(string filePath, string extension, vector<string> & returnFileName)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	string  fullPath = filePath + extension;

	char fullPath1[MAX_PATH];
	int i;
	for (i = 0; i < fullPath.size(); i++)
		fullPath1[i] = fullPath[i];
	fullPath1[i] = '\0';
	TCHAR FullPath[MAX_PATH];
	mbstowcs(FullPath, fullPath1, MAX_PATH);

	char temp[260];
	hFind = FindFirstFile(FullPath, &fileInfo);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		wcstombs(temp, fileInfo.cFileName, MAX_PATH);
		returnFileName.push_back(string(temp));
		while (FindNextFile(hFind, &fileInfo) != 0) 
		{
			wcstombs(temp, fileInfo.cFileName, MAX_PATH);
			returnFileName.push_back(string(temp));
		}
	}
}
bool FTP_Client::processCmd(char *command, char *data)
{
	if (timeout) {
		if (stricmp(command, "dir") == 0)
			dir();
		else if (stricmp(command, "ls") == 0)
			ls();
		else if (stricmp(command, "put") == 0)
		{
			if (data == NULL)
			{
				printf("Local file ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("Local file put: remote file.\n");
				return FALSE;
			}
			put(data);
		}
		else if (stricmp(command, "get") == 0)
		{
			if (data == NULL)
			{
				printf("Remote file ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("Remote file get [ local-file ].\n");
				return FALSE;
			}
			get(data);
		}
		else if (stricmp(command, "mput") == 0)
		{
			if (data == NULL)
			{
				printf("Local files ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("Local files put.\n");
				return FALSE;
			}
			mput(data);
		}
		else if (stricmp(command, "mget") == 0)
		{
			if (data == NULL)
			{
				printf("Remote files ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("Remote files mget.\n");
				return FALSE;
			}
			mget(data);
		}
		else if (stricmp(command, "cd") == 0)
		{
			if (data == NULL)
			{
				printf("Remote directory ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("cd remote directory.\n");
				return FALSE;
			}
			cd(data);
		}
		else if (stricmp(command, "lcd") == 0)
		{
			if (data == NULL)
			{
				printf("Remote directory ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("lcd remote directory.\n");
				return FALSE;
			}
			lcd(data);
		}
		else if (stricmp(command, "delete") == 0)
		{
			if (data == NULL)
			{
				printf("Remote file ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("delete remote file.\n");
				return FALSE;
			}
			dele(data);
		}
		else if (stricmp(command, "mdelete") == 0)
		{
			if (data == NULL)
			{
				printf("Remote files ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("Remote files mdelete.\n");
				return FALSE;
			}
			mdele(data);
		}
		else if (stricmp(command, "rmdir") == 0)
		{
			if (data == NULL)
			{
				printf("Directory name ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("rmdir directory-name\n");
				return FALSE;
			}
			rmdir(data);
		}
		else if (stricmp(command, "mkdir") == 0)
		{
			if (data == NULL)
			{
				printf("Directory name ");
				char temp[300];
				gets_s(temp);
				if (strcmp(temp, "") != 0)
					data = strdup(temp);
				else data = NULL;
			}
			if (data == NULL)
			{
				printf("mkdir directory-name\n");
				return FALSE;
			}
			mkdir(data);
		}
		else if (stricmp(command, "pwd") == 0)
			pwd();
		else if (stricmp(command, "pasv") == 0)
		{
			if (mode == 1)
				passiveMode(1);
			else
			{
				printf("Entering Active Mode\n");
				mode = 1;
			}
		}
		else if (stricmp(command, "quit") == 0 || stricmp(command, "exit") == 0)
		{
			quit();
			return TRUE;
		}
		else
			printf("Invalid Command\n");
	}
	else {
		if (stricmp(command, "quit") == 0 || stricmp(command, "exit") == 0)
			cout << "\n";
		else
			cout << "Not connected.\n";
	}
	return FALSE;
}
int FTP_Client::sendPort(int show)
{
	UINT p;
	CString s;
	ClientSocket.GetSockName(s, p);//  s = 192.168.43.2: ipClient,
	int a, b, c, d;//từng phần ip của Client:: a.b.c.d
	int t = 0, count = 0;
	//Xử lý: Tách từng phần trong chuỗi ipClient thành các đối số a,b,c,d để truyền vào lệnh gửi port của dsocket vừa tạo lên server
	for (int i = 0; i < wcslen(s); i++)
	{
		if (isdigit(s[i]))// s[i] is decimal digit

		{
			t = t * 10 + (s[i] - 48);
			if (!isdigit(s[i + 1])) //s[i + 1] is NOT decimal digit: dấu . || /0
			{
				count++;
				switch (count)
				{
				case 1: a = t; break;
				case 2: b = t; break;
				case 3: c = t; break;
				case 4: d = t; break;
				}
				t = 0;
			}
		}
	}
	//
	dsocket.Create();
	dsocket.GetSockName(s, p);
	dsocket.Listen(1);
	//
	//Truyền lệnh port cho server
	memset(buf, 0, BUFSIZ);
	sprintf(buf, "PORT %d,%d,%d,%d,%d,%d\r\n", a, b, c, d, p / 256, p % 256);
	ClientSocket.Send(buf, BUFSIZ, 0);// Gửi lệnh port lên server

	memset(buf, 0, BUFSIZ);
	ClientSocket.Receive(buf, BUFSIZ, 0);// Nhận phản hồi từ server:: 200 PORT command successful
	if (show == 1)
	{
		printf("%s", buf);//in phản hồi từ server::200 PORT command successful
	}
	increase++;	
	return p;
}
/*=================================CONNECT VÀO FTP SERVER======================================================*/
bool FTP_Client::connect(const wchar_t* ip)// wchar_t *ip ?? => Vì:
{
	//Tạo socket connect tới Server
	if (AfxSocketInit() == FALSE)
	{
		cout << "Khong the khoi tao Socket Library";
		return FALSE;
	}
	// Tao socket dau tien
	ClientSocket.Create();

	// Ket noi den Server
	wcsncpy(ipServer, ip, 20);//Copy cái ip truyền vào qua ipServer cùng kiểu wchar_t

	if (ClientSocket.Connect(ipServer, 21) != 0)//Kết nối đến port 21 mà không phải port 20 là vì: 
												//Port 21: control port. port này dùng để trao đổi lệnh giữa client & server
	{
		cout << "Ket noi toi Server thanh cong !!!" << endl << endl;
	}
	else
	{
		cout << "Ket noi khong thanh cong !!!" << endl;
		return FALSE;
	}

	char * str;
	int codeftp;
	printf("Connection established, waiting for welcome message...\n");
	//How to know the end of welcome message: http://stackoverflow.com/questions/13082538/how-to-know-the-end-of-ftp-welcome-message
	memset(buf, 0, sizeof buf); //biến nguyên dãy buf thành 0
	while ((tmpres = ClientSocket.Receive(buf, BUFSIZ, 0)) > 0)
	{
		sscanf(buf, "%d", &codeftp);
		printf("%s", buf);//"220 Microsoft FTP Service\r\n421 Timeout (120 seconds): closing control connection.\r\n421 Terminating connection.\r\n"
		if (codeftp != 220) //120, 240, 421: something wrong
			exit(1);

		str = strstr(buf, "220"); //Why ??? strstr tìm chuỗi "220" trong chuỗi buf, nếu không có trả về NULL, nếu có trả về con trỏ ở vị trí đầu tiên của chuỗi
		if (str != NULL)
		{
			break;
		}
		memset(buf, 0, tmpres); //biến tmpres kí tự đầu tiên của buf thành 0
	}
	return TRUE;
}
/*=============================LOGIN VÀO FTP SERVER==============================================================*/
bool FTP_Client::login()
{
	int codeftp;
	//Send Username
	char info[50];
	printf("Name (%s): ", "192.168.43.1");
	memset(buf, 0, sizeof buf); //set buf thành 0 hết
	scanf("%s", info); //nhận user nhập từ người dùng

	sprintf(buf, "USER %s\r\n", info); //nhét info vào buf
	tmpres = ClientSocket.Send(buf, strlen(buf), 0); //gửi thông điệp user lên server

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);//nhận phản hồi từ server

	sscanf(buf, "%d", &codeftp);//nhét buf vào codeftp = 331

	printf("%s", buf); //in mã phản hồi từ user::"331 Password required for ftpuser.\r\n"

	if (codeftp != 331)// 331 User name okay, need password. 
		return FALSE;

	//Send Password
	memset(info, 0, sizeof info);
	printf("Password: ");
	memset(buf, 0, sizeof buf);
	string pass = "";
	char a;
	//ẩn password
	int i = -1;
	do
	{
		a = _getch();
		if (a > 32) {
			pass += a;
			/*if (a != '\n'&&a != '\r')
				cout << a;*/
		}
	} while (a != '\r');
	
	sprintf(buf,"PASS %s\r\n", pass.c_str()); //đổ info vào buf
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);//gửi pass lên server

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);//nhận phản hồi từ server

	sscanf(buf, "%d", &codeftp); //nhét buf vào codeftp = 230
	printf("\n%s", buf);//in phản hồi từ server:: 230 User logged in, proceed. 
	if (codeftp != 230) //đăng nhập không thành công
		return FALSE;
	return TRUE;
}
/*===========LIỆT KÊ DANH SÁCH CÁC THƯ MỤC, TẬP TIN TRÊN FTP SERVER===============================================================================================================*/
void FTP_Client::ls()
{
	if (mode == 1) //ACTIVE
	{
		//Client Tạo thêm dsocket và lắng nghe kết nối
		//Gửi thông tin dsocket vừa được tạo qua server thông qua csocket <ClientSocket>
		int p = sendPort(1);

		//Gửi lệnh LIST
		memset(buf, 0, sizeof buf);
		sprintf(buf, "NLST\r\n");
		ClientSocket.Send(buf, BUFSIZ, 0);
		//Nhận phản hồi từ server
		memset(buf, 0, sizeof buf);
		ClientSocket.Receive(buf, BUFSIZ, 0);//Nhận phản hồi từ server:: 150 opening ACSCII mode data connection..... 

		vector<char*> cmd;
		int codeftp = 0;
		;
		char *t = strtok(buf, "\n");
		while (t != NULL)
		{
			cmd.push_back(t);
			t = strtok(NULL, "\n");
		}
		printf("%s\n", cmd[0]);//In phản hồi từ server

		CSocket Connector;
		//Khoi tao 1 socket de duy tri viec ket noi va trao doi du lieu
		if (dsocket.Accept(Connector))
		{
			//Nhận list file tại dsocket
			memset(buf, 0, sizeof buf);
			Connector.Receive(buf, BUFSIZ, 0);
			printf("%s", buf);//in danh sách các file trên server
		}
		Connector.Close();
		dsocket.Close();
		//Nhận phản hồi từ server
		if (cmd.size() == 1) {
			memset(buf, 0, sizeof buf);
			ClientSocket.Receive(buf, BUFSIZ, 0);// 226 trasfer complete
			printf("%s", buf);//In phản hồi từ server:: 226 trasfer complete
		}
		else printf("%s\n", cmd[1]);
	}
	else //PASSIVE
	{
		passiveMode(1);
		int portServer = portPASSIVE;//Đã được tính toán khi Client gửi PASV lên Server
									 //Tao dsocket ket noi vao dia chi server cung cap
									 //CSocket dsocket;
		//Tạo dsocket kết nối vào địa chỉ tính được:portServer
		dsocket.Create(); //SOCK_STREAM: giao thức TCP
											  // Ket noi den Server qua địa chỉ server cung cấp
		dsocket.Connect(ipServer, portServer);
		//Dùng csocket Gửi lệnh LIST lên server
		memset(buf, 0, sizeof buf);
		sprintf(buf, "NLST\r\n");
		ClientSocket.Send(buf, BUFSIZ, 0);
		//Nhận phản hồi từ server
		memset(buf, 0, sizeof buf);
		ClientSocket.Receive(buf, 54, 0);

		vector<char*> cmd;
		int codeftp = 0;
		;
		char *t = strtok(buf, "\n");
		while (t != NULL)
		{
			cmd.push_back(t);
			t = strtok(NULL, "\n");
		}
		printf("%s\n", cmd[0]);//in phản hồi từ server::125 Data connection already open; Transfer starting.
						  ////Nhận danh sách thư mục, têp tin trên server thông qua dsocket
		memset(buf, 0, sizeof buf);
		dsocket.Receive(buf, BUFSIZ, 0);//Nhận danh sách thư mục, têp tin trên server
		printf("%s", buf);//in phản hồi từ server

		dsocket.Close();

		if (cmd.size() == 1) {
			memset(buf, 0, sizeof buf);
			ClientSocket.Receive(buf, BUFSIZ, 0);// 226 trasfer complete
			printf("%s", buf);//In phản hồi từ server:: 226 trasfer complete
		}
		else printf("%s\n", cmd[1]);
	}
}
void FTP_Client::dir()
{
	if (mode == 1) //ACTIVE
	{
		//Client Tạo thêm dsocket và lắng nghe kết nối
		//Gửi thông tin dsocket vừa được tạo qua server thông qua csocket <ClientSocket>
		sendPort(1);

		//Gửi lệnh LIST
		memset(buf, 0, sizeof buf);
		sprintf(buf, "LIST\r\n");
		ClientSocket.Send(buf, BUFSIZ, 0);
		//Nhận phản hồi từ server
		memset(buf, 0, sizeof buf);
		ClientSocket.Receive(buf, BUFSIZ, 0);//Nhận phản hồi từ server:: 150 opening ACSCII mode data connection..... 

		vector<char*> cmd;
		int codeftp = 0;
		char *t = strtok(buf, "\n");
		while (t != NULL)
		{
			cmd.push_back(t);
			t = strtok(NULL, "\n");
		}
		printf("%s", cmd[0]);//In phản hồi từ server

		CSocket Connector;
		//Khoi tao 1 socket de duy tri viec ket noi va trao doi du lieu
		if (dsocket.Accept(Connector))
		{
			//Nhận list file tại dsocket
			memset(buf, 0, sizeof buf);
			Connector.Receive(buf, BUFSIZ, 0);
			printf("%s", buf);//in danh sách các file trên server
		}
		Connector.Close();
		dsocket.Close();
		//Nhận phản hồi từ server
		if (cmd.size() == 1) {
			memset(buf, 0, sizeof buf);
			ClientSocket.Receive(buf, BUFSIZ, 0);// 226 trasfer complete
			printf("%s", buf);//In phản hồi từ server:: 226 trasfer complete
		}
		else printf("%s", cmd[1]);
	}
	else //PASSIVE
	{
		passiveMode(1);
		int portServer = portPASSIVE;//Đã được tính toán khi Client gửi PASV lên Server
									 //Tao dsocket ket noi vao dia chi server cung cap
		//CSocket dsocket;
		//Tạo dsocket kết nối vào địa chỉ tính được:portServer
		dsocket.Create(); //SOCK_STREAM: giao thức TCP
											  // Ket noi den Server qua địa chỉ server cung cấp
		dsocket.Connect(ipServer, portServer);
		//Dùng csocket Gửi lệnh LIST lên server
		memset(buf, 0, sizeof buf);
		sprintf(buf, "LIST\r\n");
		ClientSocket.Send(buf, BUFSIZ, 0);
		//Nhận phản hồi từ server
		memset(buf, 0, sizeof buf);
		ClientSocket.Receive(buf, 54, 0);

		vector<char*> cmd;
		int codeftp = 0;
		;
		char *t = strtok(buf, "\n");
		while (t != NULL)
		{
			cmd.push_back(t);
			t = strtok(NULL, "\n");
		}
		printf("%s", cmd[0]);//in phản hồi từ server::125 Data connection already open; Transfer starting.\r\n226 Transfer complete.\r\n"
						  ////Nhận danh sách thư mục, têp tin trên server thông qua dsocket

		memset(buf, 0, sizeof buf);
		dsocket.Receive(buf, BUFSIZ, 0);//Nhận danh sách thư mục, têp tin trên server
		printf("%s", buf);//in phản hồi từ server

		dsocket.Close();
		if (cmd.size() == 1) {
			memset(buf, 0, sizeof buf);
			ClientSocket.Receive(buf, BUFSIZ, 0);// 226 trasfer complete
			printf("%s", buf);//In phản hồi từ server:: 226 trasfer complete
		}
		else printf("%s", cmd[1]);
	}
}
/*==============XÓA 1 FILE TRÊN FTP SERVER===============================================================================================================*/
void FTP_Client::dele(const char* fileName)
{
	char* FILENAME = strdup(fileName);
	FILENAME = strtok(FILENAME, " ");
	sprintf(buf, "DELE %s\r\n", FILENAME); //đổ info vào buf
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);//gửi lên server

	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);//nhận phản hồi từ server:
												  //Nếu fileName tồn tại-> Xóa được:"250 DELE command successful.\r\n"
												  //Nếu fileName không tồn tại -> Không thấy file để xóa: 550 fileName: The system cannot find the file specified.
	printf("%s", buf);//in phản hồi từ server
}
/*==============UPLOAD 1 FILE ĐẾN SERVER===============================================================================================================*/
void FTP_Client::put(const char* fileName)
{
	vector<char*> T;
	char *FILENAME = strdup(fileName);
	char *temp = strtok(FILENAME, " ");

	while (temp != NULL)
	{
		T.push_back(temp);
		temp = strtok(NULL, "\n");
	}
	FILENAME = strdup(T[0]);
	FILE *file;
	memset(buf, 0, BUFSIZ);
	sprintf(buf, "%s%s", currentDirectoryPath, FILENAME);
	file = fopen(buf, "rb");
	if (file == NULL)
	{
		cout << fileName << ": File not found\n";
	}
	else
	{
		if (mode == 1) //ACTIVE
		{
			sendPort(1);
			//STOR <filename>
			memset(buf, 0, BUFSIZ);
			sprintf(buf, "STOR %s\r\n", FILENAME);
			ClientSocket.Send(buf, BUFSIZ, 0);
			memset(buf, 0, BUFSIZ);
			ClientSocket.Receive(buf, BUFSIZ, 0);//"150 Opening ASCII mode data connection for me.txt.\r\n"
			printf("%s", buf);//in ph?n h?i t? server
							  //Chap nhan ket noi tu server tai dsocket
			CSocket Connector;
			//Khoi tao 1 socket de duy tri viec ket noi va trao doi du lieu
			if (dsocket.Accept(Connector))
			{
				memset(buf, 0, BUFSIZ);
				int size = 0;
				do
				{
					size = fread(buf, 1, BUFSIZ, file);// Trả về số lượng data được đọc (Không phải số byte) nếu thành công
													   //  Nếu xảy ra lỗi || gặp cuối file => fread() trả về số nguyên x < số lượng data được đọc
					Connector.Send(buf, size, 0);//gui file
					memset(buf, 0, BUFSIZ);
				} while (size != 0);

				fclose(file);
				Connector.Close();
				dsocket.Close();
				memset(buf, 0, BUFSIZ);
				ClientSocket.Receive(buf, BUFSIZ, 0);//226: Transfer complete
				printf("%s", buf);
			}
		}
		else
		{

			//(1) Client thông qua csocket cho server biết cần dùng passive mode 
			passiveMode(0);
			//(2) nhận thông tin về IP và port từ server thông qua csocket 
			//Nhận thông tin port(m) = portPassive đã được tính toán trong passiveMode(1);
			//(3) Tạo dsocket nết nối vào server : Kết nối vào port(m), ipServer
			//CSocket dsocket;
			//=>Client tạo kết nối truyền dữ liệu từ cổng “n+1” của nó đến cổng truyền dữ liệu “m” của Server.

			dsocket.Create(); //SOCK_STREAM: giao th?c TCP
												  //Kết nối vào port(m), ipServer
			dsocket.Connect(ipServer, portPASSIVE);
			//(4) Gửi lệnh STOR <filename> thông qua csocket
			memset(buf, 0, BUFSIZ);
			sprintf(buf, "STOR %s\r\n", FILENAME);
			ClientSocket.Send(buf, BUFSIZ, 0);
			//(5) Truyen file qua server tại dsocket
			memset(buf, 0, BUFSIZ);
			int size = 0;
			do
			{
				size = fread(buf, 1, BUFSIZ, file);
				dsocket.Send(buf, size, 0);//gui file
				memset(buf, 0, BUFSIZ);
			} while (size != 0);
			fclose(file);

			memset(buf, 0, BUFSIZ);
			ClientSocket.Receive(buf, BUFSIZ, 0);//Nhận phản hồi từ server
												 //"125 Data connection already open; Transfer starting.\r\n"
			printf("%s", buf);//In phản hồi từ server
			dsocket.Close();

			memset(buf, 0, BUFSIZ);
			ClientSocket.Receive(buf, BUFSIZ, 0);//Nhận phản hồi từ server
												 // 226 transfer complete
			printf("%s", buf);//In phản hồi từ server
		}
	}
	//mode = 1;
}
/*==============DOWNLOAD 1 FILE TỪ SERVER===============================================================================================================*/
void FTP_Client::get(const char* fileName)
{
	vector<char*> T;
	char *FILENAME = strdup(fileName);
	char *temp = strtok(FILENAME, " ");//buf = "150 Opening ASCII mode data connection for me.txt(515 bytes).\r
								   //Erorr: buf = "550 thanh.txt: The system cannot find the file specified. \r\n"
	while (temp != NULL)
	{
		T.push_back(temp);//Cắt 2 chuỗi 150, 226 rồi nhét chúng vào vector<char*>;  Erorr: cho 550 vô
		temp = strtok(NULL, "\n");
	}
	FILENAME = strdup(T[0]);
	if (mode == 1) //ở mode active
	{
		sendPort(1); //gửi port cho server
		//Gửi lệnh RETR <filename>
		memset(buf, 0, BUFSIZ);
		sprintf(buf, "RETR %s\r\n", FILENAME);
		ClientSocket.Send(buf, BUFSIZ, 0);
		//Xử lý cắt cHuỗi 150 & 226 từ phản hồi của server
		vector<char*> cmd;
		int codeftp = 0;
		memset(buf, 0, BUFSIZ);
		ClientSocket.Receive(buf, BUFSIZ, 0);//"150 Opening ASCII mode data connection for me.txt(515 bytes).\r\n226 Transfer complete.\r\n"
											 //Erorr:"550 thanh.txt: The system cannot find the file specified. \r\n"
		temp = strtok(buf, "\n");//buf = "150 Opening ASCII mode data connection for me.txt(515 bytes).\r
									   //Erorr: buf = "550 thanh.txt: The system cannot find the file specified. \r\n"
		while (temp != NULL)
		{
			cmd.push_back(temp);//Cắt 2 chuỗi 150, 226 rồi nhét chúng vào vector<char*>;  Erorr: cho 550 vô
			temp = strtok(NULL, "\n");
		}
		printf("%s\n", cmd[0]);//In ra 150 || Erorr: in ra 550
		sscanf(cmd[0], "%d", &codeftp);
		if (codeftp == 550)
		{
			dsocket.Close();
			return;
		}
		if (cmd.size() == 2)
		{
			printf("%s\n", cmd[1]);//In ra 226
		}
		CSocket Connector;
		//Khoi tao 1 socket de duy tri viec ket noi va trao doi du lieu
		//Khi 550 thì không Accept!
		if (dsocket.Accept(Connector))
		{
			FILE *file;
			memset(buf, 0, BUFSIZ);
			sprintf(buf, "%s%s", currentDirectoryPath, FILENAME);
			file = fopen(buf, "wb");//Mở FILENAME trong currentDirectoryPath ở Client để tiến hành ghi nhị phân
			if (file == NULL)
			{
				cout << FILENAME << ": Can not create file\n";
			}
			else
			{
				memset(buf, 0, BUFSIZ);
				int size = 0;
				do
				{
					size = Connector.Receive(buf, BUFSIZ, 0);//Nhận dữ liệu trong file đặt tại Server vào buf
					fwrite(buf, 1, size, file);//Ghi từ buf vô file ở Client
											   // Trả về số lượng data được ghi (Không phải số byte) nếu thành công
											   // Nếu xảy ra lỗi || gặp cuối file => fread() trả về số nguyên x < số lượng data được ghi
					memset(buf, 0, BUFSIZ);
				} while (size != 0);

				fclose(file);
				Connector.Close();
				dsocket.Close();

				if (cmd.size() == 1)//Trường hợp không bị dính chuỗi 150 và 226; không phải trường hợp 550
				{
					memset(buf, 0, BUFSIZ);
					ClientSocket.Receive(buf, BUFSIZ, 0);//nhận phản hồi từ server
					printf("%s", buf);//in phản hồi từ server
				}

			}
		}
	}
	else
	{
		passiveMode(0);
		//Tao dsocket ket noi vao dia chi server cung cap
		//CSocket dsocket;
		dsocket.Create(); //SOCK_STREAM: giao thức TCP

											  //Ket noi den Server qua địa chỉ server cung cấp
		dsocket.Connect(ipServer, portPASSIVE);
		//Gui lenh RETR <filename>
		memset(buf, 0, BUFSIZ);
		sprintf(buf, "RETR %s\r\n", FILENAME);
		ClientSocket.Send(buf, BUFSIZ, 0);
		//
		vector<char*> cmd;
		int codeftp = 0;
		memset(buf, 0, BUFSIZ);
		ClientSocket.Receive(buf, BUFSIZ, 0);
		char *temp = strtok(buf, "\n");
		while (temp != NULL)
		{
			cmd.push_back(temp);
			temp = strtok(NULL, "\n");
		}
		printf("%s\n", cmd[0]);//In ra 150 || Erorr: in ra 550
		sscanf(cmd[0], "%d", &codeftp);
		if (codeftp == 550) {
			dsocket.Close();
			return;
		}
		if (cmd.size() == 2)
		{
			printf("%s\n", cmd[1]);//In ra 226
		}
		//Truyen file qua dsocket
		FILE *file;
		memset(buf, 0, BUFSIZ);
		sprintf(buf, "%s%s", currentDirectoryPath, FILENAME);
		file = fopen(buf, "wb");
		if (file == NULL)
		{
			cout << FILENAME << ": Can not create file\n";
		}
		else
		{
			memset(buf, 0, BUFSIZ);
			int size = 0;
			int count = 0;
			do
			{
				count++;
				size = dsocket.Receive(buf, BUFSIZ, 0);
				fwrite(buf, 1, size, file);
				memset(buf, 0, BUFSIZ);
			} while (size != 0);
			fclose(file);
			dsocket.Close();

			if (cmd.size() == 1)
			{
				memset(buf, 0, BUFSIZ);
				ClientSocket.Receive(buf, BUFSIZ, 0);//nhận phản hồi từ server
				printf("%s", buf);//in phản hồi từ server
			}
		}
	}
}
/*===============================XÓA NHIỀU FILE====================================*/
void FTP_Client::mdele(char* listFileName)
{
	memset(buf, 0, BUFSIZ);
	//TYPE A for NLST
	sprintf(buf, "TYPE A\r\n");
	ClientSocket.Send(buf, BUFSIZ, 0);//gửi lên server
	memset(buf, 0, BUFSIZ);
	ClientSocket.Receive(buf, BUFSIZ, 0);
	memset(buf, 0, BUFSIZ);
	//
	string temp = "";//giữ tên từng file trong list file
	string listfilenametoDELE = "";//giữ tên những file hợp lệ trong danh sách file mà người dùng nhập vào
	for (int i = 0; i <= strlen(listFileName); i++)
	{
		if (listFileName[i] != ' '&&listFileName[i] != NULL) //lấy tên của 1 file trong danh sách người dùng nhập vào
			temp += listFileName[i];
		else
		{
			const char *tempInfo = temp.c_str();
			//xét chế độ là active hay passive
			if (mode == 1)
			{
				//gửi lên server
				sendPort(0);
			}
			else
			{
				passiveMode(0);
				dsocket.Create();//SOCK_STREAM: giao thức TCP
				dsocket.Connect(ipServer, portPASSIVE);
			}
			//gửi lệnh NLST để server xác nhận file

			memset(buf, 0, BUFSIZ);
			sprintf(buf, "NLST %s\r\n", tempInfo);
			ClientSocket.Send(buf, strlen(buf), 0);
			//do nhận phản hồi về đôi khi bị dính 150 và 226 nên phải tách ra và kt
			vector<char*> cmd;
			int codeftp = 0;

			memset(buf, 0, BUFSIZ);
			ClientSocket.Receive(buf, BUFSIZ, 0);
			char *t = strtok(buf, "\n");
			while (t != NULL)
			{
				cmd.push_back(t);
				t = strtok(NULL, "\n");
			}
			if (cmd.size() == 2)
			{
				sscanf(cmd[1], "%d", &codeftp);
				if (codeftp == 550)
				{
					printf("%s\n", cmd[1]);
				}
				else 
				{//Transfer complete
					if (mode == 1)
					{
						CSocket Connector;
						//Khoi tao 1 socket de duy tri viec ket noi va trao doi du lieu
						if (dsocket.Accept(Connector))
						{
							//Nh?n list file t?i dsocket
							memset(buf, 0, sizeof buf);
							Connector.Receive(buf, BUFSIZ, 0);
						}
						Connector.Close();
						dsocket.Close();
					}
					else
					{
						memset(buf, 0, sizeof buf);
						dsocket.Receive(buf, BUFSIZ, 0);//Nh?n danh sách thu m?c, têp tin trên server
						printf("%s", buf);//in ph?n h?i t? server

						dsocket.Close();
					}
					t = strtok(buf, "\r\n");
					while (t != NULL)
					{
						temp = t;
						listfilenametoDELE += temp + " ";
						t = strtok(NULL, "\r\n");
					}
					dsocket.Close();
				}
			}
			else 
			{
				memset(buf, 0, BUFSIZ);
				ClientSocket.Receive(buf, BUFSIZ, 0);//nhận phản hồi từ server

				sscanf(buf, "%d", &codeftp);
				if (codeftp == 550) {
					printf("%s", buf);
				}
				else//Transfer complete
				{
					if (mode == 1)
					{
						CSocket Connector;
						//Khoi tao 1 socket de duy tri viec ket noi va trao doi du lieu
						if (dsocket.Accept(Connector))
						{
							//Nh?n list file t?i dsocket
							memset(buf, 0, sizeof buf);
							Connector.Receive(buf, BUFSIZ, 0);
						}
						Connector.Close();
						dsocket.Close();
					}
					else
					{
						memset(buf, 0, sizeof buf);
						dsocket.Receive(buf, BUFSIZ, 0);//Nh?n danh sách thu m?c, têp tin trên server
						printf("%s", buf);//in ph?n h?i t? server

						dsocket.Close();
					}
					t = strtok(buf, "\r\n");
					while (t != NULL)
					{
						temp = t;
						listfilenametoDELE += temp + " ";
						t = strtok(NULL, "\r\n");
					}
					dsocket.Close();
				}
			}
			temp = "";
		}
	}
	//
	if (listfilenametoDELE == "") return; //nếu không có một file nào hợp lệ

										  //nếu có file hợp lệ
	listfilenametoDELE.pop_back();
	const char *listfilenameFINAL = listfilenametoDELE.c_str();
	//TYPE A for RETR
	sprintf(buf, "TYPE A\r\n");
	ClientSocket.Send(buf, strlen(buf), 0);//gửi lên server
	memset(buf, 0, BUFSIZ);
	ClientSocket.Receive(buf, BUFSIZ, 0);//nhận phản hồi từ server
	printf("%s", buf);//in phản hồi từ server
	for (int i = 0; i <= strlen(listfilenameFINAL); i++)
	{
		if (listfilenameFINAL[i] != ' '&&listfilenameFINAL[i] != NULL)
			temp += listfilenameFINAL[i];
		else
		{
			const char *tempInfo = temp.c_str();
			printf("mdelete %s? ", tempInfo);
			char skipfile = getch();
			cout << "\n";
			if (skipfile == '\r') {
				
				dele(tempInfo);
				;
			}
			temp = "";
		}
	}
}

void FTP_Client::mget(char* listFileName)
{
	memset(buf, 0, BUFSIZ);
	//TYPE A for NLST
	sprintf(buf, "TYPE A\r\n");
	ClientSocket.Send(buf, BUFSIZ, 0);//gửi lên server
	memset(buf, 0, BUFSIZ);
	ClientSocket.Receive(buf, BUFSIZ, 0);
	memset(buf, 0, BUFSIZ);
	//////////
	string temp = "";//chứa tên từng file trong danh sách file truyền vào
	string listfilenametoGET = "";//chứa các file hợp lệ (có trên server) mà ng dùng nhập vào
	for (int i = 0; i <= strlen(listFileName); i++)
	{
		if (listFileName[i] != ' '&&listFileName[i] != NULL)
			temp += listFileName[i];
		else 
		{
			const char *tempInfo = temp.c_str(); //chuyển từ string sang char*
			if (mode == 1) {
				//Gửi lệnh lên server
				sendPort(0);
			}
			else
			{
				passiveMode(0);
				dsocket.Create();//SOCK_STREAM: giao thức TCP
				dsocket.Connect(ipServer, portPASSIVE);
			}
			//
			
			memset(buf, 0, BUFSIZ);
			sprintf(buf, "NLST %s\r\n", tempInfo);
			ClientSocket.Send(buf, strlen(buf), 0);
			//nhận phản hồi từ server, do có lúc bị dính 150 và 226 nên phải tách ra nếu nó bị dính lệnh
			vector<char*> cmd;//mảng để giữ các phản hồi
			int codeftp = 0;
			memset(buf, 0, BUFSIZ);
			ClientSocket.Receive(buf, BUFSIZ, 0);
			//cắt phản hồi ra thành từng cái riêng biệt
			char *t = strtok(buf, "\n");
			while (t != NULL)
			{
				cmd.push_back(t);
				t = strtok(NULL, "\n");
			}
			if (cmd.size() == 2) //xét nếu là có dính
			{
				sscanf(cmd[1], "%d", &codeftp);
				if (codeftp == 550) {
					printf("%s\n", cmd[1]);
				}
				else
				{//Transfer complete
					if (mode == 1) 
					{
						CSocket Connector;
						//Khoi tao 1 socket de duy tri viec ket noi va trao doi du lieu
						if (dsocket.Accept(Connector))
						{
							//Nh?n list file t?i dsocket
							memset(buf, 0, sizeof buf);
							Connector.Receive(buf, BUFSIZ, 0);
						}
						Connector.Close();
						dsocket.Close();
					}
					else 
					{
						memset(buf, 0, sizeof buf);
						dsocket.Receive(buf, BUFSIZ, 0);//Nh?n danh sách thu m?c, têp tin trên server
						printf("%s", buf);//in ph?n h?i t? server

						dsocket.Close();
					}
					t = strtok(buf, "\r\n");
					while (t != NULL)
					{
						temp = t;
						listfilenametoGET += temp + " ";
						t = strtok(NULL, "\r\n");
					}
				}
				dsocket.Close();
			}
			else//nếu không bị dính
			{
				CSocket Connector;
				//Khoi tao 1 socket de duy tri viec ket noi va trao doi du lieu
				if (dsocket.Accept(Connector))
				{
					//Nh?n list file t?i dsocket
					memset(buf, 0, sizeof buf);
					Connector.Receive(buf, BUFSIZ, 0);
				
				Connector.Close();
				dsocket.Close();
				}
				else 
				{
					memset(buf, 0, sizeof buf);
					dsocket.Receive(buf, BUFSIZ, 0);//Nh?n danh sách thu m?c, têp tin trên server
					printf("%s", buf);//in ph?n h?i t? server

					dsocket.Close();
				}

				t = strtok(buf, "\r\n");
				while (t != NULL)
				{
					temp = t;
					listfilenametoGET += temp + " ";
					t = strtok(NULL, "\r\n");
				}
				dsocket.Close();
				temp = "";
			}
		}
	}
	sprintf(buf, "TYPE A\r\n");
	ClientSocket.Send(buf, strlen(buf), 0);//g?i lên server
	memset(buf, 0, BUFSIZ);
	ClientSocket.Receive(buf, BUFSIZ, 0);//nh?n ph?n h?i t? server
	printf("%s", buf);//in ph?n h?i t? server
	//đã xác nhận đuợc các file hợp lệ, bây giờ tải file từ server về
	if (listfilenametoGET == "") return;//nếu không có 1 file nào hợp lệ

										//nếu có file hợp lệ
	listfilenametoGET.pop_back(); //bỏ dấu " " cuối cùng
	const char *listfilenameFINAL = listfilenametoGET.c_str(); //chuyển từ string sang char*
															   //TYPE A for RETR
	sprintf(buf, "TYPE A\r\n");
	ClientSocket.Send(buf, strlen(buf), 0);//gửi lên server
	memset(buf, 0, BUFSIZ);
	ClientSocket.Receive(buf, BUFSIZ, 0);//nhận phản hồi từ server
	printf("%s", buf);//in phản hồi từ server

	for (int i = 0; i <= strlen(listfilenameFINAL); i++)
	{
		if (listfilenameFINAL[i] != ' '&&listfilenameFINAL[i] != NULL) //lấy từng file ra
			temp += listfilenameFINAL[i];
		else //đã lấy xong 1 file
		{
			const char *tempInfo = temp.c_str();
			printf("mget %s? ", tempInfo); //dừng để cho người dùng xác nhận lấy file về
			char skipfile = getch();
			cout << "\n";
			if (skipfile == '\r')
			{
				
				get(tempInfo);
				;
			}
			temp = "";
		}
	}
}

/*==================UPLOAD NHIỀU FILE LÊN SERVER===================================================================================*/
void FTP_Client::mput(char* listFileName)
{
	bool flag = 1;
	if (mode == 0)
	{
		flag = 0;
	}
	string temp = "";
	//string listfilenametoPUT = "";
	//Xử lý tách các fileName trong list file cần put
	char *temp1 = strdup(listFileName);
	char *temp2 = strtok(temp1, " \0");
	char *temp3 = strtok(NULL, " \0");
	if(temp3==NULL && temp2[0]!='*' || temp3!=NULL)
	{
		for (int i = 0; i <= strlen(listFileName); i++)
		{
			if (listFileName[i] != ' '&&listFileName[i] != NULL) //xét vòng for để tách từng file trong danh sách các file
				temp += listFileName[i];
			else //đã lấy được file
			{
				const char *filename = temp.c_str();
				FILE *file;
				memset(buf, 0, BUFSIZ);
				sprintf(buf, "%s%s", currentDirectoryPath, filename);
				file = fopen(buf, "rb");
				if (file == NULL)
				{
					cout << filename << " : File not found\n";
				}
				else
				{
					printf("mput %s? ", filename); // "filename" ??
					char skipfile = getch();
					if (skipfile == '\r')
					{
						cout << "\n";
						//Lấy từng filename put lên server
						put(filename);
						if (flag == 0)
						{
							mode = 0;
						}
						
					}
					fclose(file);
				}
				temp = "";
			}
		}
	}
	else //*.txt hoặc *.*
	{
		vector<string> listFilesNameToPut;
		temp1 = strdup(listFileName);
		string extention = string(temp1);
		getFilesList(currentDirectoryPath, extention, listFilesNameToPut);
		int i = 0;
		while(i<listFilesNameToPut.size())
		{
			const char *filename = listFilesNameToPut[i].c_str();

			if(stricmp(filename, ".")!=0 && stricmp(filename, "..")!=0)
			{
				FILE *file;
				memset(buf, 0, BUFSIZ);
				sprintf(buf, "%s%s", currentDirectoryPath, filename);
				file = fopen(buf, "rb");
				if (file == NULL)
				{
					cout << filename << " : File not found\n";
				}
				else
				{
					printf("mput %s? ", filename); // "filename" ??
					char skipfile = getch();
					cout << "\n";
					if (skipfile == '\r')
					{
					
						put(filename);
						if (flag == 0)
						{
							mode = 0;
						}

					}
					fclose(file);
				}
			}
			i++;
		}
	}
}
/*==================THAY ĐỔI ĐƯỜNG DẪN TRÊN SERVER ===================================================================================*/
void FTP_Client::cd(char* newDirectoryPath)
{
	memset(buf, 0, BUFSIZ);

	sprintf(buf, "cwd %s\r\n", newDirectoryPath);
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);
	memset(buf, 0, BUFSIZ);

	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	printf("%s", buf);
}
/*==================TẠO THƯ MỤC MỚI	===================================================================================*/
void FTP_Client::mkdir(char* folderName)
{
	memset(buf, BUFSIZ, 0);
	sprintf(buf, "xmkd %s\r\n", folderName);
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);
	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	printf("%s", buf);
}
/*==================XÓA THƯ MỤC RỖNG===================================================================================*/
void FTP_Client::rmdir(char* folderName)
{
	memset(buf, BUFSIZ, 0);
	sprintf(buf, "XRMD %s\r\n", folderName);
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);
	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	printf("%s", buf);
}
/*==================IN RA ĐƯỜNG DẪN CỦA THƯ MỤC HIỆN HÀNH===================================================================================*/
void FTP_Client::pwd()
{
	memset(buf, BUFSIZ, 0);
	sprintf(buf, "XPWD\r\n");
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);
	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
	printf("%s", buf);
}

/*==================THAY ĐỔI ĐƯỜNG DẪN TRÊN CLIENT===================================================================================*/
//Chek newDirectoryPath Exists ? 1|0
bool FTP_Client::dirExists(const char* newDirectoryPath)
{
	struct stat sb;
	if (stat(newDirectoryPath, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
void FTP_Client::lcd(char* newDirectoryPathClient)
{
	bool check;
	check = dirExists(newDirectoryPathClient);
	if (check == 1)//newDirectoryPathClient có tồn tại
	{
		memset(buf, BUFSIZ, 0);
		sprintf(buf, "Local directory now %s\r\n", newDirectoryPathClient);
		printf("%s", buf);
		strcpy(currentDirectoryPath, newDirectoryPathClient);
		strcat(currentDirectoryPath, "\\");
	}
	else//newDirectoryPathClient không tồn tại
	{
		char notfound[50] = "File not found";
		memset(buf, BUFSIZ, 0);
		sprintf(buf, "%s:: %s\r\n", newDirectoryPathClient, notfound);
		printf("%s", buf);
	}
}
/*=================================CHUYỂN SANG TRẠNG THÁI PASSIVE==========================================*/
void FTP_Client::passiveMode(int show)
{
	//(1) Client mở port “n” kết nối vào cổng lệnh của Server (port 21) => hàm Connector
	//Phát lệnh PASV.
	memset(buf, 0, BUFSIZ);
	sprintf(buf, "PASV\r\n");
	ClientSocket.Send(buf, BUFSIZ, 0);//Gửi lệnh passive lên server
	//Response:
	//(3) Server trả lời bằng lệnh “PORT (m)”, cho Client biết nó mở cổng “m” để nhận kết nối dữ liệu.
	memset(buf, 0, BUFSIZ);
	ClientSocket.Receive(buf, BUFSIZ, 0);//Nhận phản hồi từ sever : 227 Entering Passive Mode(a, b, c, d, q, r).
	if (show == 1)
	{
		printf("%s", buf);
	}
	//Extract Passive Port
	//227 Entering Passive Mode(a, b, c, d, q, r): Đây là thông tin về ip & (port/256; port%256) từ server
	int q = 0, r = 0, t = 0, count = 0;
	for (int i = 0; i < strlen(buf); i++)
	{
		if (isdigit(buf[i]))
		{
			t = t * 10 + (buf[i] - 48);
			if (!isdigit(buf[i + 1]))
			{
				count++;
				switch (count)
				{
				case 6: q = t; break;
				case 7: r = t; break;
				}
				t = 0;
			}
		}
	}
	//(3)Client tạo kết nối truyền dữ liệu từ cổng “n+1” của nó đến cổng truyền dữ liệu “m” của Server.
	//=> Chỗ này chỉ tính ra portPassive = port(m) chưa chưa tạo truyền kết nối dl từ công "n + 1"
	// => CV này sẽ được thực hiện trong hàm put & get
	//port(m) = portPassive
	portPASSIVE = q * 256 + r;// Từ hai thông số q và r : q = port/259. r = port%256. ta suy ngược lại được port
	mode = 0;//passive thi mode = 0

}
/*========================THOÁT KHỎI FTP SERVER=============================================================================================================*/
void FTP_Client::quit()
{
	memset(buf, BUFSIZ, 0);
	sprintf(buf, "QUIT\r\n");
	tmpres = ClientSocket.Send(buf, strlen(buf), 0);
	memset(buf, 0, sizeof buf);
	tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);//221
	printf("%s", buf);//221
}
/*=====================================KHỞI TẠO FTP_CLIENT==========================================================================*/
FTP_Client::FTP_Client()
{
	srand(time(NULL));
	port = (int)(1025 + rand() % 15000);
	mode = 1;
	portPASSIVE = 0; 
	//lấy đường dẫn hiện tại của file thực thi
	TCHAR fullPath[MAX_PATH];
	char FullPath[MAX_PATH];
	CHAR driveLetter[3];
	CHAR directory[MAX_PATH];
	GetModuleFileName(NULL, fullPath, MAX_PATH);
	wcstombs(FullPath, fullPath, MAX_PATH);
	_splitpath(FullPath, driveLetter, directory, NULL, NULL);
	sprintf(currentDirectoryPath, "%s%s", driveLetter, directory);
}
FTP_Client::~FTP_Client()
{

}

