#include "Universe.h"

using namespace std;
using namespace boost;

Universe::Universe():
		settings(), planets(), ships(), bullets() {}

void Universe::update(list<pair<uint16_t, uint16_t>> &hits) {
	auto it = bullets.begin();
	while (it != bullets.end()) {
		if (it->update(planets, ships, hits)) {
			it = bullets.erase(it);
		} else ++it;
	}
}
