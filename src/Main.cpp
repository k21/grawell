#include "Game.h"
#include "Log.h"

int main() {

	Log::setMaxLogLevel(Log::DEBUG);
	LOG(TRACE) << "Entering main";

	Game game;
	game.run();

	LOG(TRACE) << "Leaving main";
	return 0;

}
