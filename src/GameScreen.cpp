#include <cmath>
#include <cstring>
#include <utility>

#include "Client.h"
#include "Driver.h"
#include "Log.h"
#include "Server.h"

#include "GameScreen.h"

using namespace std;
using namespace sf;

GameScreen::GameScreen(Driver &driver_,
		const char *serverAddress, uint16_t port,
		const char *playerName_):
		Screen(driver_),
		view(), hudView(),
		windowWidth(0), windowHeight(0),
		universe(), id(0), client(0),
		state(NOTHING), roundCntr(0),
		moveDown(0), moveRight(0), zoom(0),
		moveDownDelta(0), moveRightDelta(0), zoomDelta(0),
		keepPlanets(), keepBullets(), trails(),
		playerName() {
	strncpy(playerName, playerName_, sizeof playerName);
	playerName[(sizeof playerName) - 1] = '\0';
	windowWidth  = driver.getRenderWindow().GetWidth();
	windowHeight = driver.getRenderWindow().GetHeight();
	view.SetCenter(0, 0);
	view.SetHalfSize(
			(float)FIXED_ONE * (float)windowWidth  / 2,
			(float)FIXED_ONE * (float)windowHeight / 2);
	hudView.SetCenter(0, 0);
	client = new Client(IPAddress(serverAddress), (short)port);
	client->Launch();
}

GameScreen::~GameScreen() {
	if (client) {
		client->exit();
		delete client;
	}
}

void GameScreen::shoot() {
	state = SELECT_DONE;
	Message m = Message::action(
			universe.ships[id].direction,
			universe.ships[id].strength
			);
	client->send(m);
}

static int32_t changeAmount(bool control, bool shift) {
	if (control && shift) return 5;
	if (control) return 25;
	if (shift) return 1000;
	return 200;
}

