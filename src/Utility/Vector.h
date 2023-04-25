#ifndef VECTOR_H
#define VECTOR_H

#include "../Defines.h"


template <class T>
class Vector
{
private:
	T array[CORR_LEN * 2] = {0};
	uint32_t lastPos = 0;
	uint32_t size = 0;
public:

	void insert(T element, uint32_t pos)
	{
		if(array[pos])
		{
			for(int i = lastPos; i > pos; --i)
			{
				array[i] = array[i-1];
			}
			++lastPos;
		}
		array[pos] = element;
		++size;
	}

	void emplace(T element)
	{
		array[lastPos] = element;
		++lastPos;
		++size;
	}
	uint32_t getSize()
	{
		return size;
	}
	T at(uint32_t i)
	{
		return array[i];
	}
	T* get(uint32_t i)
	{
		return &array[i];
	}
	void set(T element, uint32_t pos)
	{
		array[pos] = element;
	}
};


#endif
