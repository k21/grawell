#include <sstream>

#include "Entities.h"

using namespace std;
using namespace boost;
using namespace sf;

void Planet::draw(RenderTarget &target) const {
	Color green(0, 255, 0);
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius, green);
	target.Draw(circle);
}

void Ship::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), blue(0, 0, 255),
			whiteTransparent(255, 255, 255, 128);
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius,
			transparent, FIXED_ONE, blue);
	Vector d = center + Vector::polar(direction, radius);
	Shape line = Shape::Line((float)center.x, (float)center.y,
			(float)d.x, (float)d.y, FIXED_ONE, blue);
	target.Draw(circle);
	target.Draw(line);
	stringstream ss;
	ss << name << " (" << score << ")";
	String s(ss.str(), Font::GetDefaultFont(), 20*FIXED_ONE);
	s.SetColor(whiteTransparent);
	s.SetCenter(s.GetRect().GetWidth()/2, s.GetRect().GetHeight());
	s.SetPosition((float)center.x, (float)(center.y-radius-5));
	target.Draw(s);
}

void Bullet::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), red(255, 0, 0), yellow(255, 255, 0);
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius, red);
	target.Draw(circle);
	for (size_t i = 1; i < trail.size(); ++i) {
		Shape line = Shape::Line((float)trail[i-1].x, (float)trail[i-1].y,
				(float)trail[i].x, (float)trail[i].y, FIXED_ONE, yellow);
		target.Draw(line);
	}
}

void Ship::shoot(EntityManager<Bullet> &bullets) const {
	Vector d = Vector::polar(direction, radius+2*FIXED_ONE);
	Point pos = center + d;
	Vector speed = Vector::polar(direction, 50*FIXED_ONE);
	Bullet &b = bullets.alloc();
	b.playerID = id(); b.center = pos; b.speed = speed;
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
		EntityManager<Ship> &ships, list<pair<uint16_t, uint16_t>> &hits) {
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
	if (trail.empty() || (center-trail.back()).size() > 6*FIXED_ONE) {
		trail.push_back(center);
	}
	for (Planet &p : planets) {
		if (p.active() && p.intersects(center)) return true;
	}
	for (Ship &s : ships) {
		if (s.alive && s.intersects(center)) {
			hits.push_back(make_pair(playerID, s.id()));
			return true;
		}
	}
	return false;
}
