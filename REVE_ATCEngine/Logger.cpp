#define _CRT_SECURE_NO_WARNINGS

#include "Logger.h"
#include <stdio.h>   
#include <stdlib.h>
#include <string>
#include <time.h>
#include <exception>
#include <fstream>

int Logger::level;
bool Logger::isFileLoggingEnabled;
bool Logger::isConsoleLoggingEnabled;
std::ofstream* Logger::m_file;


Logger::Logger()
{
	m_file = NULL;
}

Logger::~Logger()
{
}

//int Logger::getDatatype(Data data)
//{
//	int i = 0;
//
//	if (data.length == 8)               // Can Be LONGLONG or String Array
//	{
//		if (data.buffer[0] == '\0')
//			return 3;
//	}
//	else if (data.length == 4)        // Can Be LONG or String Array
//	{
//		if (data.buffer[0] == '\0')
//			return 3;
//	}
//	else if (data.length == 2)		// Can Be Int or String Array
//	{
//		if (data.buffer[0] == '\0')
//			return 1;
//	}
//	else                         // Is String Array
//	{
//		return 4;
//	}
//
//	return 4;
//}

bool Logger::isDigit(char value)
{
	if (value >= 30 && value <= 39 || value == '\0')
		return TRUE;
	else
		return FALSE;
}

void Logger::initLogger(std::string logFile, int log_dest)
{
	if (m_file == NULL)
	{
		m_file = new ofstream(logFile, std::ofstream::out | fstream::app);
	}

	if (log_dest & 1)
		isConsoleLoggingEnabled = true;
	else
		isConsoleLoggingEnabled = false;

	if (log_dest & 2)
		isFileLoggingEnabled = true;
	else
		isFileLoggingEnabled = false;
}

void Logger::setLevel(int level)
{
	this->level = level;
}

void Logger::enableFileLogging()
{
	isFileLoggingEnabled = true;
}

void Logger::enableConsoleLogging()
{
	isConsoleLoggingEnabled = true;
}

void Logger::disableFileLogging()
{
	isFileLoggingEnabled = false;
}

void Logger::disableConsoleFileLogging()
{
	isConsoleLoggingEnabled = false;
}

string Logger::getCurrentTime()
{
	string currTime;
	//Current date/time based on current time
	time_t now = time(0);
	// Convert current time to string
	currTime.assign(ctime(&now));

	// Last charactor of currentTime is "\n", so remove it
	string currentTime = currTime.substr(0, currTime.size() - 1);
	return currentTime;
}

void Logger::logOnConsole(std::string& data)
{

	cout << getCurrentTime() << "  " << data << endl;
	//unlock()
}

void Logger::logIntoFile(std::string& data)
{
	//EnterCriticalSection(&logLocker);
	try
	{
		if (m_file != NULL)
			*m_file << getCurrentTime() << "  " << data << endl;
	}
	catch (...)
	{

	}
	//LeaveCriticalSection(&logLocker);
}

void Logger::trace(const char* log, ...)														// Level 6
{
	string data;
	va_list args;
	char logs[4096];

	if ((level >= TRACE) & (isConsoleLoggingEnabled | isFileLoggingEnabled))
	{
		va_start(args, log);
		vsnprintf(logs, sizeof(logs), log, args);

		data.append("[TRACE]: ");
		data.append(logs);

		if (isConsoleLoggingEnabled)
			logOnConsole(data);

		if (isFileLoggingEnabled)
			logIntoFile(data);
	}
}

void Logger::debug(const char* log, ...)
{
	string data;
	va_list args;
	char logs[4096];

	if ((level >= DEBUG) & (isConsoleLoggingEnabled | isFileLoggingEnabled))
	{
		va_start(args, log);
		vsnprintf(logs, sizeof(logs), log, args);

		data.append("[DEBUG]: ");
		data.append(logs);

		if (isConsoleLoggingEnabled)
			logOnConsole(data);

		if (isFileLoggingEnabled)
			logIntoFile(data);
	}
}

void Logger::info(const char* log, ...)
{
	string data;
	va_list args;
	char logs[4096];

	if ((level >= INFO) & (isConsoleLoggingEnabled | isFileLoggingEnabled))
	{
		va_start(args, log);
		vsnprintf(logs, sizeof(logs), log, args);

		data.append("[INFO]: ");
		data.append(logs);

		if (isConsoleLoggingEnabled)
			logOnConsole(data);

		if (isFileLoggingEnabled)
			logIntoFile(data);
	}
}

void Logger::warn(const char* log, ...)
{
	string data;
	va_list args;
	char logs[4096];

	if ((level >= WARN) & (isConsoleLoggingEnabled | isFileLoggingEnabled))
	{
		va_start(args, log);
		vsnprintf(logs, sizeof(logs), log, args);

		data.append("[WARN]: ");
		data.append(logs);

		if (isConsoleLoggingEnabled)
			logOnConsole(data);

		if (isFileLoggingEnabled)
			logIntoFile(data);
	}
}

void Logger::error(const char* log, ...)
{
	string data;
	va_list args;
	char logs[4096];

	if ((level >= ERRORS) & (isConsoleLoggingEnabled | isFileLoggingEnabled))
	{
		va_start(args, log);
		vsnprintf(logs, sizeof(logs), log, args);

		data.append("[WARN]: ");
		data.append(logs);

		if (isConsoleLoggingEnabled)
			logOnConsole(data);

		if (isFileLoggingEnabled)
			logIntoFile(data);
	}
}

void Logger::fatal(const char* log, ...)                                              //level 1
{

	string data;
	va_list args;
	char logs[4096];

	if ((level >= FATAL) & (isConsoleLoggingEnabled | isFileLoggingEnabled))
	{
		va_start(args, log);
		vsnprintf(logs, sizeof(logs), log, args);

		data.append("[FATAL]: ");
		data.append(logs);

		if (isConsoleLoggingEnabled)
			logOnConsole(data);

		if (isFileLoggingEnabled)
			logIntoFile(data);
	}
}





