#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include "cMain.h"
#include <string>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <vector>
#include <fstream>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define wxSP_BORDER   0x0200
#define wxSP_LIVE_UPDATE   0x0080

SOCKET ClientSocket = INVALID_SOCKET;
SOCKET ConnectSocket = INVALID_SOCKET;

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
wxEND_EVENT_TABLE()

cMain::cMain() :wxFrame(nullptr, wxID_ANY, "井字棋小游戏", wxPoint(30, 30), wxSize(800, 800)){
	std::fill(num_arr, num_arr + 9, 0);
	wxArrayString choices;
	choices.Add(wxT("player first"));
	choices.Add(wxT("computer first"));

	wxFont font(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);
	wxFont font_time(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);

	wxPanel* panel_head = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	wxPanel* panel_body = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer* bs1 = new wxBoxSizer(wxVERTICAL);
	wxPanel* panel_bodyleft = new wxPanel(panel_body, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxPanel* panel_bodyright = new wxPanel(panel_body, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer* bs2 = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* panel_play = new wxPanel(panel_bodyleft, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxPanel* panel_command = new wxPanel(panel_bodyleft, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer* bs3 = new wxBoxSizer(wxVERTICAL);
	btn_regret = new wxButton(panel_command, 20001, "悔棋");
	btn_lose = new wxButton(panel_command, 20002, "认输");
	wxBoxSizer* bs4 = new wxBoxSizer(wxHORIZONTAL);

	wxPanel* panel_find = new wxPanel(panel_bodyright, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxPanel* panel_help = new wxPanel(panel_bodyright, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer* bs5 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* bs6 = new wxBoxSizer(wxVERTICAL);
	wxButton* btn_creatRoom = new wxButton(panel_find, 20003, "创建房间");
	wxButton* btn_joinRoom = new wxButton(panel_find, 20004, "加入房间");
	wxButton* btn_minmax = new wxButton(panel_find, 20005, "挑战觉悟人机");
	wxButton* btn_easy = new wxButton(panel_find, 20006, "简单");
	wxButton* btn_medium = new wxButton(panel_find, 20007, "中等");
	wxButton* btn_hard = new wxButton(panel_find, 20008, "困难");
	ch1 = new wxChoice(panel_find, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
	wxBoxSizer* bs7 = new wxBoxSizer(wxHORIZONTAL);
	m_txt = new wxTextCtrl(panel_find, wxID_ANY, "");
	wxStaticText* text1 = new wxStaticText(panel_head, wxID_ANY, "井字棋小游戏");
	wxBoxSizer* bs8 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* bs9 = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* text2 = new wxStaticText(panel_command, wxID_ANY, "剩余时间：");
	text_time = new wxStaticText(panel_command, wxID_ANY, "--");
	wxStaticText* text3 = new wxStaticText(panel_command, wxID_ANY, "秒");
	wxBoxSizer* bs10 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* bs11 = new wxBoxSizer(wxVERTICAL);
	wxStaticText* text4 = new wxStaticText(panel_help, wxID_ANY, "1. 对战模式：您首先创建房间，将自己的ip告诉对方，对方将ip输入便可加入房间对战(默认房主先手)",wxDefaultPosition, wxSize(300, 100));
	wxStaticText* text5 = new wxStaticText(panel_help, wxID_ANY, "2. 人机模式：您首先选择谁先手，然后就可以点击相应的困难模式，进入对战", wxDefaultPosition, wxSize(300, 100));

	wxGridSizer* grid2 = new wxGridSizer(2, 1, 0, 0);


	panel_head->SetBackgroundColour(wxColor(66, 185, 131));
	panel_body->SetBackgroundColour(wxColor(66, 185, 131));
	panel_bodyleft->SetBackgroundColour(wxColor(66, 185, 131));
	panel_bodyright->SetBackgroundColour(wxColor(66, 185, 131));
	panel_play->SetBackgroundColour(wxColor(150, 100, 100));
	panel_command->SetBackgroundColour(wxColor(100, 150, 100));
	panel_find->SetBackgroundColour(wxColor(200, 100, 100));
	panel_help->SetBackgroundColour(wxColor(100, 200, 100));

	btn_regret->Enable(false);
	btn_lose->Enable(false);
	btn_regret->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::Regret, this);
	btn_lose->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::Lose, this);
	btn_creatRoom->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::CreatRoom, this);
	btn_joinRoom->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::JoinRoom, this);
	btn_minmax->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::PlayWithMinMax, this);
	btn_easy->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::PlayWithEasy, this);
	btn_medium->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::PlayWithMedium, this);
	btn_hard->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::PlayWithHard, this);
	

	text1->SetFont(font);
	text2->SetFont(font_time);
	text3->SetFont(font_time);
	text_time->SetFont(font_time);

	bs1->Add(panel_head, 1, wxEXPAND|wxLEFT | wxTOP | wxRIGHT, 5);
	bs1->Add(panel_body, 6, wxEXPAND | wxALL, 5);
	bs2->Add(panel_bodyleft, 5, wxEXPAND );
	bs2->Add(panel_bodyright, 3, wxEXPAND|wxLEFT, 5);
	bs3->Add(panel_play, 7, wxEXPAND);
	bs3->Add(panel_command, 3, wxEXPAND | wxTOP, 5);
	bs4->Add(btn_regret, 0);
	bs4->Add(btn_lose, 0);

	bs5->Add(panel_find, 8, wxEXPAND);
	bs5->Add(panel_help, 2, wxEXPAND);
	
	bs6->Add(btn_creatRoom, 0, wxALIGN_CENTER);
	bs6->Add(m_txt, 0, wxALIGN_CENTER);
	bs6->Add(btn_joinRoom, 0, wxALIGN_CENTER);
	bs6->Add(ch1, 0, wxALIGN_CENTER);
	bs6->Add(btn_minmax, 0, wxALIGN_CENTER);
	bs6->Add(btn_easy, 0, wxALIGN_CENTER);
	bs6->Add(btn_medium, 0, wxALIGN_CENTER);
	bs6->Add(btn_hard, 0, wxALIGN_CENTER);
	
	bs7->Add(bs6, 1, wxALIGN_CENTER);

	bs8->Add(text1, 0, wxALIGN_CENTER);
	bs9->Add(bs8, 1, wxALIGN_CENTER);

	bs10->Add(text2, 0, wxALIGN_CENTER);
	bs10->Add(text_time, 0, wxALIGN_CENTER);
	bs10->Add(text3, 0, wxALIGN_CENTER);
	bs11->Add(bs10, 0, wxALIGN_CENTER);
	bs11->Add(bs4, 0, wxALIGN_CENTER);
	grid2->Add(text4, 1 );
	grid2->Add(text5, 1);

	this->SetSizer(bs1);
	panel_head->SetSizerAndFit(bs9);
	panel_body->SetSizerAndFit(bs2);
	panel_bodyleft->SetSizerAndFit(bs3);
	panel_command->SetSizerAndFit(bs11);
	panel_bodyright->SetSizerAndFit(bs5);
	panel_find->SetSizerAndFit(bs7);
	panel_help->SetSizer(grid2);
	grid2->Layout();
	

	btn = new wxButton * [Width * Height];

	wxGridSizer* grid = new wxGridSizer(Width, Height, 0 , 0);
	
	for (int x = 0; x < Width; ++x) {
		for (int y = 0; y < Height; ++y) {
			btn[x * Width + y] = new wxButton(panel_play, 10000 + (x * Width + y));
			btn[x * Width + y]->SetFont(font);
			grid->Add(btn[x * Width + y], 1, wxEXPAND | wxALL);

			btn[x * Width + y]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::OnButtonClicked, this);
		}
	}

	panel_play->SetSizer(grid);
	grid->Layout();
}

