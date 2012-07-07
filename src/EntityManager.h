#ifndef ENTITY_MANAGER_H_
#define ENTITY_MANAGER_H_

#include <vector>

#include <cstdint>

#include "Vector.h"

template <typename T>
class EntityManager {
public:
	EntityManager(): pool(), freeIDs() {}
	T &alloc() {
		T *res;
		if (!freeIDs.empty()) {
			auto it = freeIDs.begin();
			size_t i = *it;
			res = &pool[i];
			freeIDs.erase(it);
		} else {
			size_t i = pool.size();
			pool.push_back(T((uint16_t)i));
			res = &pool.back();
		}
		res->myActive = true;
		return *res;
	}
	void free(T &e) {
		e.myActive = false;
		freeIDs.insert(e.id());
		e = T(e.id());
		while (!pool.empty() && !pool.back().active()) {
			pool.pop_back();
			freeIDs.erase((uint16_t)pool.size());
		}
	}
	T &operator [] (size_t id) {
		while (pool.size() < id+1) {
			pool.push_back(T((uint16_t)pool.size()));
		}
		pool[id].myActive = true;
		return pool[id];
	}
	size_t size() const { return pool.size(); }
	bool empty() const { return pool.empty(); }
	void clear() { pool.clear(); freeIDs.clear(); }

	typename std::vector<T>::iterator begin() { return pool.begin(); }
	typename std::vector<T>::iterator end()   { return pool.end();   }
	typename std::vector<T>::const_iterator begin() const { return pool.begin(); }
	typename std::vector<T>::const_iterator end()   const { return pool.end();   }

private:
	std::vector<T> pool;
	std::set<uint16_t> freeIDs;

};

#endif
