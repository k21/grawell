#include <sstream>

#include "Objects.h"

using namespace std;
using namespace boost;
using namespace sf;

void Planet::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), green(0, 255, 0);
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius,
			transparent, 1, green);
	target.Draw(circle);
}

void Ship::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), blue(0, 0, 255),
			whiteTransparent(255, 255, 255, 128);
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius,
			transparent, 1, blue);
	Vector d = center + radius * Vector::direction(direction/18000.0*M_PI);
	Shape line = Shape::Line((float)center.x, (float)center.y,
			(float)d.x, (float)d.y, 1, blue);
	target.Draw(circle);
	target.Draw(line);
	stringstream ss;
	ss << name << " (" << score << ")";
	String s(ss.str(), Font::GetDefaultFont(), 20);
	s.SetColor(whiteTransparent);
	s.SetCenter(s.GetRect().GetWidth()/2, s.GetRect().GetHeight());
	s.SetPosition((float)center.x, (float)(center.y-radius-5));
	target.Draw(s);
}

void Bullet::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), red(255, 0, 0);
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius,
			transparent, 1, red);
	target.Draw(circle);
}

Bullet Ship::shoot() const {
	Vector d = Vector::direction(direction/18000.0*M_PI);
	Point pos = center + d*(radius+2);
	Vector speed = 200*d;
	return Bullet(id, pos, speed);
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

bool Bullet::update(const vector<Planet> &planets, vector<Ship> &ships,
		list<pair<uint16_t, uint16_t>> &hits, double dt) {
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
	for (size_t i = 0; i < ships.size(); ++i) {
		if (!ships[i].active) continue;
		if (ships[i].intersects(center)) {
			hits.push_back(make_pair(playerID, (uint16_t)i));
			return true;
		}
	}
	return false;
}
