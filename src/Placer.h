#ifndef PLACER_H_
#define PLACER_H_

#include <set>
#include <utility>

#include <boost/cstdint.hpp>

class Sphere;

class Placer {

public:
	Placer(boost::int32_t gridSize_): gridSize(gridSize_), used(), edge() {
		edge.insert(std::make_pair(0, 0));
	}

	void place(Sphere &s);
	void remove(Sphere &s);

private:
	void checkEdge(std::pair<boost::int32_t, boost::int32_t> p);

	boost::int32_t gridSize;
	std::set<std::pair<boost::int32_t, boost::int32_t>> used, edge;

};

#endif
