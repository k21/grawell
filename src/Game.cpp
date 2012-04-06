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
	ships.push_back(Ship(Point(100,100)));
	planets.push_back(Planet(Point(400,300),20,30000000));
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
					case Key::Left:  ships[controlled].dir -= 200; break;
					case Key::Right: ships[controlled].dir += 200; break;
					case Key::Space: bullets.push_back(ships[controlled].shoot()); break;
					default: break;
				}
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
