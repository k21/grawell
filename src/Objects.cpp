#include "Objects.h"

using namespace std;
using namespace sf;

void Planet::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), green(0, 255, 0);
	Shape circle = Shape::Circle(
			center.x, center.y, radius,
			transparent, 1, green);
	target.Draw(circle);
}

void Ship::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), blue(0, 0, 255);
	Shape circle = Shape::Circle(
			center.x, center.y, radius,
			transparent, 1, blue);
	Vector d = center + radius * Vector::direction(dir/18000.0*M_PI);
	Shape line = Shape::Line(center.x, center.y, d.x, d.y, 1, blue);
	target.Draw(circle);
	target.Draw(line);
}

void Bullet::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), red(255, 0, 0);
	Shape circle = Shape::Circle(
			center.x, center.y, radius,
			transparent, 1, red);
	target.Draw(circle);
}

Bullet Ship::shoot() const {
	return Bullet(center, 200*Vector::direction(dir/18000.0*M_PI));
}

static Vector acceleration(const Point &pos, const Planet &pl) {
	Vector v = pl.center - pos;
	double dist = v.size();
	v.normalize();
	double size = pl.mass / (dist * dist);
	return size * v;
}

static Vector acceleration(const Point &pos, const vector<Planet> &planets) {
	Vector v;
	for (size_t i = 0; i < planets.size(); ++i) {
		v += acceleration(pos, planets[i]);
	}
	return v;
}

bool Bullet::update(const vector<Planet> &planets, double dt) {
	Vector kv1 = acceleration(center, planets);
	Vector kr1 = speed;
	Vector kv2 = acceleration(center+dt/2*kr1, planets);
	Vector kr2 = speed+dt/2*kv1;
	Vector kv3 = acceleration(center+dt/2*kr2, planets);
	Vector kr3 = speed+dt/2*kv2;
	Vector kv4 = acceleration(center+dt*kr3, planets);
	Vector kr4 = speed+dt*kv3;
	speed += dt/6 * (kv1 + 2*kv2 + 2*kv3 + kv4);
	center += dt/6 * (kr1 + 2*kr2 + 2*kr3 + kr4);
	for (size_t i = 0; i < planets.size(); ++i) {
		if (planets[i].intersects(center)) return true;
	}
	return false;
}
