#include <cstdlib>

#include "Sphere.h"

#include "Placer.h"

using namespace std;

static long round(double x, double gridSize) {
	return (long)floor((x+gridSize/2)/gridSize);
}

void Placer::place(Sphere &s) {
	int i = rand() % edge.size();
	auto it = edge.begin();
	advance(it, i);
	pair<long, long> p = *it;
	edge.erase(p);
	used.insert(p);
	long x = p.first, y = p.second;
	pair<long, long> left(x-1, y), right(x+1, y), top(x, y-1), bottom(x, y+1);
	if (used.find(  left) == used.end()) edge.insert(  left);
	if (used.find( right) == used.end()) edge.insert( right);
	if (used.find(   top) == used.end()) edge.insert(   top);
	if (used.find(bottom) == used.end()) edge.insert(bottom);
	long radius = (long)(s.radius*1024);
	long range = gridSize*1024 - 2*radius;
	s.center.x = x*gridSize-(gridSize/2)+(radius+rand()%range)/1024.0;
	s.center.y = y*gridSize-(gridSize/2)+(radius+rand()%range)/1024.0;
}

void Placer::checkEdge(pair<long, long> p) {
	long x = p.first, y = p.second;
	if (x == 0 && y == 0) return;
	if (edge.find(p) == edge.end()) return;
	pair<long, long> left(x-1, y), right(x+1, y), top(x, y-1), bottom(x, y+1);
	if (used.find(  left) != used.end()) return;
	if (used.find( right) != used.end()) return;
	if (used.find(   top) != used.end()) return;
	if (used.find(bottom) != used.end()) return;
	edge.erase(p);
}

void Placer::remove(Sphere &s) {
	long x = round(s.center.x, gridSize), y = round(s.center.y, gridSize);
	pair<long, long> p = make_pair(x, y);
	used.erase(p);
	edge.insert(p);
	pair<long, long> left(x-1, y), right(x+1, y), top(x, y-1), bottom(x, y+1);
	checkEdge(  left);
	checkEdge( right);
	checkEdge(   top);
	checkEdge(bottom);
}
