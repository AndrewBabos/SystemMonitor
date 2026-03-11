#include <Logger.h>


Logger::Logger()
{
	outputFile.open("Log.txt");
}

std::string Logger::logLevelToString(LogLevel logLevel)
{
	switch (logLevel)
	{
		case DEBUG:	  return "DEBUG";
		case INFO:    return "INFO";
		case WARNING: return "WARNING";
		case ERROR:	  return "ERROR";
	}
}

std::string Logger::timestampString()
{
	return std::string();
}

void Logger::Log(LogLevel logLevel)
{

}

void Logger::Debug(LogLevel level, std::string logMessage)
{
}

void Logger::Info(LogLevel level, std::string logMessage)
{
	//outputFile << level << ": " << logMessage;
}

void Logger::Warning(LogLevel level, std::string logMessage)
{
}

void Logger::Error(LogLevel level, std::string logMessage)
{
}

Logger::~Logger()
{
	outputFile.close();
}
