#ifndef GAME_H_
#define GAME_H_

#include <SFML/Graphics.hpp>

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

	sf::RenderWindow *screen;
	sf::Clock clock;
	Universe universe;
	int controlled;
	Server *server;

};

#endif
