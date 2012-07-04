#include "Bullet.h"

using namespace std;
using namespace boost;
using namespace sf;

void Bullet::draw(RenderTarget &target) const {
	Color c = Ship::getColorByID(myOwner);
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius, c);
	target.Draw(circle);
	trail.draw(target);
}

static Vector acceleration(const Point &pos, const Planet &pl) {
	Vector v = pl.center - pos;
	int32_t dist = v.size();
	int32_t force = (int32_t)(((int64_t)pl.mass<<32) / ((int64_t)dist * dist));
	v.normalize(force);
	return v;
}

static Vector acceleration(const Point &pos, EntityManager<Planet> &planets) {
	Vector v;
	for (size_t i = 0; i < planets.size(); ++i) {
		v += acceleration(pos, planets[i]);
	}
	return v;
}

bool Bullet::update(EntityManager<Planet> &planets,
		EntityManager<Ship> &ships, list<pair<uint16_t, uint16_t>> &hits,
		bool updateTrails) {
#define SPEED_TO_POS(a) ((a)/(1<<8))
	Vector kv1 = acceleration(center, planets);
	Vector kr1 = SPEED_TO_POS(speed);
	Vector kv2 = acceleration(center+kr1/2, planets);
	Vector kr2 = SPEED_TO_POS(speed+kv1/2);
	Vector kv3 = acceleration(center+kr2/2, planets);
	Vector kr3 = SPEED_TO_POS(speed+kv2/2);
	Vector kv4 = acceleration(center+kr3, planets);
	Vector kr4 = SPEED_TO_POS(speed+kv3);
	speed  += (kv1 + 2*kv2 + 2*kv3 + kv4)/6;
	center += (kr1 + 2*kr2 + 2*kr3 + kr4)/6;
#undef SPEED_TO_POS
	if (updateTrails) {
		trail.add(center);
	}
	for (Planet &p : planets) {
		if (p.active() && p.intersects(center)) return true;
	}
	for (Ship &s : ships) {
		if (s.alive && s.intersects(center)) {
			hits.push_back(make_pair(myOwner, s.id()));
			return true;
		}
	}
	return false;
}
