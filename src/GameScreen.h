#ifndef GAME_SCREEN_H_
#define GAME_SCREEN_H_

#include <cstdint>
#include <list>
#include <set>

#include "Screen.h"
#include "Trail.h"
#include "Universe.h"

class Client;
class Message;

class GameScreen : public Screen {

public:
	GameScreen(Driver &driver_, const char *serverAddress, uint16_t port,
			const char *playerName_);
	GameScreen(const GameScreen &) = delete;
	const GameScreen &operator = (const GameScreen &) = delete;
	~GameScreen();

protected:
	void logic(float elapsed);
	void display();

	void handleKey(sf::Event::KeyEvent e, bool pressed);

	void handleMessage(const Message &m);
	void shoot();
	void drawHud(sf::RenderWindow &window);

	sf::View view, hudView;
	sf::Font font;
	uint32_t windowWidth, windowHeight;
	Universe universe;
	uint16_t id;
	Client *client;
	enum {
		NOTHING, REQUEST_SENT, WAITING,
		SELECT_ACTION, SELECT_DONE,
		ROUND, ROUND_DONE
	} state;
	int32_t roundCntr;
	float moveDown, moveRight, zoom;
	int8_t moveDownDelta, moveRightDelta, zoomDelta;
	std::set<uint16_t> keepPlanets, keepBullets;
	std::list<Trail> trails;
	char playerName[32 + 1];

};

#endif
