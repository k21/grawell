#ifndef GAME_H_
#define GAME_H_

#include <set>

#include <boost/cstdint.hpp>

#include <SFML/Graphics.hpp>

#include "Universe.h"
#include "Trail.h"

class Client;
class Message;

class Game {

public:
	Game(const char *serverAddress, boost::uint16_t port);
	Game(const Game &) = delete;
	Game &operator = (const Game &) = delete;
	~Game();

	void run();

private:
	void input();
	void logic();
	void display();
	void handleMessage(const Message &m);
	void handleKey(sf::Event::KeyEvent code, bool down);
	void shoot();

	sf::RenderWindow *screen;
	sf::View view;
	sf::Clock clock;
	Universe universe;
	boost::uint16_t id;
	Client *client;
	enum {
		NOTHING, REQUEST_SENT, WAITING,
		SELECT_ACTION, SELECT_DONE,
		ROUND, ROUND_DONE
	} state;
	boost::int32_t roundCntr;
	double pendingUpdates;
	double moveDown, moveRight, zoom;
	boost::int8_t moveDownDelta, moveRightDelta, zoomDelta;
	std::set<uint16_t> keepPlanets, keepBullets;
	std::vector<Trail> trails;

};

#endif
