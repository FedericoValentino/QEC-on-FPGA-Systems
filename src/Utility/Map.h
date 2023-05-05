#ifndef MAP_H
#define MAP_H

#include "Vector.h"

template <typename T1, typename T2>
struct Touple
{
	T1 v1;
	T2 v2;
};

template <class T, class U>
class Map
{
private:
	Vector<Touple<T, U>> map;
public:
	void add(T v1, U v2)
	{
		Touple<T, U> tmp = {v1, v2};
		map.emplace(tmp);
	}

	U* find(T v1)
	{
		for(int i = 0; i < map.getSize(); ++i)
		{
			if(map.at(i).v1 == v1)
			{
				return &map.get(i)->v2;
			}
		}
		return nullptr;
	}

	void update(T v1, U v2)
	{
		U* toUpdate = find(v1);
		*toUpdate = v2;
	}

	Touple<T, U>* get(uint32_t pos)
	{
		return map.get(pos);
	}

	uint32_t size()
	{
		return map.getSize();
	}

	void erase(uint32_t key)
	{
		uint32_t pos;
		for(int i = 0; i < map.getSize(); ++i)
		{
			if(map.at(i).v1 == key)
			{
				pos = i;
				break;
			}
		}
		map.erase(pos);
	}

	void reset()
	{
		while(map.getSize() != 0)
		{
			map.erase(0);
		}
	}
};



#endif