cMain::~cMain() {
	delete[]btn;
}

void cMain::OnButtonClicked(wxCommandEvent& evt) {
	if (turn) {
		std::unique_lock<std::mutex> lck(mtx);
		int x = (evt.GetId() - 10000) / Width;
		int y = (evt.GetId() - 10000) % Width;
		//const std::string s = std::to_string(x) + std::to_string(y);
		//wxMessageBox(s);
		btn[x * Width + y]->Enable(false);
		last = x * Width + y;
		if (!PlayWithCPU) {
			strcpy(sendbuf, (std::to_string(x * Width + y) + "0").c_str());
			if (isServer) {
				btn[x * Width + y]->SetLabel("X");
				num_arr[x * Width + y] = 1;
			}
			else {
				btn[x * Width + y]->SetLabel("O");
				num_arr[x * Width + y] = -1;
			}
			hasDone = true;
			turn = false;
			++num;
			thread_cv.notify_one();
		}
		else {
			btn[x * Width + y]->SetLabel("X");
			num_arr[x * Width + y] = USER;
			hasDone = true;
			turn = false;
			++num;
			cpu_cv.notify_one();
		}
	}
	evt.Skip();
}

void cMain::Regret(wxCommandEvent& evt) {
	if (PlayWithCPU&&last != -1) {
		btn[last]->SetLabelText("");
		btn[last]->Enable(true);
		num_arr[last] = 0;
		--num;

		if (isMinimax) {
			btn[index1]->SetLabelText("");
			btn[index1]->Enable(true);
			num_arr[index1] = 0;
			last = -1;
		}
		else {
			btn[qlearningaction]->SetLabelText("");
			btn[qlearningaction]->Enable(true);
			num_arr[qlearningaction] = 0;
		}
		last = -1;
		--num;
		btn_regret->Enable(false);
	}
	else if (!PlayWithCPU) {
		if (!turn && last != -1) {
			isregret = true;
			strcpy(sendbuf, std::string("04").c_str());
			SendRegret();
		}
	}
	evt.Skip();
}

