#include <SFML/Audio.hpp>

#include "Game.h"

using namespace std;
using namespace sf;

Game::Game() {
	VideoMode mode(800, 600);
	unsigned long style = Style::Close;
	WindowSettings settings(24, 8, 8);
	screen = new RenderWindow(mode, "GraWell", style, settings);
	controlled = 0;
}

Game::~Game() {
	delete screen;
}

void Game::run() {
	const double dt = 1.0/1024;
	double lastUpdate = clock.GetElapsedTime();
	while (screen->IsOpened()) {
		double now = clock.GetElapsedTime();
		input();
		while (lastUpdate < now) {
			logic(dt);
			lastUpdate += dt;
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
				switch (event.Key.Code) {
					case Key::Escape: screen->Close(); break;
					case Key::Left:
						universe.ships[controlled].dir -= 200;
						break;
					case Key::Right:
						universe.ships[controlled].dir += 200;
						break;
					case Key::Space:
						universe.bullets.push_back(universe.ships[controlled].shoot());
						break;
					default: break;
				}
				break;

			default:
				break;
		}
	}
}

void Game::logic(double dt) {
	universe.update(dt);
}

void Game::display() {
	screen->Clear();
	for (size_t i  = 0; i < universe.planets.size(); ++i) {
		universe.planets[i].draw(*screen);
	}
	for (size_t i  = 0; i < universe.ships.size(); ++i) {
		universe.ships[i].draw(*screen);
	}
	list<Bullet>::iterator it = universe.bullets.begin();
	while (it != universe.bullets.end()) {
		it->draw(*screen); ++it;
	}
	screen->Display();
}
