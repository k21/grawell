#include "Trail.h"

using namespace std;
using namespace sf;

void Trail::add(Point p) {
	last = p;
	if (points.empty() || (p-points.back()).size() > 24*FIXED_ONE) {
		points.push_back(p);
		if (points.size() > maxSize) {
			points.pop_front();
		}
	}
}

void Trail::setMaxSize(size_t size) {
	maxSize = size;
	while (points.size() > maxSize) {
		points.pop_front();
	}
}

void Trail::draw(RenderTarget &target) const {
	if (points.empty()) return;
	float width = 8*FIXED_ONE;
	auto it1 = points.begin();
	auto it2 = it1; ++it2;
	while (it2 != points.end()) {
		Shape line = Shape::Line((float)it1->x, (float)it1->y,
				(float)it2->x, (float)it2->y, width, color);
		target.Draw(line);
		++it1; ++it2;
	}
	Shape line = Shape::Line((float)points.back().x, (float)points.back().y,
			(float)last.x, (float)last.y, width, color);
	target.Draw(line);
}
