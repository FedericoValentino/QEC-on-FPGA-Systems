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
VECTOR_INSERT_LOOP:
			for(int i = (CORR_LEN * 2) - 1; i >= 0; --i)
			{
#pragma HLS UNROLL factor=8
				if(i > pos)
				{
					tmp = array[i-1];
					array[i] = tmp;
				}

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
		T arraytmp[CORR_LEN*2] ={};
#pragma HLS ARRAY_PARTITION variable=arraytmp type=cyclic factor=4
		int j = 0;

ERASING_LOOP:
		for(int i = 0; i < (CORR_LEN*2); ++i)
		{
#pragma HLS UNROLL factor=2
			if(i != pos)
			{
				arraytmp[j] = array[i];
				j++;
			}
		}

COPY_LOOP:
		for(int i = 0; i < CORR_LEN*2; i++)
		{
#pragma HLS UNROLL factor=8
			array[i] = arraytmp[i];
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
ELEMENT_ERASE_LOOP:
		for(int i = 0; i < (CORR_LEN*2); ++i)
		{
#pragma HLS UNROLL factor=8
			T tmp =array[i];
			if(tmp == element && i < lastPos)
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
		if(size == 0)
		{
			emplace(element);
		}
		else
		{
ELEMENT_EMPLACE_LOOP:
			for(int i = 0; i < CORR_LEN*2; i++)
			{
#pragma HLS UNROLL factor=8
				T tmp = array[i];
				if(element == tmp)
				{
					return;
				}
				if(element < tmp)
				{
					insert(element, i);
					return;
				}
			 }
			emplace(element);
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
