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
		font(),
		windowWidth(0), windowHeight(0),
		universe(), id(0), client(0),
		state(NOTHING), roundCntr(0),
		moveDown(0), moveRight(0), zoom(0),
		moveDownDelta(0), moveRightDelta(0), zoomDelta(0),
		keepPlanets(), keepBullets(), trails(),
		playerName() {
	strncpy(playerName, playerName_, sizeof playerName);
	playerName[(sizeof playerName) - 1] = '\0';
	windowWidth  = driver.getRenderWindow().getSize().x;
	windowHeight = driver.getRenderWindow().getSize().y;
	view.setCenter(0, 0);
	view.setSize(
			(float)FIXED_ONE * (float)windowWidth,
			(float)FIXED_ONE * (float)windowHeight);
	hudView.setCenter(0, 0);
	if (!font.loadFromFile("fonts/OpenSans-Regular.ttf")) {
		LOG(ERR) << "Font cannot be loaded.";
	}
	client = new Client(IpAddress(serverAddress), (short)port);
	client->launch();
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

static int32_t rotateAmount(bool control, bool shift) {
	if (control && shift) return 5;
	if (control) return 25;
	if (shift) return 1000;
	return 200;
}

static int32_t powerChangeAmount(bool control, bool shift) {
	if (control && shift) return 25;
	if (control) return 200;
	if (shift) return 5000;
	return 1000;
}

void GameScreen::handleKey(Event::KeyEvent e, bool pressed) {
	switch (e.code) {
		case Keyboard::Key::Escape: driver.exit(); break;
		case Keyboard::Key::Up:
			if (pressed && state == SELECT_ACTION) {
				int32_t strength = universe.ships[id].strength;
				strength += powerChangeAmount(e.control, e.shift);
				if (strength > MAX_STRENGTH) {
					strength = MAX_STRENGTH;
				}
				universe.ships[id].strength = strength;
			}
			break;
		case Keyboard::Key::Down:
			if (pressed && state == SELECT_ACTION) {
				int32_t strength = universe.ships[id].strength;
				strength -= powerChangeAmount(e.control, e.shift);
				if (strength < 0) {
					strength = 0;
				}
				universe.ships[id].strength = strength;
			}
			break;
		case Keyboard::Key::Left:
			if (pressed && state == SELECT_ACTION) {
				int32_t r = rotateAmount(e.control, e.shift);
				universe.ships[id].rotate(-r);
			}
			break;
		case Keyboard::Key::Right:
			if (pressed && state == SELECT_ACTION) {
				int32_t r = rotateAmount(e.control, e.shift);
				universe.ships[id].rotate(+r);
			}
			break;
		case Keyboard::Key::Space:
			if (pressed && state == SELECT_ACTION) {
				shoot();
			}
			break;
		case Keyboard::Key::W: moveDownDelta  = (int8_t)-pressed; break;
		case Keyboard::Key::S: moveDownDelta  = (int8_t) pressed; break;
		case Keyboard::Key::A: moveRightDelta = (int8_t)-pressed; break;
		case Keyboard::Key::D: moveRightDelta = (int8_t) pressed; break;
		case Keyboard::Key::Q: zoomDelta = (int8_t)-pressed; break;
		case Keyboard::Key::E: zoomDelta = (int8_t) pressed; break;
		default: break;
	}
}

void GameScreen::handleMessage(const Message &m) {
	LOG(DEBUG) << "Client received message "
			<< Message::typeToString((Message::Type)m.type());
	switch (m.type()) {
		case Message::JOIN_RESPONSE:
			if (!m.accepted()) {
				LOG(ERR) << "Server refused connection";
				driver.exit();
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
		list<uint16_t> hits;
		while (elapsed > 0) {
			universe.update(hits, true, &trails);
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
	if (client->error()) driver.exit();
}

void GameScreen::drawHud(RenderWindow &window) {
	window.setView(hudView);
	float sizeX = (float)driver.getRenderWindow().getSize().x;
	float sizeY = (float)driver.getRenderWindow().getSize().y;
	hudView.setSize(sizeX, sizeY);
	Ship &s = universe.ships[id];
	char info[256] = {0};
	if (state == SELECT_ACTION) {
		sprintf(info, "Power: %03ld.%02ld    Direction: %03ld.%02ld",
				(long)(s.strength / 100), (long)(s.strength % 100),
				(long)(s.direction / 100), (long)(s.direction % 100));
	}
	if (state == ROUND) {
		sprintf(info, "Round timer: %02ld", (long)(roundCntr/100));
	}
	if (state == SELECT_DONE || state == ROUND_DONE) {
		strcpy(info, "Waiting for other players...");
	}
	Text text;
	text.setFont(font);
	text.setString(info);
	text.setCharacterSize(20);
	text.setFillColor(Color::White);
	text.setOrigin(text.getLocalBounds().width/2, 0);
	text.setPosition(0, -sizeY/2);
	window.draw(text);
}

void GameScreen::display() {
	RenderWindow &window = driver.getRenderWindow();
	window.clear();

	if (state == NOTHING || state == REQUEST_SENT || state == WAITING) {
		window.setView(hudView);

		Text text;
		text.setFont(font);
		text.setString(
				client->isConnected()
					? "Waiting for next round..."
					: "Connecting...");
		text.setFillColor(Color::White);
		text.setOrigin(text.getLocalBounds().width/2, text.getLocalBounds().height/2);
		text.setPosition(0, 0);
		window.draw(text);
		window.display();
		return;
	}

	window.setView(view);

	if (windowWidth != window.getSize().x || windowHeight != window.getSize().y) {
		view.zoom((float)windowWidth / (float)window.getSize().x);
		windowWidth  = window.getSize().x;
		windowHeight = window.getSize().y;
		view.setSize(
				view.getSize().x,
				view.getSize().x * (float)windowHeight / (float)windowWidth);
	}

	float moveModifier = (view.getSize().x + view.getSize().y) / 400;
	moveRight += moveRightDelta * moveModifier;
	moveDown  += moveDownDelta  * moveModifier;
	zoom      += 4*(float)zoomDelta;
	moveRight *= 0.85f; moveDown *= 0.85f; zoom *= 0.85f;

	view.move((float)moveRight, (float)moveDown);
	float zoomLevel = view.getSize().x / (float)windowWidth / 2.0f;
	if ((zoomLevel > 50*FIXED_ONE && zoom < 0) ||
			(zoomLevel < FIXED_ONE/16 && zoom > 0)) {
		zoom = 0;
	} else {
		view.zoom((float)exp(zoom/250));
	}

	for (Trail &t : trails) {
		t.draw(window);
	}
	for (Bullet &b : universe.bullets) {
		if (b.active()) b.trail.draw(window);
	}
	for (Bullet &b : universe.bullets) {
		if (b.active()) b.draw(window);
	}
	for (Planet &p : universe.planets) {
		if (p.active()) p.draw(window);
	}
	for (Ship &s : universe.ships) {
		if (s.alive) s.draw(window, zoomLevel);
	}
	drawHud(window);
	window.display();
}
