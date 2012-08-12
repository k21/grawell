#include <cstdlib>

#include "Entity.h"

#include "Placer.h"

using namespace std;

static int32_t round(int32_t x, int32_t gridSize) {
	int32_t rx = x+gridSize/2;
	return rx/gridSize - (rx < 0);
}

uint32_t Placer::random(uint32_t begin, uint32_t end) {
	seed *= 0x5DEECE66D;
	seed += 0xB;
	seed &= 0xFFFFFF;
	return (uint32_t)(begin + seed % (end - begin));
}

void Placer::place(Entity &e) {
	int32_t i = (int32_t)random(0, (uint32_t)edge.size());
	auto it = edge.begin();
	advance(it, i);
	pair<int32_t, int32_t> p = *it;
	edge.erase(p);
	used.insert(p);
	int32_t x = p.first, y = p.second;
	pair<int32_t, int32_t>
			left(x-1, y), right(x+1, y), top(x, y-1), bottom(x, y+1);
	if (used.find(  left) == used.end()) edge.insert(  left);
	if (used.find( right) == used.end()) edge.insert( right);
	if (used.find(   top) == used.end()) edge.insert(   top);
	if (used.find(bottom) == used.end()) edge.insert(bottom);
	e.center.x = x*gridSize-(gridSize/2);
	e.center.y = y*gridSize-(gridSize/2);
	e.center.x += random(e.radius, gridSize - e.radius);
	e.center.y += random(e.radius, gridSize - e.radius);
}

void Placer::checkEdge(pair<int32_t, int32_t> p) {
	int32_t x = p.first, y = p.second;
	if (edge.size() == 1) return;
	if (edge.find(p) == edge.end()) return;
	pair<int32_t, int32_t> left(x-1, y), right(x+1, y), top(x, y-1), bottom(x, y+1);
	if (used.find(  left) != used.end()) return;
	if (used.find( right) != used.end()) return;
	if (used.find(   top) != used.end()) return;
	if (used.find(bottom) != used.end()) return;
	edge.erase(p);
}

void Placer::remove(Entity &e) {
	int32_t x = round(e.center.x, gridSize), y = round(e.center.y, gridSize);
	pair<int32_t, int32_t> p = make_pair(x, y);
	used.erase(p);
	edge.insert(p);
	pair<int32_t, int32_t>
			left(x-1, y), right(x+1, y), top(x, y-1), bottom(x, y+1);
	checkEdge(  left);
	checkEdge( right);
	checkEdge(   top);
	checkEdge(bottom);
	checkEdge(     p);
}
