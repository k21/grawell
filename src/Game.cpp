#include <cmath>
#include <utility>

#include <SFML/Audio.hpp>

#include "Client.h"
#include "Log.h"
#include "Server.h"

#include "Game.h"

using namespace std;
using namespace boost;
using namespace sf;

Game::Game(const char *serverAddress, uint16_t port):
		screen(0), view(), clock(), universe(), id(0), client(0),
		state(NOTHING), roundCntr(0), lastUpdate(0),
		moveDown(0), moveRight(0), zoom(0),
		moveDownDelta(0), moveRightDelta(0), zoomDelta(0) {
	VideoMode mode(800, 600);
	WindowSettings settings(24, 8, 8);
	screen = new RenderWindow(mode, "GraWell", Style::Close, settings);
	view.SetCenter(0, 0);
	view.SetHalfSize(400, 300);
	screen->SetView(view);
	client = new Client(IPAddress(serverAddress), (short)port);
	client->Launch();
}

Game::~Game() {
	delete screen;
	if (client) {
		client->exit();
		delete client;
	}
}

void Game::run() {
	lastUpdate = clock.GetElapsedTime();
	while (screen->IsOpened()) {
		input();
		logic();
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

void Game::handleKey(Key::Code code, bool pressed) {
	switch (code) {
		case Key::Escape: screen->Close(); break;
		case Key::Left:
			if (pressed && state == SELECT_ACTION) {
				universe.ships[id].rotate(-200);
			}
			break;
		case Key::Right:
			if (pressed && state == SELECT_ACTION) {
				universe.ships[id].rotate(+200);
			}
			break;
		case Key::Space:
			if (pressed && state == SELECT_ACTION) {
				shoot();
			}
			break;
		case Key::W: moveDownDelta  = (int8_t)-pressed; break;
		case Key::S: moveDownDelta  = (int8_t) pressed; break;
		case Key::A: moveRightDelta = (int8_t)-pressed; break;
		case Key::D: moveRightDelta = (int8_t) pressed; break;
		case Key::Q: zoomDelta = (int8_t)-pressed; break;
		case Key::E: zoomDelta = (int8_t) pressed; break;
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
				handleKey(event.Key.Code, true);
				break;

			case Event::KeyReleased:
				handleKey(event.Key.Code, false);
				break;

			default:
				break;
		}
	}
}

void Game::allocShips(size_t n) {
	while (universe.ships.size() < n) {
		universe.ships.push_back(Ship((uint16_t)universe.ships.size()));
	}
}

void Game::handleMessage(const Message &m) {
	LOG(DEBUG) << "Client received packet type " << (uint16_t)m.type();
	switch (m.type()) {
		case Message::JOIN_RESPONSE:
			if (!m.accepted()) {
				//TODO: error message
				LOG(ERR) << "Server refused connection";
				return;
			}
			id = m.id();
			allocShips(id+1);
			universe.ships[id].name = "name";
			state = WAITING;
			break;
		case Message::GAME_SETTINGS:
			//TODO
			break;
		case Message::SHIP_INFO:
			allocShips(m.id()+1);
			universe.ships[m.id()].center.x = m.x()/1024.0;
			universe.ships[m.id()].center.y = m.y()/1024.0;
			universe.ships[m.id()].active = true;
			break;
		case Message::PLANET_INFO:
			while (universe.planets.size() < (size_t)(m.id()+1)) {
				universe.planets.push_back(Planet(Point(0,0), 0, 0));
			}
			universe.planets[m.id()].center.x = m.x()/1024.0;
			universe.planets[m.id()].center.y = m.y()/1024.0;
			universe.planets[m.id()].radius = m.size();
			universe.planets[m.id()].mass = m.mass();
			break;
		case Message::PLAYER_INFO:
			if (m.state() == Message::CONNECTED) {
				allocShips(m.id()+1);
				universe.ships[m.id()].name = m.text;
			} else {
				universe.ships[m.id()].deactivate = true;
			}
			break;
		case Message::SCORE_INFO:
			//TODO
			break;
		case Message::NEW_ROUND:
			//TODO
			for (Ship &s : universe.ships) {
				s.ready = false;
				if (s.deactivate) {
					s.active = false;
					s.deactivate = false;
				}
			}
			state = SELECT_ACTION;
			break;
		case Message::PLAYER_READY:
			//TODO
			break;
		case Message::ACTION_INFO:
			{
				//TODO
				universe.ships[m.id()].direction = m.direction();
				universe.ships[m.id()].strength = m.strength();
				universe.bullets.push_back(universe.ships[m.id()].shoot());
				universe.ships[m.id()].ready = true;
				bool allReady = true;
				for (Ship &s : universe.ships) {
					if (s.active
							&& !s.deactivate
							&& !s.ready) {
						allReady = false;
					}
				}
				if (allReady) { state = ROUND; roundCntr = 8192; }
			}
			break;
	}
}

void Game::logic() {
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
		static const double dt = 1.0/1024;
		double now = clock.GetElapsedTime();
		list<pair<uint16_t, uint16_t>> hits;
		while (lastUpdate < now) {
			universe.update(hits, dt);
			for (pair<uint16_t, uint16_t> p : hits) {
				++universe.ships[p.first].score;
				universe.ships[p.second].active = false;
			}
			hits.clear();
			lastUpdate += dt;
			--roundCntr;
			if (roundCntr == 0 || universe.bullets.empty()) {
				state = ROUND_DONE;
				Message m = Message::roundChecksum(0,0);
				client->send(m);
				break;
			}
		}
	}
	lastUpdate = clock.GetElapsedTime();
}

void Game::display() {
	moveRight += moveRightDelta*(double)view.GetHalfSize().x/200;
	moveDown  += moveDownDelta *(double)view.GetHalfSize().y/200;
	zoom      += 2*zoomDelta;
	moveRight *= 0.85; moveDown *= 0.85; zoom *= 0.85;
	view.Move((float)moveRight, (float)moveDown);
	view.Zoom((float)exp(zoom/250));
	screen->Clear();
	for (size_t i  = 0; i < universe.planets.size(); ++i) {
		universe.planets[i].draw(*screen);
	}
	for (size_t i  = 0; i < universe.ships.size(); ++i) {
		if (!universe.ships[i].active) continue;
		universe.ships[i].draw(*screen);
	}
	for (Bullet &b : universe.bullets) {
		b.draw(*screen);
	}
	screen->Display();
}