void cMain::Lose(wxCommandEvent& evt) {
	if (turn) {
		if (PlayWithCPU) {
			surrender = true;
			cpu_cv.notify_one();
		}
		else {
			surrender = true;
			strcpy(sendbuf, std::string("03").c_str());
			thread_cv.notify_one();
		}
	}
	evt.Skip();
}

void cMain::CreatRoom(wxCommandEvent& evt) {
	thread_sever = new std::thread(&cMain::server, this);
	evt.Skip();
}

void cMain::JoinRoom(wxCommandEvent& evt) {
	//wxMessageBox("join room");
	thread_client = new std::thread (&cMain::client, this);
	//wxMessageBox("cr created");
	//thread_client->join();
	//wxMessageBox("after join");
	evt.Skip();
}

void cMain::server() {
	std::unique_lock<std::mutex> lck(mtx);
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	wxMessageBox("房间创建成功！");

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	wxMessageBox("匹配成功！");

	turn = true;
	isServer = true;
	thread_clock = new std::thread(&cMain::Clock, this);
	thread_clock->detach();
	while (!hasDone && resTime && !surrender) {
		CanReciveRegret = true;
		thread_cv.wait(lck);
	}
	stopClock = true;
	hasDone = false;
	if (resTime == 0) {
		std::strcpy(sendbuf, std::string("12").c_str());
		stopClock = false;
		wxMessageBox("超时!你输了！");
	}
	else if (surrender) {
		wxMessageBox("你认输了!");
	}
	iSendResult = send(ClientSocket, sendbuf, std::strlen(sendbuf), 0);
	if (iSendResult == SOCKET_ERROR) {
		
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}
	bool ServerTimeoutFlag = false;
	bool ClientSurrender = false;
	while (num < 9 && resTime != 0 && !surrender) {
		if (last != -1) {
			btn_regret->Enable(true);
		}
		turn = false;
		btn_lose-> Enable(false);
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

		if (iResult > 0) {
			btn_regret->Enable(false);
			int point = recvbuf[0] - '0';
			int control = recvbuf[1] - '0';
			if (control == 2) {
				ServerTimeoutFlag = true;
				wxMessageBox("对方超时，你赢了！");
				break;
			}
			else if (control == 3) {
				ClientSurrender = true;
				wxMessageBox("对方认输！");
				break;
			}
			else if (control == 5) {
				--num;
				btn[last]->SetLabelText("");
				btn[last]->Enable(true);
				num_arr[last] = 0;
				last = -1;
			}
			else if (control == 6) {
				wxMessageBox("对方不同意您悔棋!");
				continue;
			}
			//wxMessageBox(std::to_string(point));
			if (control != 5 && control != 6) {
				btn[point]->Enable(false);
				btn[point]->SetLabel("O");
				num_arr[point] = -1;
				++num;
			}
			if (control == 1) {
				wxMessageBox("输啦");
				break;
			}
			//take an action 

			turn = true;
			thread_clock = new std::thread(&cMain::Clock, this);
			thread_clock->detach();
			btn_regret->Enable(false);
			btn_lose->Enable(true);
			while (!hasDone && resTime != 0&& !surrender && !agreeRegret) {
				CanReciveRegret = true;
				thread_recvregret = new std::thread(&cMain::RecvRegret, this);
				thread_recvregret->detach();
				thread_cv.wait(lck);
			}
			stopClock = true;
			CanReciveRegret = false;
			hasDone = false;
			std::this_thread::sleep_for(std::chrono::microseconds(500));

			if (resTime == 0) {
				wxMessageBox("超时！你输了!");
				stopClock = false;
				strcpy(sendbuf, std::string("02").c_str());
				break;
			}
			else if(surrender) {
				iSendResult = send(ClientSocket, sendbuf, std::strlen(sendbuf), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return;
				}
				wxMessageBox("你投降了!");
				break;
			}
			else if (agreeRegret) {
				btn[point]->Enable(true);
				btn[point]->SetLabel("");
				num_arr[point] = 0;
				--num;
				agreeRegret = false;
				last = -1;
				continue;
			}
			bool resflag = isGameOver();
			if (resflag) {
				sendbuf[1] = '1';
			}
			iSendResult = send(ClientSocket, sendbuf, std::strlen(sendbuf), 0);
			if (resflag) {
				wxMessageBox("赢啦");
				break;
			}
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return;
			}
		}
		else if (iResult < 0) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return ;
		}
	}

	if (!isGameOver()&& resTime != 0 && !ServerTimeoutFlag && !surrender && !ClientSurrender) {
		wxMessageBox("平局");
	}
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	initAllParam();
}

