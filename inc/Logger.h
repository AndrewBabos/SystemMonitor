#include <iostream>
#include <fstream>

enum LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR,
};

// experimenting
class Logger
{
private:
	LogLevel currentLevel;
	std::fstream outputFile;

private:
	std::string logLevelToString(LogLevel logLevel);
	std::string timestampString();
public:
	Logger();
	void Log(LogLevel logLevel);
	void Debug(LogLevel logLevel, std::string logMessage);
	void Info(LogLevel logLevel, std::string logMessage);
	void Warning(LogLevel logLevel, std::string logMessage);
	void Error(LogLevel logLevel, std::string logMessage);
	~Logger();
};