#pragma once
#include <Windows.h>
#include <iostream>
#include <ostream>
#include <unordered_map>

using namespace std;

enum Level
{
	TRACE = 6,
	DEBUG = 5,
	INFO = 4,
	WARN = 3,
	ERRORS = 2,
	FATAL = 1
};


class Logger
{
private:

	//int getDatatype(Data data);
	bool isDigit(char value);

public:
	Logger();
	~Logger();


	static int level;
	static bool isFileLoggingEnabled;
	static bool isConsoleLoggingEnabled;
	static std::ofstream* m_file;
	static CRITICAL_SECTION logLocker;


	void initLogger(std::string logFile, int log_dest);

	void setLevel(int level);
	void enableFileLogging();
	void enableConsoleLogging();
	void disableFileLogging();
	void disableConsoleFileLogging();

	static void logOnConsole(std::string& data);
	static void logIntoFile(std::string& data);
	static std::string getCurrentTime();

	static void trace(const char* log, ...);
	static void debug(const char* log, ...);
	static void info(const char* log, ...);
	static void warn(const char* log, ...);
	static void error(const char* log, ...);
	static void fatal(const char* log, ...);                                          // level 1

};