void GameScreen::handleKey(Event::KeyEvent e, bool pressed) {
	switch (e.Code) {
		case Key::Escape: driver.exit(); break;
		case Key::Up:
			if (pressed && state == SELECT_ACTION) {
				int32_t strength = universe.ships[id].strength;
				strength += 5 * changeAmount(e.Control, e.Shift);
				if (strength > MAX_STRENGTH) {
					strength = MAX_STRENGTH;
				}
				universe.ships[id].strength = strength;
			}
			break;
		case Key::Down:
			if (pressed && state == SELECT_ACTION) {
				int32_t strength = universe.ships[id].strength;
				strength -= 5 * changeAmount(e.Control, e.Shift);
				if (strength < 0) {
					strength = 0;
				}
				universe.ships[id].strength = strength;
			}
			break;
		case Key::Left:
			if (pressed && state == SELECT_ACTION) {
				int32_t r = changeAmount(e.Control, e.Shift);
				universe.ships[id].rotate(-r);
			}
			break;
		case Key::Right:
			if (pressed && state == SELECT_ACTION) {
				int32_t r = changeAmount(e.Control, e.Shift);
				universe.ships[id].rotate(+r);
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

void GameScreen::handleMessage(const Message &m) {
	LOG(DEBUG) << "Client received packet type " << (uint16_t)m.type();
	switch (m.type()) {
		case Message::JOIN_RESPONSE:
			if (!m.accepted()) {
				//TODO: error message
				LOG(ERR) << "Server refused connection";
				return;
			}
			id = m.id();
			universe.ships[id].name = playerName;
			universe.ships[id].inGame = true;
			universe.ships[id].ready = true;
			state = WAITING;
			break;
		case Message::GAME_SETTINGS:
			for (Ship &s : universe.ships) {
				s.inGame = false;
				s.alive = false;
			}
			keepPlanets.clear(); keepBullets.clear();
			break;
		case Message::SHIP_INFO:
			{
				Ship &s = universe.ships[m.id()];
				s.center.x = m.x();
				s.center.y = m.y();
				s.alive = true;
			}
			break;
		case Message::PLANET_INFO:
			{
				Planet &p = universe.planets[m.id()];
				p.center.x = m.x();
				p.center.y = m.y();
				p.radius = m.size();
				p.mass = m.mass();
				keepPlanets.insert(m.id());
			}
			break;
		case Message::BULLET_INFO:
			{
				Bullet &b = universe.bullets[m.id()];
				b.owner(m.owner());
				b.center.x = m.x(); b.center.y = m.y();
				b.speed.x = m.speedX(); b.speed.y = m.speedY();
				keepBullets.insert(m.id());
			}
			break;
		case Message::PLAYER_INFO:
			if (m.state() == Message::CONNECTED) {
				Ship &s = universe.ships[m.id()];
				s.name = m.text;
				s.inGame = true;
			} else {
				//TODO
			}
			break;
		case Message::SCORE_INFO:
			{
				Ship &s = universe.ships[m.id()];
				s.score = m.score();
			}
			break;
		case Message::NEW_ROUND:
			for (Planet &p : universe.planets) {
				if (p.active() && keepPlanets.find(p.id()) == keepPlanets.end()) {
					universe.planets.free(p);
				}
			}
			for (Bullet &b : universe.bullets) {
				if (b.active() && keepBullets.find(b.id()) == keepBullets.end()) {
					universe.bullets.free(b);
				}
			}
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
				universe.ships[m.id()].shoot(universe.bullets);
			}
			break;
	}
}

void GameScreen::logic(float elapsed) {
	{
		Message m;
		while (client->recv(m)) {
			handleMessage(m);
		}
	}
	if (state == NOTHING) {
		Message m = Message::joinRequest(PROTOCOL_VERSION, playerName);
		client->send(m);
		state = REQUEST_SENT;
	}
	if (state == ROUND) {
		list<pair<uint16_t, uint16_t>> hits;
		while (elapsed > 0) {
			universe.update(hits, true, &trails);
			for (pair<uint16_t, uint16_t> p : hits) {
				if (p.first != Message::NO_PLAYER) {
					if (p.first == p.second) --universe.ships[p.first].score;
					else ++universe.ships[p.first].score;
				}
				universe.ships[p.second].alive = false;
			}
			hits.clear();
			elapsed -= 1.0f/1024;
			--roundCntr;
			if (roundCntr == 0 || universe.bullets.empty()) {
				state = ROUND_DONE;
				Message m = Message::roundChecksum(0,0);
				client->send(m);
				break;
			}
		}
	}
}

void GameScreen::drawHud(RenderWindow &window) {
	window.SetView(hudView);
	float sizeX = (float)driver.getRenderWindow().GetWidth();
	float sizeY = (float)driver.getRenderWindow().GetHeight();
	hudView.SetHalfSize(sizeX / 2, sizeY / 2);
	Ship &s = universe.ships[id];
	char shipInfo[256];
	sprintf(shipInfo, "Power: %03d.%02d   Direction: %03d.%02d",
			s.strength / 100, s.strength % 100,
			s.direction / 100, s.direction % 100);
	String str(shipInfo, Font::GetDefaultFont(), 20);
	str.SetColor(Color::White);
	str.SetCenter(str.GetRect().GetWidth()/2, 0);
	str.SetPosition(0, -sizeY/2);
	window.Draw(str);
}

void GameScreen::display() {
	RenderWindow &window = driver.getRenderWindow();
	window.Clear();

	if (state == NOTHING || state == REQUEST_SENT || state == WAITING) {
		window.SetView(hudView);

		String s(
				state == client->isConnected()
					? "Waiting for next round..."
					: "Connecting...",
				Font::GetDefaultFont(), 60);
		s.SetColor(Color::White);
		s.SetCenter(s.GetRect().GetWidth()/2, s.GetRect().GetHeight()/2);
		s.SetPosition(0, 0);
		window.Draw(s);
		window.Display();
		return;
	}

	window.SetView(view);

	if (windowWidth != window.GetWidth() || windowHeight != window.GetHeight()) {
		view.Zoom((float)windowWidth / (float)window.GetWidth());
		windowWidth  = window.GetWidth();
		windowHeight = window.GetHeight();
		view.SetHalfSize(
				view.GetHalfSize().x,
				view.GetHalfSize().x * (float)windowHeight / (float)windowWidth);
	}

	moveRight += moveRightDelta*(double)view.GetHalfSize().x/100;
	moveDown  += moveDownDelta *(double)view.GetHalfSize().y/100;
	zoom      += 4*zoomDelta;
	moveRight *= 0.85; moveDown *= 0.85; zoom *= 0.85;
	view.Move((float)moveRight, (float)moveDown);
	view.Zoom((float)exp(zoom/250));

	for (Trail &t : trails) {
		t.draw(window);
	}
	for (Planet &p : universe.planets) {
		if (p.active()) p.draw(window);
	}
	for (Bullet &b : universe.bullets) {
		if (b.active()) b.draw(window);
	}
	for (Ship &s : universe.ships) {
		if (s.alive) s.draw(window);
	}
	drawHud(window);
	window.Display();
}
