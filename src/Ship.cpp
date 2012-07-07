#include <sstream>

#include "Bullet.h"

#include "Ship.h"

using namespace std;
using namespace boost;
using namespace sf;

Color Ship::getColorByID(uint16_t id) {
	float p = fmodf(2.2f*id,6);
	int8_t n = (int8_t)p;
	int16_t q1 = (int16_t)((p-n)*256);
	int16_t q2 = (int16_t)(256-q1);
	int16_t r, g, b;
	switch (n) {
		case 0: r = 255; g =  q1; b =   0; break;
		case 1: r =  q2; g = 255; b =   0; break;
		case 2: r =   0; g = 255; b =  q1; break;
		case 3: r =   0; g =  q2; b = 255; break;
		case 4: r =  q1; g =   0; b = 255; break;
		case 5: r = 255; g =   0; b =  q2; break;
	}
	r = max((int16_t)0, min((int16_t)255, r));
	g = max((int16_t)0, min((int16_t)255, g));
	b = max((int16_t)0, min((int16_t)255, b));
	return Color((Uint8)r, (Uint8)g, (Uint8)b);
}

void Ship::draw(RenderTarget &target) const {
	Color whiteTransparent(255, 255, 255, 128);
	Color c = getColorByID(id());
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius, c);
	float lineLength = 1.2f*(float)radius + (float)(strength*FIXED_ONE)/100.f;
	Vector d = center + Vector::polar(direction, (Vector::T)lineLength);
	Shape line = Shape::Line((float)center.x, (float)center.y,
			(float)d.x, (float)d.y, FIXED_ONE, c);
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

void Ship::shoot(EntityManager<Bullet> &bullets) const {
	Vector d = Vector::polar(direction, radius+2*FIXED_ONE);
	Point pos = center + d;
	Vector speed = Vector::polar(direction, strength*FIXED_ONE/100);
	Bullet &b = bullets.alloc();
	b.owner(id()); b.center = pos; b.speed = speed;
}
