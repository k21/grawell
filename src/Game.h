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
	void keyPressed(sf::Key::Code code);
	void shoot();
	void allocShips(size_t n);

	sf::RenderWindow *screen;
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

};

#endif
