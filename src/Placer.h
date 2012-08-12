#ifndef PLACER_H_
#define PLACER_H_

#include <cstdint>
#include <set>
#include <utility>

class Entity;

class Placer {

public:
	Placer(int32_t gridSize_): gridSize(gridSize_), used(), edge() {
		edge.insert(std::make_pair(0, 0));
	}

	void place(Entity &e);
	void remove(Entity &e);

private:
	void checkEdge(std::pair<int32_t, int32_t> p);

	int32_t gridSize;
	std::set<std::pair<int32_t, int32_t>> used, edge;

};

#endif
