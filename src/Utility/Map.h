#ifndef MAP_H
#define MAP_H

#include "Vector.h"


template <class U>
class Map
{
public:
	U map[SYN_LEN] = {};
	U defaultValue;


	void add(uint32_t pos, U v2)
	{
		map[pos] = v2;
	}

	U find(uint32_t pos)
	{
		return map[pos];
	}

	void update(uint32_t v1, U v2)
	{
		map[v1] = v2;
	}

	void erase(uint32_t key)
	{
		map[key] = defaultValue;
	}

	void reset()
	{
		for(int i = 0; i < MAPLEN; i++)
		{
			map[i] = defaultValue;
		}
	}
};



#endif
