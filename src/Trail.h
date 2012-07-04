#ifndef TRAIL_H_
#define TRAIL_H_

#include <algorithm>
#include <list>

#include <SFML/Graphics.hpp>

#include "Vector.h"

class Trail {
public:
	Trail(size_t maxSize_, sf::Color color_):
			maxSize(maxSize_), color(color_), points(), last() {}
	void add(Point p);
	void setMaxSize(size_t size);
	void draw(sf::RenderTarget &target) const;

	size_t getMaxSize() const { return maxSize; }
	const sf::Color &getColor() const { return color; }

	void setColor(const sf::Color &c) { color = c; }

	void swapPoints(Trail &t) {
		points.swap(t.points);
		std::swap(last, t.last);
	}

private:
	size_t maxSize;
	sf::Color color;
	std::list<Point> points;
	Point last;
};

#endif
