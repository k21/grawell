#ifndef MENU_SCREEN_H_
#define MENU_SCREEN_H_

#include "Screen.h"

class Driver;

class MenuScreen : public Screen {

public:
	MenuScreen(Driver &driver_);

protected:
	void logic(float) {}
	void display();

	void handleKey(sf::Event::KeyEvent e, bool pressed);

};

#endif
