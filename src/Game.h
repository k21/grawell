#ifndef GAME_H_
#define GAME_H_

#include <SFML/Graphics.hpp>

#include "Universe.h"

class Client;
class Message;

class Game {

public:
	Game(const char *serverAddress, unsigned short port);
	~Game();
	void run();

private:
	Game(const Game&);
	const Game &operator = (const Game&);

	void input();
	void logic();
	void display();
	void handleMessage(const Message &m);
	void handleKey(sf::Key::Code code, bool down);
	void shoot();
	void allocShips(size_t n);

	sf::RenderWindow *screen;
	sf::View view;
	sf::Clock clock;
	Universe universe;
	unsigned short id;
	Client *client;
	enum {
		NOTHING, REQUEST_SENT, WAITING,
		SELECT_ACTION, SELECT_DONE,
		ROUND, ROUND_DONE
	} state;
	long roundCntr;
	double lastUpdate;
	double moveDown, moveRight, zoom;
	short moveDownDelta, moveRightDelta, zoomDelta;

};

#endif
