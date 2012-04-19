#include <cstring>

#include <boost/cstdint.hpp>

#include "Game.h"
#include "Log.h"
#include "Server.h"

using namespace boost;

static const char LOCALHOST_ADDRESS[] = "localhost";
static const uint16_t DEFAULT_PORT = 4920;

static const char USAGE_STRING[] =
"Usage:\n"
"    %s join [address [port]]\n"
"    %s host [port]\n"
"    %s dedicated [port]\n"
;

static void printUsage(const char *name) {
	fprintf(stderr, USAGE_STRING, name, name, name);
}

int main(int argc, char **argv) {

	Log::setMaxLogLevel(Log::DEBUG);
	LOG(TRACE) << "Entering main";

	if (argc < 2) {
		printUsage(argv[0]); return 1;
	}

	enum {JOIN, HOST, DEDICATED} mode;
	static const char joinStr[] = "join";
	static const char hostStr[] = "host";
	static const char dedicatedStr[] = "dedicated";
	if (strncmp(argv[1], joinStr, sizeof joinStr) == 0) {
		mode = JOIN;
	} else if (strncmp(argv[1], hostStr, sizeof hostStr) == 0) {
		mode = HOST;
	} else if (strncmp(argv[1], dedicatedStr, sizeof dedicatedStr) == 0) {
		mode = DEDICATED;
	} else {
		printUsage(argv[0]); return 1;
	}
	const char *joinAddress = LOCALHOST_ADDRESS;
	uint16_t joinPort = DEFAULT_PORT, hostPort = DEFAULT_PORT;
	if (mode == JOIN) {
		if (argc > 4) {
			printUsage(argv[0]); return 1;
		}
		if (argc >= 3) {
			joinAddress = argv[2];
		}
		if (argc >= 4) {
			char *end;
			long p = strtol(argv[3], &end, 10);
			if (*end != '\0' || p < 0 || p >= 65536) {
				printUsage(argv[0]); return 1;
			}
			joinPort = (uint16_t)p;
		}
	} else if (mode == HOST || mode == DEDICATED) {
		if (argc > 3) {
			printUsage(argv[0]); return 1;
		}
		if (argc == 3) {
			char *end;
			long p = strtol(argv[3], &end, 10);
			if (*end != '\0' || p < 0 || p >= 65536) {
				printUsage(argv[0]); return 1;
			}
			hostPort = (uint16_t)p;
			joinPort = (uint16_t)p;
		}
	}

	Server *server = 0;

	if (mode == HOST || mode == DEDICATED) {
		server = new Server(hostPort);
		if (mode == HOST) {
			server->Launch();
		} else {
			server->Run();
		}
	}

	if (mode == JOIN || mode == HOST) {
		Game game(joinAddress, joinPort);
		game.run();
	}

	if (server) {
		server->exit();
		delete server;
	}

	LOG(TRACE) << "Leaving main";
	return 0;

}
