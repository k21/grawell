#include "Driver.h"

#include "Screen.h"

using namespace sf;

void Screen::input() {
	RenderWindow &window = driver.getRenderWindow();
	Event event;
	while (window.GetEvent(event)) {
		switch (event.Type) {

			case Event::Closed:
				driver.exit();
				break;

			case Event::KeyPressed:
				handleKey(event.Key, true);
				break;

			case Event::KeyReleased:
				handleKey(event.Key, false);
				break;

			default:
				break;
		}
	}
}
