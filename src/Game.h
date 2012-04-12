#ifndef GAME_H_
#define GAME_H_

#include <SFML/Graphics.hpp>

#include "Client.h"
#include "Universe.h"
#include "Server.h"

class Game {

public:
	Game();
	~Game();
	void run();
	void newGame(const Settings &settings);

private:
	Game(const Game&);
	const Game &operator = (const Game&);

	void input();
	void logic(double dt);
	void display();
	void handleMessage(const Message &m);
	void keyPressed(sf::Key::Code code);
	void shoot();
	void allocShips(size_t n);

	sf::RenderWindow *screen;
	sf::Clock clock;
	Universe universe;
	unsigned short id;
	Server *server;
	Client *client;
	enum {
		NOTHING, REQUEST_SENT, WAITING,
		SELECT_ACTION, SELECT_DONE,
		ROUND, ROUND_DONE
	} state;

};

#endif
