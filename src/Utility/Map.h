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
public:
	Vector<Touple<T, U>> map;
	U defaultValue;
	Map()
	{
#pragma HLS ARRAY_PARTITION variable=map.array type=cyclic factor=16
	}

	void add(T v1, U v2)
	{
		Touple<T, U> tmp = {v1, v2};
		map.emplace(tmp);
	}

	U find(T v1)
	{
#pragma HLS INLINE off
		U toRet = defaultValue;
FIND_LOOP:
		for(int i = 0; i < (CORR_LEN*2); ++i)
		{
#pragma HLS UNROLL factor=16
			if(map.at(i).v1 == v1 && i < map.getSize())
			{
				toRet = map.get(i)->v2;
			}
		}
		return defaultValue;
	}

	void update(T v1, U v2)
	{
#pragma HLS INLINE off
UPDATE_LOOP:
		for(int i = 0; i < (CORR_LEN*2); ++i)
		{
#pragma HLS UNROLL factor=16
			if(map.at(i).v1 == v1 && i < map.getSize())
			{
				map.get(i)->v2 = v2;
			}
		}
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
#pragma HLS INLINE off
		uint32_t pos;
ERASE_LOOP:
		for(int i = 0; i < (CORR_LEN*2); ++i)
		{
#pragma HLS UNROLL factor=16
			if(map.at(i).v1 == key && i < map.getSize())
			{
				pos = i;
			}

		}
		map.erase(pos);
	}

	void reset()
	{
RESET_LOOP:
		while(map.getSize() != 0)
		{
#pragma HLS PIPELINE
			map.erase(0);
		}
	}
};



#endif
