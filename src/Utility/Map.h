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
		return 0;
	}

	void update(T v1, U v2)
	{
		U* toUpdate = find(v1);
		*toUpdate = v2;
	}

};



#endif
