#include "Planet.h"

using namespace std;
using namespace sf;

void Planet::draw(RenderTarget &target) const {
	Color green(0, 255, 0);
	Shape circle = Shape::Circle(
			(float)center.x, (float)center.y, (float)radius, green);
	target.Draw(circle);
}
