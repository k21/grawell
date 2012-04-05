#ifndef GAME_H_
#define GAME_H_

#include <vector>

#include <SFML/Graphics.hpp>

#include "Objects.h"
#include "Settings.h"

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
	void logic();
	void display();

	sf::RenderWindow *screen;
	sf::Clock clock;
	Settings settings;
	std::vector<Planet> planets;
	std::vector<Ship>   ships;
	std::vector<Bullet> bullets;

};

#endif
