#pragma once
#include "wx/wx.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#define DEFAULT_BUFLEN 8
#define DEFAULT_PORT "27015"

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

public:
	int Width = 3;
	int Height = 3;
	wxButton** btn;
	wxTextCtrl* m_txt;
	wxStaticText* text_time;
	char recvbuf[DEFAULT_BUFLEN];
	char sendbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int sendbuflen = DEFAULT_BUFLEN;
	int num = 0;
	bool turn = false;
	bool isServer = false;
	int hasDone = false;
	std::mutex mtx;
	std::condition_variable main_cv, thread_cv, cpu_cv, sendregret_cv, recvregret_cv;
	std::thread* thread_sever;
	std::thread* thread_client;
	std::thread* thread_min_max;
	std::thread* thread_sendregret;
	std::thread* thread_recvregret;
	std::thread* thread_qeasy;
	std::thread* thread_qmedium;
	std::thread* thread_qhard;
	int num_arr[9];
	const int CPU = 1;
	const int USER = CPU * -1;
	const int FULL_TIME = 60;
	int index1 = -1;
	std::thread* thread_clock;
	int resTime = FULL_TIME;
	wxChoice* ch1;
	bool PlayWithCPU = false;
	bool surrender = false;
	int last = -1;
	bool isregret = false;
	wxButton* btn_regret;
	wxButton* btn_lose;
	bool CanReciveRegret = false;
	bool agreeRegret = false;
	bool stopClock = false;
	bool isMinimax = true;
	int qlearningaction = -1;

	void OnButtonClicked(wxCommandEvent& evt);
	void Regret(wxCommandEvent& evt);
	void Lose(wxCommandEvent& evt);
	void CreatRoom(wxCommandEvent& evt);
	void JoinRoom(wxCommandEvent& evt);
	wxDECLARE_EVENT_TABLE();
	void server();
	void client();
	bool isGameOver();
	void initAllParam();
	void Clock();
	int min_max(bool);
	int cpu_won();
	int user_won();
	int isFull();
	void PlayWithMinMax(wxCommandEvent& evt);
	void _play_with_minimax();
	void RecvRegret();
	void SendRegret();
	void PlayWithEasy(wxCommandEvent& evt);
	void PlayWithMedium(wxCommandEvent& evt);
	void PlayWithHard(wxCommandEvent& evt);
	void PlayWithQLearning(int);
	int myhash(int* num_arr);
};

