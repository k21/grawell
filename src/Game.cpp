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
	view.SetHalfSize(400.0*FIXED_ONE, 300.0*FIXED_ONE);
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
	static const double FPS = 30;
	static const double SPF = 1.0/FPS;
	lastUpdate = clock.GetElapsedTime();
	double frameStart = lastUpdate;
	while (screen->IsOpened()) {
		input();
		logic();
		display();
		double now = clock.GetElapsedTime();
		if (now-frameStart < SPF) {
			Sleep((float)(SPF-(now-frameStart)));
			frameStart = clock.GetElapsedTime();
		} else {
			frameStart = now;
		}
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
			universe.ships[id].active = true;
			universe.ships[id].ready = true;
			state = WAITING;
			break;
		case Message::GAME_SETTINGS:
			for (Ship &s : universe.ships) {
				s.active = false;
				s.alive = false;
			}
			universe.planets.clear();
			universe.bullets.clear();
			break;
		case Message::SHIP_INFO:
			{
				allocShips(m.id()+1);
				Ship &s = universe.ships[m.id()];
				s.center.x = m.x();
				s.center.y = m.y();
				s.alive = true;
			}
			break;
		case Message::PLANET_INFO:
			{
				while (universe.planets.size() < (size_t)(m.id()+1)) {
					universe.planets.push_back(Planet(Point(0,0), 0, 0));
				}
				Planet &p = universe.planets[m.id()];
				p.center.x = m.x();
				p.center.y = m.y();
				p.radius = m.size();
				p.mass = m.mass();
			}
			break;
		case Message::BULLET_INFO:
			universe.bullets.push_back(Bullet(m.id(),
					Point(m.x(), m.y()), Vector(m.speedX(), m.speedY())));
			break;
		case Message::PLAYER_INFO:
			if (m.state() == Message::CONNECTED) {
				allocShips(m.id()+1);
				Ship &s = universe.ships[m.id()];
				s.name = m.text;
				s.active = true;
			} else {
				//TODO
			}
			break;
		case Message::SCORE_INFO:
			//TODO
			break;
		case Message::NEW_ROUND:
			state = SELECT_ACTION;
			break;
		case Message::PLAYER_READY:
			universe.ships[m.id()].ready = true;
			break;
		case Message::ACTION_INFO:
			if (m.id() == Message::NO_PLAYER) {
				state = ROUND;
				roundCntr = 8192;
			} else {
				Ship &s = universe.ships[m.id()];
				s.direction = m.direction();
				s.strength = m.strength();
				universe.bullets.push_back(universe.ships[m.id()].shoot());
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
		double now = clock.GetElapsedTime();
		list<pair<uint16_t, uint16_t>> hits;
		while (lastUpdate < now) {
			universe.update(hits);
			for (pair<uint16_t, uint16_t> p : hits) {
				if (p.first != Message::NO_PLAYER) ++universe.ships[p.first].score;
				universe.ships[p.second].alive = false;
			}
			hits.clear();
			lastUpdate += 1.0/1024.0;
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
	moveRight += moveRightDelta*(double)view.GetHalfSize().x/100;
	moveDown  += moveDownDelta *(double)view.GetHalfSize().y/100;
	zoom      += 4*zoomDelta;
	moveRight *= 0.85; moveDown *= 0.85; zoom *= 0.85;
	view.Move((float)moveRight, (float)moveDown);
	view.Zoom((float)exp(zoom/250));
	screen->Clear();
	for (Planet &p : universe.planets) {
		p.draw(*screen);
	}
	for (Ship &s : universe.ships) {
		if (s.alive) s.draw(*screen);
	}
	for (Bullet &b : universe.bullets) {
		b.draw(*screen);
	}
	screen->Display();
}
