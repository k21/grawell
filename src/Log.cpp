#include "Log.h"

Log::Level Log::maxLogLevel = Log::INFO;
char Log::levelString[6][10]
		= {"FATAL", "ERR", "WARN", "INFO", "DEBUG", "TRACE"};
