#include "Driver.h"

#include "Screen.h"

using namespace sf;

void Screen::input() {
	RenderWindow &window = driver.getRenderWindow();
	Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {

			case Event::Closed:
				driver.exit();
				break;

			case Event::KeyPressed:
				handleKey(event.key, true);
				break;

			case Event::KeyReleased:
				handleKey(event.key, false);
				break;

			default:
				break;
		}
	}
}
