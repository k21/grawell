#include <SFML/Audio.hpp>

#include "Game.h"

using namespace sf;

Game::Game() {
	VideoMode mode(800, 600);
	unsigned long style = Style::Close;
	WindowSettings settings(24, 8, 8);
	screen = new RenderWindow(mode, "GraWell", style, settings);
	planets.push_back(Planet(Point(100,100),20,0));
}

Game::~Game() {
	delete screen;
}

void Game::run() {
	double lastUpdate = clock.GetElapsedTime();
	while (screen->IsOpened()) {
		double now = clock.GetElapsedTime();
		input();
		while (lastUpdate < now) {
			logic();
			lastUpdate += 0.01;
		}
		display();
	}
}

void Game::input() {
	Event event;
	while (screen->GetEvent(event)) {
		switch (event.Type) {

			case Event::Closed:
				screen->Close();
				break;

			case Event::KeyPressed:
				if (event.Key.Code == sf::Key::Escape) screen->Close();
				break;

			default:
				break;
		}
	}
}

void Game::logic() {
	planets[0].x(planets[0].x()+1);
}

void Game::display() {
	screen->Clear();
	for (size_t i  = 0; i < planets.size(); ++i) planets[i].draw(*screen);
	screen->Display();
}
