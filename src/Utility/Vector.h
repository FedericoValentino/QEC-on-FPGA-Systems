#ifndef VECTOR_H
#define VECTOR_H

#include "../Defines.h"


template <class T>
class Vector
{
private:
	T array[CORR_LEN * 2] = {};
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
	T back()
	{
		return array[lastPos-1];
	}
	void empty()
	{
		lastPos = 0;
		size = 0;
	}
	void erase(uint32_t pos)
	{
		for(int i = pos; i < lastPos; ++i)
		{
			array[i] = array[i+1];
		}
		size--;
		lastPos--;
	}

	void pushFront(T element)
	{
		for(int i = lastPos; i >0 ; --i)
		{
			array[i] = array[i-1];
		}
		array[0] = element;
		size++;
		lastPos++;
	}

	void elementErase(T element)
	{
		for(int i = 0; i < lastPos; ++i)
		{
			if(array[i] == element)
			{
				erase(i);
				break;
			}
		}
	}

	void elementEmplace(T element)
	{
		for(int i = 0; i < lastPos; ++i)
		{
			if(array[i] == element)
			{
				return;
			}
		}
		emplace(element);
	}

	void fillnReset(T element)
	{
		for(int i = 0; i < CORR_LEN*2; ++i)
		{
			array[i] = element;
		}
		size = 0;
		lastPos = 0;
	}
};


#endif
