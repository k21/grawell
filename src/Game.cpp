#include <SFML/Audio.hpp>

#include "Client.h"
#include "Server.h"

#include "Game.h"

using namespace std;
using namespace sf;

Game::Game():
		screen(0), clock(), universe(), id(0), server(0), client(0),
		state(NOTHING) {
	VideoMode mode(800, 600);
	unsigned long style = Style::Close;
	WindowSettings settings(24, 8, 8);
	screen = new RenderWindow(mode, "GraWell", style, settings);
	server = new Server(4920);
	server->Launch();
	client = new Client(IPAddress("localhost"), 4920);
	client->Launch();
}

Game::~Game() {
	delete screen;
	if (server) {
		server->exit();
		delete server;
	}
	if (client) {
		client->exit();
		delete client;
	}
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

void Game::shoot() {
	state = SELECT_DONE;
	Message m = Message::action(
			universe.ships[id].direction,
			universe.ships[id].strength
			);
	client->send(m);
}

void Game::keyPressed(Key::Code code) {
	switch (code) {
		case Key::Escape: screen->Close(); break;
		case Key::Left:
			if (state == SELECT_ACTION) {
				universe.ships[id].rotate(-200);
			}
			break;
		case Key::Right:
			if (state == SELECT_ACTION) {
				universe.ships[id].rotate(+200);
			}
			break;
		case Key::Space:
			if (state == SELECT_ACTION) {
				shoot();
			}
			break;
		default: break;
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
				keyPressed(event.Key.Code);
				break;

			default:
				break;
		}
	}
}

void Game::allocShips(size_t n) {
	while (universe.ships.size() < n) {
		universe.ships.push_back(Ship(Point(0,0)));
	}
}

void Game::handleMessage(const Message &m) {
	switch (m.type()) {
		case Message::JOIN_RESPONSE:
			if (!m.accepted()) {
				//TODO: error message
				return;
			}
			id = m.id();
			state = WAITING;
			break;
		case Message::GAME_SETTINGS:
			//TODO
			break;
		case Message::SHIP_INFO:
			allocShips(m.id()+1);
			universe.ships[m.id()].center.x = m.x()/1024.0;
			universe.ships[m.id()].center.y = m.y()/1024.0;
			break;
		case Message::PLANET_INFO:
			while (universe.planets.size() < (size_t)(m.id()+1)) {
				universe.planets.push_back(Planet(Point(0,0), 0, 0));
			}
			universe.planets[m.id()].center.x = m.x()/1024.0;
			universe.planets[m.id()].center.y = m.y()/1024.0;
			universe.planets[m.id()].radius = m.size()/1024.0;
			universe.planets[m.id()].mass = m.mass()/1024.0;
			break;
		case Message::PLAYER_INFO:
			allocShips(m.id()+1);
			universe.ships[m.id()].name = m.text;
			break;
		case Message::SCORE_INFO:
			//TODO
			break;
		case Message::NEW_ROUND:
			//TODO
			state = SELECT_ACTION;
			break;
		case Message::PLAYER_READY:
			//TODO
			break;
		case Message::ACTION_INFO:
			//TODO
			state = ROUND;
			break;
		case Message::CHECKSUM_MISMATCH:
			//TODO
			break;
		default:
			//TODO: error
			break;
	}
}

void Game::logic(double dt) {
	{
		Message m;
		while (client->recv(m)) {
			handleMessage(m);
		}
	}
	if (state == NOTHING) {
		Message m = Message::joinRequest(PROTOCOL_VERSION, "name");
		client->send(m);
		state = REQUEST_SENT;
	}
	if (state == ROUND) {
		universe.update(dt);
	}
}

void Game::display() {
	screen->Clear();
	for (size_t i  = 0; i < universe.planets.size(); ++i) {
		universe.planets[i].draw(*screen);
	}
	for (size_t i  = 0; i < universe.ships.size(); ++i) {
		universe.ships[i].draw(*screen);
	}
	for (Bullet &b : universe.bullets) {
		b.draw(*screen);
	}
	screen->Display();
}
