/*
== == == == == == == == == == == == == == == == == == == == ==
Title:  REVE_ATCEngine
Author: Abhilash / Ardhendu / Krishan
Copyright(c) Reve Antivirus Inc.  All Rights Reserved.
This software is the proprietary information of Reve Antivirus
== == == == == == == == == == == == == == == == == == == == ==
*/
#include <tchar.h>
#include <strsafe.h>
#include <locale>
#include <codecvt>
#include <sstream>
#include <ctime>
#include <stdlib.h>
#include<string.h>
#include"Logger.h"
#include <thread>
using namespace std;
//using namespace System;
#ifndef CHAR_T
#define CHAR_T wchar_t
#endif
#pragma comment(lib, "advapi32.lib")
#pragma region Includes
#include <Wtsapi32.h>
#pragma comment(lib, "wtsapi32.lib")
#include <userenv.h>
#pragma comment(lib, "userenv.lib")
#pragma endregion
#pragma warning(disable : 4996)
SERVICE_STATUS_HANDLE g_ServiceStatusHandle;
HANDLE g_StopEvent;
DWORD g_CurrentState = 0;
int servicestop = 0;
bool g_SystemShutdown = false;

char logFileLocation[MAX_PATH];
char logFlag[MAX_PATH];
char FolderToWatch[MAX_PATH];

Logger logger;
bool settingsChangeFlag = false;

DWORD WINAPI keepAliveThread(LPVOID);
DWORD WINAPI fileWatcher(LPVOID);

//std::wstring s2ws(const std::string& s)
//{
//	int len;
//	int slength = (int)s.length() + 1;
//	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
//	wchar_t* buf = new wchar_t[len];
//	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
//	std::wstring r(buf);
//	delete[] buf;
//	return r;
//}
//
//std::string ws2s(const std::wstring& s)
//{
//	int len;
//	int slength = (int)s.length() + 1;
//	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
//	char* buf = new char[len + 1];
//	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
//	buf[len] = '\0';
//	std::string r(buf);
//	delete[] buf;
//	//Logger::info("WS2S: END");
//	return r;
//}

ULONGLONG getCurrentTimeStamp()
{
	FILETIME ftime;
	GetSystemTimeAsFileTime(&ftime);
	ULONGLONG ret = (LONGLONG)ftime.dwLowDateTime + ((LONGLONG)(ftime.dwHighDateTime) << 32LL);
	ret = ret / 10000 - 11644473600000LL;
	return ret;
}

BOOL IsWindowsXP()
{
	BOOL isWindowsXp = FALSE;

	OSVERSIONINFO vi;
	ZeroMemory(&vi, sizeof(OSVERSIONINFO));
	vi.dwOSVersionInfoSize = sizeof vi;
	GetVersionEx(&vi);
	if ((vi.dwPlatformId == VER_PLATFORM_WIN32_NT) && (vi.dwMajorVersion == 5))
	{
		isWindowsXp = TRUE;
	}

	return isWindowsXp;
}
//const wchar_t* GetWC(const char* c)
//{
//	const size_t cSize = strlen(c) + 1;
//	wchar_t* wc = new wchar_t[cSize];
//	mbstowcs(wc, c, cSize);
//	return wc;
//}

int mains()
{
	DWORD   dwThreadIdArray;
	HANDLE threads[2];
	//Configuration config;
	//HINSTANCE hinstLib;

	if (servicestop)
		exit(0);

	threads[0] = CreateThread(NULL, 0, keepAliveThread, NULL, 0, &dwThreadIdArray);
	threads[1] = CreateThread(NULL, 0, fileWatcher, NULL, 0, &dwThreadIdArray);
	if (threads[0] != 0 )
		WaitForSingleObject(threads[0], INFINITE);
	if (threads[1] != 0 )
		TerminateThread(threads[1], 1);
	//WaitForMultipleObjects(2, threads, TRUE, INFINITE);

	if (threads[0])
		CloseHandle(threads[0]);
	if (threads[1])
		CloseHandle(threads[1]);
	Logger::info("Stopping ATC Engine .......................................................");
	return 0;
}
//
//wchar_t* wstringToCharArray(wstring s)
//{
//	wchar_t* cstr = (wchar_t*)malloc(sizeof(wchar_t) * s.size() + 1);
//	wcscpy(cstr, s.c_str());	// or pass &s[0]
//	cstr[s.size()] = '\0';
//	return cstr;
//}

void InitLogger()
{
	int temp;
	temp = GetPrivateProfileIntA("Debug Mode", "ATCEngine", 0, logFlag);

	if (temp > 0)
	{
		logger.initLogger(logFileLocation, 2);
		logger.setLevel(5);
	}
}

