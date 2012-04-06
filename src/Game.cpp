#include <SFML/Audio.hpp>

#include "Game.h"

using namespace std;
using namespace sf;

Game::Game() {
	VideoMode mode(800, 600);
	unsigned long style = Style::Close;
	WindowSettings settings(24, 8, 8);
	screen = new RenderWindow(mode, "GraWell", style, settings);
	planets.push_back(Planet(Point(400,300),40,3000));
	bullets.push_back(Bullet(Point(400,550), Vector(3,0)));
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
				if (event.Key.Code == sf::Key::Escape) screen->Close();
				break;

			default:
				break;
		}
	}
}

void Game::logic(double dt) {
	list<Bullet>::iterator it = bullets.begin();
	while (it != bullets.end()) {
		if (it->update(planets, dt)) {
			list<Bullet>::iterator it2 = it; ++it2;
			bullets.erase(it);
			it = it2;
		} else ++it;
	}
}

void Game::display() {
	screen->Clear();
	for (size_t i  = 0; i < planets.size(); ++i) planets[i].draw(*screen);
	for (size_t i  = 0; i < ships  .size(); ++i) ships  [i].draw(*screen);
	list<Bullet>::iterator it = bullets.begin();
	while (it != bullets.end()) {
		it->draw(*screen); ++it;
	}
	screen->Display();
}
