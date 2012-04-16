#ifndef PLACER_H_
#define PLACER_H_

#include <set>
#include <utility>

class Sphere;

class Placer {

public:
	Placer(long gridSize_): gridSize(gridSize_), used(), edge() {
		edge.insert(std::make_pair(0, 0));
	}

	void place(Sphere &s);
	void remove(Sphere &s);

private:
	void checkEdge(std::pair<long, long> p);

	long gridSize;
	std::set<std::pair<long, long> > used, edge;

};

#endif
