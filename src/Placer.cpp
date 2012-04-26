#include <cstdlib>

#include "Sphere.h"

#include "Placer.h"

using namespace std;
using namespace boost;

static int32_t round(int32_t x, int32_t gridSize) {
	return (x+gridSize/2)/gridSize;
}

void Placer::place(Sphere &s) {
	int32_t i = (int32_t)(rand() % edge.size());
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
	int32_t range = gridSize - 2*s.radius;
	s.center.x = x*gridSize-(gridSize/2)+(s.radius+rand()%range);
	s.center.y = y*gridSize-(gridSize/2)+(s.radius+rand()%range);
}

void Placer::checkEdge(pair<int32_t, int32_t> p) {
	int32_t x = p.first, y = p.second;
	if (x == 0 && y == 0) return;
	if (edge.find(p) == edge.end()) return;
	pair<int32_t, int32_t> left(x-1, y), right(x+1, y), top(x, y-1), bottom(x, y+1);
	if (used.find(  left) != used.end()) return;
	if (used.find( right) != used.end()) return;
	if (used.find(   top) != used.end()) return;
	if (used.find(bottom) != used.end()) return;
	edge.erase(p);
}

void Placer::remove(Sphere &s) {
	int32_t x = round(s.center.x, gridSize), y = round(s.center.y, gridSize);
	pair<int32_t, int32_t> p = make_pair(x, y);
	used.erase(p);
	edge.insert(p);
	pair<int32_t, int32_t>
			left(x-1, y), right(x+1, y), top(x, y-1), bottom(x, y+1);
	checkEdge(  left);
	checkEdge( right);
	checkEdge(   top);
	checkEdge(bottom);
}
