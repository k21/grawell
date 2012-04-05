#include "Objects.h"

using namespace sf;

void Planet::draw(RenderTarget &target) const {
	Color transparent(0, 0, 0, 0), green(0, 255, 0);
	Shape circle = Shape::Circle(
			x(), y(), radius(),
			transparent, 1, green);
	target.Draw(circle);
}
