#include "Planet.h"

using namespace std;
using namespace sf;

void Planet::draw(RenderTarget &target) const {
	Color green(0, 255, 0);
	CircleShape circle = CircleShape((float)radius);
	circle.setOrigin((float)radius, (float)radius);
	circle.setPosition((float)center.x, (float)center.y);
	circle.setFillColor(green);
	target.draw(circle);
}
