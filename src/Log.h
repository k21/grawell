#ifndef LOG_H_
#define LOG_H_

#include <cmath>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>

#include <SFML/System.hpp>

#define LOG(level) \
	if (Log::level <= Log::getMaxLogLevel()) \
		Log::Instance(Log::level, __FILE__, __LINE__)

class Log {

public:
	Log() = delete;

	enum Level {FATAL, ERR, WARN, INFO, DEBUG, TRACE};

	class Instance {

	public:
		Instance(const Instance &) = delete;
		Instance &operator = (const Instance &) = delete;

		inline Instance(Level level_, const char *file, int line):
				os(), level(level_) {
			float now = clock.getElapsedTime().asSeconds();
			os << '['
					<< (long)now << '.'
					<< std::setw(4) << std::setfill('0')
					<< (long)(fmodf(now, 1.f)*10000)
					<< "]\t"
					<< levelString[level]
					<< '\t'
					<< file << ':' << line
					<< '\t';
		}

		inline ~Instance() {
			os << '\n';
			if (level <= maxLogLevel) {
				fputs(os.str().c_str(), stderr);
				fflush(stderr);
			}
		}

		template <typename T>
		inline Instance &operator << (const T &t) {
			os << t;
			return (*this);
		}

	private:
		std::ostringstream os;
		Level level;

	};

	static void setMaxLogLevel(Level level) { maxLogLevel = level; }
	static inline Level getMaxLogLevel() { return maxLogLevel; }

private:
	static Level maxLogLevel;
	static char levelString[6][10];
	static sf::Clock clock;

};

#endif
