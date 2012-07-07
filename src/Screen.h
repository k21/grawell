#ifndef SCREEN_H_
#define SCREEN_H_

#include <SFML/Graphics.hpp>

class Driver;

class Screen {

public:
	Screen(Driver &driver_): driver(driver_) {};
	Screen(const Screen &) = delete;
	const Screen &operator = (const Screen &) = delete;
	virtual ~Screen() {};

	virtual void input();
	virtual void logic(float elapsed) = 0;
	virtual void display() = 0;

protected:
	virtual void handleKey(sf::Event::KeyEvent, bool) {};

	Driver &driver;

};

#endif
