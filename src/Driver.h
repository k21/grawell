#ifndef DRIVER_H_
#define DRIVER_H_

#include <SFML/Graphics.hpp>

#include "Screen.h"

class Driver {

public:
	Driver();
	Driver(const Driver &) = delete;
	Driver &operator = (const Driver &) = delete;
	~Driver();

	void run();

	void changeScreen(Screen *s);
	void exit() { doExit = true; }
	sf::RenderWindow &getRenderWindow() { return *renderWindow; }

private:
	sf::RenderWindow *renderWindow;
	Screen *screen, *toDelete;
	bool doExit;
	sf::Clock clock;

};

#endif
