#ifndef VECTOR_H
#define VECTOR_H

#include "../Defines.h"


template <class T>
class Vector
{
public:
	T array[CORR_LEN * 2] = {};
	uint32_t lastPos = 0;
	uint32_t size = 0;

	void insert(T element, uint32_t pos)
	{
		T tmp;
		if(array[pos])
		{
			for(int i = (CORR_LEN * 2) - 1; i >= 0; --i)
			{
#pragma HLS UNROLL factor=8
				if(i <= pos)
				{
					break;
				}
				tmp = array[i-1];
				array[i] = tmp;
			}
		}
		array[pos] = element;
		++lastPos;
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
		T tmp;
ERASING_LOOP:
		for(int i = 0; i < (CORR_LEN*2); ++i)
		{
#pragma HLS UNROLL factor=8
			if(i >= pos)
			{
				tmp = array[i+1];
				array[i] = tmp;
			}
		}
		size--;
		lastPos--;
	}

	void pushFront(T element)
	{
		T tmp;
PUSHFRONT_LOOP:
		for(int i = (CORR_LEN*2)-1; i >0 ; --i)
		{
#pragma HLS UNROLL factor=8
			tmp = array[i-1];
			array[i] = tmp;
		}
		array[0] = element;
		size++;
		lastPos++;
	}

	void elementErase(T element)
	{
ERASE_LOOP:
		for(int i = 0; i < (CORR_LEN*2); ++i)
		{
#pragma HLS UNROLL factor=8
			if(array[i] == element && i < lastPos)
			{
				erase(i);
				break;
			}else if(i >= lastPos)
			{
				break;
			}
		}
	}

	void elementEmplace(T element)
	{
SEARCH_ELEMENT_LOOP:
		for(int i = 0; i < (CORR_LEN*2); ++i)
		{
#pragma HLS UNROLL factor=8
			if(array[i] == element && i < lastPos)
			{
				return;
			}
			else if(i >= lastPos)
			{
				break;
			}
		}

		uint32_t pos = 0;
		if(size == 0)
		{
			emplace(element);
		}
		else
		{
			for(int i = 0; i < (CORR_LEN*2); i++)
			{
#pragma HLS UNROLL factor=8
				if(element < array[i] && i < lastPos)
				{
					insert(element, i);
					return;
				}
				else if(i >= lastPos)
				{
					break;
				}
				else
				{
					pos = i+1;
				}
			}
			insert(element, pos);
			return;
		}
	}

	void fillnReset(T element)
	{
RESET_LOOP:
		for(int i = 0; i < CORR_LEN*2; ++i)
		{
#pragma HLS UNROLL factor=16
			array[i] = element;
		}
		size = 0;
		lastPos = 0;
	}

	void plusOne(uint32_t pos)
	{
		++array[pos];
	}
};


#endif
