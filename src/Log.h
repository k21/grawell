#ifndef LOG_H_
#define LOG_H_

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#define LOG(level) \
	if (Log::level <= Log::getMaxLogLevel()) \
		Log(Log::level, __FILE__, __LINE__)

class Log {

public:
	enum Level {FATAL, ERR, WARN, INFO, DEBUG, TRACE};

	inline Log(Level level_, const char *file, int line):
			os(), level(level_) {
		time_t seconds = time(0);
		tm *now = localtime(&seconds);
		os << " * " << std::setfill('0')
				<< 1900+now->tm_year << '-'
				<< std::setw(2) << now->tm_mon << '-'
				<< std::setw(2) << now->tm_mday << ' '
				<< std::setw(2) << now->tm_hour << ':'
				<< std::setw(2) << now->tm_min << ':'
				<< std::setw(2) << now->tm_sec
				<< '\t'
				<< levelString[level]
				<< '\t'
				<< file << ':'
				<< line
				<< '\t';
	}
	Log(const Log &) = delete;
	Log &operator = (const Log &) = delete;
	inline ~Log() {
		os << '\n';
		if (level <= maxLogLevel) {
			fputs(os.str().c_str(), stderr);
			fflush(stderr);
		}
	}

	template <typename T>
	inline Log &operator << (const T &t) {
		os << t;
		return (*this);
	}

	static void setMaxLogLevel(Level level) { maxLogLevel = level; }
	static inline Level getMaxLogLevel() { return maxLogLevel; }

private:
	std::ostringstream os;
	Level level;

	static Level maxLogLevel;
	static char levelString[6][10];

};

#endif