DWORD WINAPI keepAliveThread(LPVOID data)
{

	BOOL isXp = IsWindowsXP();
	char* appdata, * appfiles;
	int retry = 5;

	if (isXp)
		appdata = getenv("PROGRAMFILES");
	else
		appdata = getenv("PROGRAMDATA");

	appfiles = getenv("PROGRAMFILES");

	sprintf_s(logFileLocation, 255, "%s\\REVE Antivirus\\Common\\log\\ATCEngine.txt", appdata);
	sprintf_s(logFlag, 255, "%s\\REVE Antivirus\\Common\\data\\settings.ini", appdata);

	InitLogger();     // Initiate Logger
	//Logger::info("logger initiated");
	Logger::info("Starting ATC Engine .......................................................");
	Logger::info("keep alive thread created");
	printf("keepAliveThread Thread\n");

	//DO YPUR STUFF HERE
	int count = 0;
	while (true) {
		if (servicestop)
			break;
		Logger::debug("count: %d", count++);
		Sleep(10000);
	}
	return 0;
}

DWORD WINAPI fileWatcher(LPVOID data)
{
	BOOL isXp = IsWindowsXP();
	HANDLE hEvents;
	char* appdata, * appfiles;
	int retry = 5;

	if (isXp)
		appdata = getenv("PROGRAMFILES");
	else
		appdata = getenv("PROGRAMDATA");

	appfiles = getenv("PROGRAMFILES");

	//folder path to watch for changes
	sprintf_s(FolderToWatch, 255, "%s\\REVE Antivirus\\Common\\dlp\\", appdata);
	do
	{
		int j = 0;
		while (j++ < 10)
		{
			Sleep(500);
		}
		if (servicestop)
			break;
		hEvents = FindFirstChangeNotificationA(FolderToWatch, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
		WaitForSingleObject(hEvents, INFINITE);
		Logger::info("Change in Settings Detected");

		settingsChangeFlag = true;
		//Logger::info("Settings Read Successfully");
		Sleep(10);
	} while (!g_SystemShutdown);


	return 0;
}

void ReportStatus(DWORD state)
{
	g_CurrentState = state;
	SERVICE_STATUS serviceStatus = {
		SERVICE_WIN32_OWN_PROCESS,
		g_CurrentState,
		state == SERVICE_START_PENDING ? 0 : SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN,
		NO_ERROR,
		0,
		0,
		0,
	};
	SetServiceStatus(g_ServiceStatusHandle, &serviceStatus);
}

DWORD WINAPI HandlerEx(DWORD control, DWORD eventType, void* eventData, void* context)
{
	switch (control)
	{
	case SERVICE_CONTROL_PRESHUTDOWN:
	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		ReportStatus(SERVICE_STOP_PENDING);
		g_SystemShutdown = true;
		servicestop = 1;

		SetEvent(g_StopEvent);

		break;
	case SERVICE_CONTROL_DEVICEEVENT:
		break;
	default:
		ReportStatus(g_CurrentState);
		break;
	}
	return NO_ERROR;
}

void WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
	//WriteEventLogEntry(L"Start Installation", EVENTLOG_INFORMATION_TYPE);
	g_ServiceStatusHandle = RegisterServiceCtrlHandlerEx(L"SERVICE NAME", &HandlerEx, NULL);
	ReportStatus(SERVICE_START_PENDING);
	g_StopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ReportStatus(SERVICE_RUNNING);
	//ReportStatus(SERVICE_STOPPED);

	int ActiveStatus = 0;
	DWORD   dwThreadIdArray;
	HANDLE threads[2];
	//HINSTANCE hinstLib;


	if (servicestop)
		exit(0);
	Logger::info("Service Main function");
	threads[0] = CreateThread(NULL, 0, keepAliveThread, NULL, 0, &dwThreadIdArray);
	threads[1] = CreateThread(NULL, 0, fileWatcher, NULL, 0, &dwThreadIdArray);

	if (threads[0] != 0 )
		WaitForSingleObject(threads[0], INFINITE);
	if (threads[1] != 0 )
		TerminateThread(threads[1], 1);
	//WaitForMultipleObjects(2, threads, TRUE, INFINITE);

	if (threads[0])
		CloseHandle(threads[0]);
	if (threads[1])
		CloseHandle(threads[1]);

	Logger::info("Stopping ATC Engine .......................................................");
	ReportStatus(SERVICE_STOP_PENDING);
	//CloseHandle(g_StopEvent);
	ReportStatus(SERVICE_STOPPED);
}

int main(int argc, char** argv)
{
	char mutexname[] = { "Global\\REVE_ATCEngine" };

	HANDLE hMutex = OpenMutexA(
		MUTEX_ALL_ACCESS, 0, mutexname);

	if (!hMutex)
		// Mutex doesn’t exist. This is
		// the first instance so create
		// the mutex.
		hMutex = CreateMutexA(0, 0, mutexname);
	else
		// The mutex exists so this is the
		// the second instance so return.
		return 0;
#ifdef _DEBUG
	mains();
#else
	
	SERVICE_TABLE_ENTRY serviceTable[] = {
		{ const_cast<LPTSTR>(TEXT("")), &ServiceMain },
		{ NULL, NULL }
	};

	if (StartServiceCtrlDispatcher(serviceTable))
		return 0;
	else if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
		return -1;
	else
		return -2;
#endif

}
