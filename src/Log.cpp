#include "Log.h"

Log::Level Log::maxLogLevel = Log::INFO;
char Log::levelString[6][10]
		= {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