void cMain::client() {
	std::unique_lock<std::mutex> lck(mtx);
	const std::string ip_s = std::string(m_txt->GetValue().mb_str(wxConvUTF8));
	char ip[20] = "127.0.0.1";
	strcpy(ip, ip_s.c_str());
	WSADATA wsaData;
	ConnectSocket = INVALID_SOCKET;
	
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}
	wxMessageBox("匹配成功");
	bool ClientTimeoutflag = false;
	bool ServerSurrender = false;
	while (true) {
		if (num > 1 && last != -1) {
			btn_regret->Enable(true);
		}
		turn = false;
		btn_lose->Enable(false);
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

		if (iResult > 0) {
			btn_regret->Enable(false);
			int point = recvbuf[0] - '0';
			int control = recvbuf[1] - '0';

			if (control == 2) {
				wxMessageBox("对方超时！你赢了！");
				ClientTimeoutflag = true;
				break;
			}
			else if (control == 3) {
				wxMessageBox("对方投降，你赢了!");
				ServerSurrender = true;
				break;
			}
			else if (control == 5) {
				--num;
				btn[last]->SetLabelText("");
				btn[last]->Enable(true);
				num_arr[last] = 0;
				last = -1;
			}
			else if (control == 6) {
				wxMessageBox("对方不同意您悔棋!");
				continue;
			}

			if (control != 5 && control != 6) {
				btn[point]->Enable(false);
				btn[point]->SetLabel("X");
				num_arr[point] = 1;
				++num;
			}
			if (num >= 9) {
				break;
			}
			if (control == 1) {
				wxMessageBox("输啦");
				break;
			}

			//take an action 
			turn = true;
			thread_clock = new std::thread(&cMain::Clock, this);
			thread_clock->detach();
			btn_regret->Enable(false);
			btn_lose->Enable(true);
			while (!hasDone && resTime && !surrender&&!agreeRegret) {
				CanReciveRegret = true;
				thread_recvregret = new std::thread(&cMain::RecvRegret, this);
				thread_recvregret->detach();
				thread_cv.wait(lck);
			}
			hasDone = false;
			stopClock = true;
			CanReciveRegret = false;
			std::this_thread::sleep_for(std::chrono::microseconds(500));
	
			if (resTime == 0) {
				stopClock = false;
				strcpy(sendbuf, std::string("12").c_str());
				iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return;
				}
				wxMessageBox("超时，你输了！");
				break;
			}
			else if (surrender) {
				iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return;
				}
				wxMessageBox("投降，你输了！");
				break;
			}
			else if (agreeRegret) {
				--num;
				btn[point]->SetLabelText("");
				btn[point]->Enable(true);
				num_arr[point] = 0;
				/*wxMessageBox("agreeRegret");*/
				agreeRegret = false;
				last = -1;
				continue;
			}
			bool resflag = isGameOver();
			if (num >= 6 && resflag) {
				sendbuf[1] = '1';
			}

			iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
			if (resflag) {
				wxMessageBox("赢啦!");
				break;
			}
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return;
			}
		}
		else if (iResult < 0) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return;
		}
	}
	if (!isGameOver() && resTime && !ClientTimeoutflag && !ServerSurrender&&!surrender) {
		wxMessageBox("平手！");
	}
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	closesocket(ConnectSocket);
	WSACleanup();
	initAllParam();
}

