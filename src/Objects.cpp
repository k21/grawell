#include "Objects.h"

using namespace std;
using namespace sf;

void Planet::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), green(0, 255, 0);
	Shape circle = Shape::Circle(
			x(), y(), radius(),
			transparent, 1, green);
	target.Draw(circle);
}

void Ship::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), blue(0, 0, 255);
	Shape circle = Shape::Circle(
			x(), y(), radius(),
			transparent, 1, blue);
	target.Draw(circle);
}

void Bullet::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), red(255, 0, 0);
	Shape circle = Shape::Circle(
			x(), y(), radius(),
			transparent, 1, red);
	target.Draw(circle);
}

static Vector acceleration(const Point &pos, const Planet &pl) {
	Vector v = pl.center() - pos;
	double dist = v.size();
	v.normalize();
	double size = pl.mass() / (dist * dist);
	return size * v;
}

static Vector acceleration(const Point &pos, const vector<Planet> &planets) {
	Vector v;
	for (size_t i = 0; i < planets.size(); ++i) {
		v += acceleration(pos, planets[i]);
	}
	return v;
}

bool Bullet::update(const vector<Planet> &planets) {
	Vector acc = acceleration(center(), planets);
	speed_ += acc;
	center() += speed_;
	for (size_t i = 0; i < planets.size(); ++i) {
		if (planets[i].intersects(center())) return true;
	}
	return false;
}
