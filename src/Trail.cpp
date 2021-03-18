#include "Trail.h"

using namespace std;
using namespace sf;

void Trail::add(Point p) {
	last = p;

	bool doAdd = false;
	if (points.size() < 2) doAdd = true;
	else {
		auto it = points.end();
		Point p2 = *(--it);
		if (p2.x != p.x || p2.y != p.y) {
			Point p1 = *(--it);
			Vector v1 = p2 - p1;
			Vector v2 = p - p2;
			float dir1 = atan2((float)v1.y, (float)v1.x);
			float dir2 = atan2((float)v2.y, (float)v2.x);
			float dirChange = dir1 - dir2;
			if (dirChange < 0) dirChange += (float)(2*M_PI);
			if (dirChange > M_PI) dirChange = (float)(2*M_PI) - dirChange;
			if ((float)v2.size() > (float)FIXED_ONE / dirChange) doAdd = true;
		}
	}

	if (doAdd) {
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

static RectangleShape line(
		Vector p1, Vector p2, float width, float outlineWidth, Color color) {
	Vector diff = p2 - p1;
	float dist = (float)diff.size();
	RectangleShape rect(Vector2f(dist, (float)width));
	rect.setOrigin(0.0f, (float)width / 2.0f);
	rect.setPosition((float)p1.x, (float)p1.y);
	rect.rotate((float)(180.0f / M_PI * atan2((float)diff.y, (float)diff.x)));
	rect.setOutlineThickness(outlineWidth);
	rect.setFillColor(color);
	rect.setOutlineColor(color);
	return rect;
}

void Trail::draw(RenderTarget &target) const {
	if (points.empty()) return;
	float width = 6*FIXED_ONE;
	float outlineWidth = 1*FIXED_ONE;
	auto it1 = points.begin();
	auto it2 = it1; ++it2;
	while (it2 != points.end()) {
		target.draw(line(*it1, *it2, width, outlineWidth, color));
		++it1; ++it2;
	}
	target.draw(line(points.back(), last, width, outlineWidth, color));
}