bool cMain::isGameOver() {
	for (int i = 0; i <= 6; i+=3) {
		int res = 0;
		for (int j = 0; j < 3; ++j) {
			res += num_arr[i + j];
		}
		if (std::abs(res) == 3) {
			return true;
		}
	}
	for (int i = 0; i <= 2; ++i) {
		int res = 0;
		for (int j = 0; j < 3; ++j) {
			res += num_arr[i + 3 * j];
		}
		if (std::abs(res) == 3) {
			return true;
		}
	}
	int res = 0;
	for (int j = 0; j < 3; ++j) {
		res += num_arr[0 + 4 * j];
	}
	if (std::abs(res) == 3) {
		return true;
	}
	res = 0;
	for (int j = 0; j < 3; ++j) {
		res += num_arr[2 + 2 * j];
	}
	if (std::abs(res) == 3) {
		return true;
	}
	
	return false;
}

void cMain::initAllParam() {
	num = 0;
    turn = false;
    isServer = false;
	hasDone = false;
	resTime = FULL_TIME;
	PlayWithCPU = false;
	surrender = false;
	last = -1;
	index1 = -1;
	isregret = false;
	CanReciveRegret = false;
	agreeRegret = false;
	qlearningaction = -1;
	std::fill(num_arr, num_arr + 9, 0);
	stopClock = false;
	for (int x = 0; x < Width; ++x) {
		for (int y = 0; y < Height; ++y) {
		      btn[x * Width + y]->Enable(true);
			  btn[x * Width + y]->SetLabel("");
		}
	}
	btn_regret->Enable(false);
	btn_lose->Enable(false);
}

void cMain::Clock() {
	resTime = FULL_TIME - 1;
	while (turn && resTime && !stopClock) {
		text_time->SetLabelText(std::to_string(resTime));
		std::this_thread::sleep_for(std::chrono::seconds(1));
		resTime--;
	}
	text_time->SetLabelText("--");
	if (resTime == 0) {
		if (PlayWithCPU) {
			cpu_cv.notify_one();
		}
		else {
			thread_cv.notify_one();
		}
	}
	stopClock = false;
}

int cMain::cpu_won()
{
	for (int i = 0; i < 9; i += 3)
	{
		if ((num_arr[i] == num_arr[i + 1]) && (num_arr[i + 1] == num_arr[i + 2]) && (num_arr[i] == CPU))
			return 1;
	}
	for (int i = 0; i < 3; i++)
	{
		if ((num_arr[i] == num_arr[i + 3]) && (num_arr[i + 3] == num_arr[i + 6]) && (num_arr[i] == CPU))
			return 1;
	}
	if ((num_arr[0] == num_arr[4]) && (num_arr[4] == num_arr[8]) && (num_arr[0] == CPU))
	{
		return 1;
	}
	if ((num_arr[2] == num_arr[4]) && (num_arr[4] == num_arr[6]) && (num_arr[2] == CPU))
	{
		return 1;
	}
	return 0;
}

