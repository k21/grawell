#ifndef GAME_H_
#define GAME_H_

#include <list>
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
	void logic(double dt);
	void display();

	sf::RenderWindow *screen;
	sf::Clock clock;
	Settings settings;
	std::vector<Planet> planets;
	std::vector<Ship>   ships;
	std::list<Bullet> bullets;
	int controlled;

};

#endif
