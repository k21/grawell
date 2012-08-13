#include "Driver.h"

using namespace sf;

Driver::Driver(unsigned int resX, unsigned int resY, bool fullscreen):
		renderWindow(0), screen(0),
		toDelete(0), doExit(false), clock() {
	VideoMode mode(resX, resY);
	WindowSettings settings(24, 0, 2);
	renderWindow = new RenderWindow(mode, "GraWell",
			Style::Close | (fullscreen ? Style::Fullscreen : Style::Resize),
			settings);
}

Driver::~Driver() {
	delete toDelete;
	toDelete = 0;
	delete screen;
	screen = 0;
	delete renderWindow;
	renderWindow = 0;
}

void Driver::run() {
	static const float FPS = 30.f;
	static const float SPF = 1.0f/FPS;
	float elapsed = 0.f;
	while (renderWindow->IsOpened()) {
		clock.Reset();
		screen->input();
		if (doExit) break;
		if (toDelete) {
			delete toDelete;
			toDelete = 0;
			continue;
		}
		screen->logic(elapsed);
		if (doExit) break;
		if (toDelete) {
			delete toDelete;
			toDelete = 0;
			continue;
		}
		screen->display();
		elapsed = clock.GetElapsedTime();
		if (elapsed < SPF) {
			Sleep(SPF-elapsed);
			elapsed = SPF;
		}
	}
}

void Driver::changeScreen(Screen *s) {
	if (toDelete) {
		delete screen;
	} else {
		toDelete = screen;
	}
	screen = s;
}
