#include "Driver.h"
#include "GameScreen.h"

#include "MenuScreen.h"

using namespace sf;

MenuScreen::MenuScreen(Driver &driver_): Screen(driver_) {}

void MenuScreen::display() {
	RenderWindow &window = driver.getRenderWindow();
	window.Clear();
	window.Display();
}

void MenuScreen::handleKey(sf::Event::KeyEvent e, bool pressed) {
	if (!pressed) return;
	switch (e.Code) {
		case Key::Escape:
			driver.exit();
			break;
		case Key::Return:
			driver.changeScreen(new GameScreen(driver, "127.0.0.1", 4920, "name"));
			break;
		default:
			break;
	}
}