int cMain::user_won()
{
	for (int i = 0; i < 9; i += 3)
	{
		if ((num_arr[i] == num_arr[i + 1]) && (num_arr[i + 1] == num_arr[i + 2]) && (num_arr[i] == USER))
			return 1;
	}
	for (int i = 0; i < 3; i++)
	{
		if ((num_arr[i] == num_arr[i + 3]) && (num_arr[i + 3] == num_arr[i + 6]) && (num_arr[i] == USER))
			return 1;
	}
	if ((num_arr[0] == num_arr[4]) && (num_arr[4] == num_arr[8]) && (num_arr[0] == USER))
	{
		return 1;
	}
	if ((num_arr[2] == num_arr[4]) && (num_arr[4] == num_arr[6]) && (num_arr[2] == USER))
	{
		return 1;
	}
	return 0;
}

int cMain::isFull()// Board is full
{
	for (int i = 0; i < 9; i++)
	{
		if (num_arr[i] == 0)
		{
			return 0;
		}
	}
	return 1;
}

int cMain::min_max(bool flag) {

	int max_val = -1000, min_val = 1000;
	int i, j, value = 1;
	if (cpu_won() == 1)
	{
		return 10;
	}
	else if (user_won() == 1)
	{
		return -10;
	}
	else if (isFull() == 1)
	{
		return 0;
	}
	int score[9] = { 1,1,1,1,1,1,1,1,1 };//if score[i]=1 then it is empty

	for (i = 0; i < 9; i++)
	{
		if (num_arr[i] == 0)
		{
			if (min_val > max_val) // reverse of pruning condition.....
			{
				if (flag == true)
				{
					num_arr[i] = CPU;
					value = min_max(false);
				}
				else
				{
					num_arr[i] = USER;
					value = min_max(true);
				}
				num_arr[i] = 0;
				score[i] = value;
			}
		}
	}

	if (flag == true)
	{
		max_val = -1000;
		for (j = 0; j < 9; j++)
		{
			if (score[j] > max_val && score[j] != 1)
			{
				max_val = score[j];
				index1 = j;
			}
		}
		return max_val;
	}
	if (flag == false)
	{
		min_val = 1000;
		for (j = 0; j < 9; j++)
		{
			if (score[j] < min_val && score[j] != 1)
			{
				min_val = score[j];
				index1 = j;
			}
		}
		return min_val;
	}
}

void cMain::PlayWithMinMax(wxCommandEvent& evt) {
	PlayWithCPU = true;
	isMinimax = true;
	thread_min_max = new std::thread(&cMain::_play_with_minimax, this);
	btn_regret->Enable(true);
	btn_lose->Enable(true);
	evt.Skip();
}

void cMain::_play_with_minimax() {
	std::string s = std::string(ch1->GetStringSelection().mb_str(wxConvUTF8));
	std::unique_lock<std::mutex> lck(mtx);
	if (s == "player first") {
		//player do action
		turn = true;
		thread_clock = new std::thread(&cMain::Clock, this);
		thread_clock->detach();
		while (!hasDone && resTime && !surrender) {
			cpu_cv.wait(lck);
		}
		stopClock = true;
		hasDone = false;

		if (!resTime) {
			wxMessageBox("超时了!");
			stopClock = false;
			initAllParam();
			return;
		}
		else if (surrender) {
			wxMessageBox("认输!");
			initAllParam();
			return;
		}
	}

	while (num < 9) {
		min_max(true);
		num_arr[index1] = CPU;
		btn[index1]->SetLabel("O");
		btn[index1]->Enable(false);
		++num; 
		if (cpu_won() == 1) {
			wxMessageBox("人机赢了!");
			break;
		}
		else if (isFull() == 1) {
			wxMessageBox("平手!");
			break;
		}

		turn = true;
		thread_clock = new std::thread(&cMain::Clock, this);
		thread_clock->detach();
		if (last == -1) {
			btn_regret->Enable(false);
		}
		else {
			btn_regret->Enable(true);
		}
		while (!hasDone && resTime && !surrender) {
			cpu_cv.wait(lck);
		}
		hasDone = false;
		stopClock = true;
		if (!resTime) {
			wxMessageBox("超时了!");
			stopClock = false;
			break;
		}
		else if (surrender) {
			wxMessageBox("认输!");
			initAllParam();
			return;
		}
		if (user_won() == 1) {
			wxMessageBox("太厉害了，打败了觉悟人机!");
			break;
		}

		if (isFull() == 1) {
			wxMessageBox("平手");
			break;
		}
	}

	initAllParam();
}

void cMain::SendRegret() {
	int iSendResult;
	if (isServer) {
		iSendResult = send(ClientSocket, sendbuf, std::strlen(sendbuf), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}
	}
	else {
		iSendResult = send(ConnectSocket, sendbuf, std::strlen(sendbuf), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return;
		}
	}

}

void cMain::RecvRegret() {
	int iResult;
	int control;
	int iSendResult;
	struct timeval timeout = { 0,300 };
	struct fd_set rfds;
	int ret;
	if (isServer) {
		while(1) {
			FD_ZERO(&rfds);
			FD_SET(ClientSocket, &rfds);
			ret = select(0, &rfds, NULL, NULL, &timeout);
			if (ret < 0)
			{
				closesocket(ClientSocket);
				return;
			}
			else if (ret == 0 && CanReciveRegret == false)
			{
				break;
			}
			else {
				if (FD_ISSET(ClientSocket, &rfds)) {
					iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
					if (iResult > 0) {
						control = recvbuf[1] - '0';
						if (control == 4) {
							wxMessageDialog* dial = new wxMessageDialog(NULL,
								wxT("同意对方悔棋吗?"), wxT("Question"),
								wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
							int value = dial->ShowModal();
							/*wxMessageBox(wxString(std::to_string(value)));*/
							if (value == 5103) {
								/*wxMessageBox("here");*/
								strcpy(sendbuf, std::string("05").c_str());
								iSendResult = send(ClientSocket, sendbuf, std::strlen(sendbuf), 0);
								if (iSendResult == SOCKET_ERROR) {
									closesocket(ClientSocket);
									WSACleanup();
									return;
								}
								agreeRegret = true;
								thread_cv.notify_one();
							}
							else if (value == 5104) {
								strcpy(sendbuf, std::string("06").c_str());
								iSendResult = send(ClientSocket, sendbuf, std::strlen(sendbuf), 0);
								if (iSendResult == SOCKET_ERROR) {
									closesocket(ClientSocket);
									WSACleanup();
									return;
								}
							}
						}
					}
					else if (iResult < 0) {
						printf("recv failed with error: %d\n", WSAGetLastError());
						closesocket(ClientSocket);
						WSACleanup();
					}
				}
			}

		}
	}
	else {
		while (1) {
			FD_ZERO(&rfds);
			FD_SET(ConnectSocket, &rfds);
			ret = select(0, &rfds, NULL, NULL, &timeout);
			if (ret < 0)
			{
				closesocket(ConnectSocket);
				return;
			}
			else if (ret == 0 && CanReciveRegret == false)
			{
				break;
			}
			else {
				if (FD_ISSET(ConnectSocket, &rfds)) {
					iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
					if (iResult > 0) {
						control = recvbuf[1] - '0';
						if (control == 4) {
							wxMessageDialog* dial = new wxMessageDialog(NULL,
								wxT("同意对方悔棋吗?"), wxT("Question"),
								wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
							int value = dial->ShowModal();
							/*wxMessageBox(wxString(std::to_string(value)));*/
							if (value == 5103) {
								/*wxMessageBox("here");*/
								strcpy(sendbuf, std::string("05").c_str());
								iSendResult = send(ConnectSocket, sendbuf, std::strlen(sendbuf), 0);
								if (iSendResult == SOCKET_ERROR) {
									closesocket(ConnectSocket);
									WSACleanup();
									return;
								}
								agreeRegret = true;
								thread_cv.notify_one();
							}
							else if (value == 5104) {
								strcpy(sendbuf, std::string("06").c_str());
								iSendResult = send(ConnectSocket, sendbuf, std::strlen(sendbuf), 0);
								if (iSendResult == SOCKET_ERROR) {
									closesocket(ConnectSocket);
									WSACleanup();
									return;
								}
							}
						}
					}
					else if (iResult < 0) {
						printf("recv failed with error: %d\n", WSAGetLastError());
						closesocket(ConnectSocket);
						WSACleanup();
					}
				}
			}

		}
	}
}

void cMain::PlayWithEasy(wxCommandEvent& evt) {
	PlayWithCPU = true;
	isMinimax = false;
	thread_qeasy = new std::thread(&cMain::PlayWithQLearning, this, 0);
	btn_regret->Enable(true);
	btn_lose->Enable(true);
	evt.Skip();
}
void cMain::PlayWithMedium(wxCommandEvent& evt) {
	PlayWithCPU = true;
	isMinimax = false;
	thread_qmedium = new std::thread(&cMain::PlayWithQLearning, this, 1);
	btn_regret->Enable(true);
	btn_lose->Enable(true);
	evt.Skip();

}
void cMain::PlayWithHard(wxCommandEvent& evt) {
	PlayWithCPU = true;
	isMinimax = false;
	thread_qhard = new std::thread(&cMain::PlayWithQLearning, this, 2);
	btn_regret->Enable(true);
	btn_lose->Enable(true);
	evt.Skip();
}

void cMain::PlayWithQLearning(int mode){
	const int QTableSize = 19683;
	const double INF = 10000000;
	std::string  filename = "Q-learning/episode";
	std::string btnname;
	std::vector<std::pair<int, double>> Qa[QTableSize];
	std::ifstream infile;
	if (mode == 0) {
		filename += "200.txt";
		btnname = "简单";
	}
	else if (mode == 1) {
		filename += "400.txt";
		btnname = "中等";
	}
	else if (mode == 2) {
		filename += "800.txt";
		btnname = "困难";
	}
	infile.open(filename);
	for (int i = 0; i < QTableSize; ++i) {
		int actionnum;
		infile >> actionnum;
		for (int j = 0; j < actionnum; ++j) {
			int action;
			double value;
			infile >> action >> value;
			Qa[i].push_back(std::make_pair(action, value));
		}
	}

	std::string s = std::string(ch1->GetStringSelection().mb_str(wxConvUTF8));
	std::unique_lock<std::mutex> lck(mtx);
	if (s == "player first") {
		//player do action
		turn = true;
		thread_clock = new std::thread(&cMain::Clock, this);
		thread_clock->detach();
		while (!hasDone && resTime && !surrender) {
			cpu_cv.wait(lck);
		}
		stopClock = true;
		hasDone = false;

		if (!resTime) {
			wxMessageBox("超时了!");
			stopClock = false;
			initAllParam();
			return;
		}
		else if (surrender) {
			wxMessageBox("认输!");
			initAllParam();
			return;
		}
	}

	while (num < 9) {
		int state = myhash(num_arr);
		//wxMessageBox(std::to_string(state));
		int action = -1;
		double t = -INF;
		for (auto p : Qa[state]) {
			if (p.second > t) {
				t = p.second;
				action = p.first;
			}
		}
		qlearningaction = action;
		//wxMessageBox(std::to_string(action));
		btn[action]->SetLabel("O");
		btn[action]->Enable(false);
		num_arr[action] = CPU;
		++num;
		if (cpu_won() == 1) {
			wxMessageBox("人机赢了!");
			break;
		}
		else if (isFull() == 1) {
			wxMessageBox("平手!");
			break;
		}

		turn = true;
		thread_clock = new std::thread(&cMain::Clock, this);
		thread_clock->detach();
		if (last == -1) {
			btn_regret->Enable(false);
		}
		else {
			btn_regret->Enable(true);
		}
		while (!hasDone && resTime && !surrender) {
			cpu_cv.wait(lck);
		}
		hasDone = false;
		stopClock = true;
		if (!resTime) {
			wxMessageBox("超时了!");
			stopClock = false;
			break;
		}
		else if (surrender) {
			wxMessageBox("认输!");
			initAllParam();
			return;
		}
		if (user_won() == 1) {
			wxMessageBox("太厉害了，打败了"+btnname+"人机!");
			break;
		}

		if (isFull() == 1) {
			wxMessageBox("平手");
			break;
		}
	}

	initAllParam();
}

int cMain::myhash(int * num_arr) {
	int ans = 0;
	int pow = 1;
	for (int i = 0; i < 9; ++i) {
		if (num_arr[i] == -1) {
			ans += pow;
		}
		else if (num_arr[i] == 1) {
			ans += pow * 2;
		}
		pow *= 3;
	}
	return ans;
}
