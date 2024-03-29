#ifndef VECTOR_H
#define VECTOR_H

#include "../Defines.h"


template <class T>
class Vector
{
public:
	T array[SYN_LEN];
	uint32_t lastPos = 0;
	uint32_t size = 0;
	Vector()
	{
//#pragma HLS ARRAY_PARTITION variable=array type=cyclic factor=16
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
		if(i >= 0 && i < SYN_LEN)
			return array[i];
		else
			return array[0];
	}
	void set(T element, uint32_t pos)
	{
#pragma HLS INLINE off
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
		if(pos < SYN_LEN - 1)
		{
			T tmp = array[pos+1];
			int i = pos;
	ERASING_LOOP:
			while(i < size)
			{
	//#pragma HLS DEPENDENCE variable=tmp type=inter false
	#pragma HLS PIPELINE II=1
				array[i] = tmp;
				tmp = array[i+2];
				i++;
			}
		}
		size--;
		lastPos--;
	}

	void elementErase(T element)
	{
		bool found = false;
		int pos;
		T tmp = array[0];
ELEMENT_ERASE_LOOP:
		for(int i = 0; i < size; i++)
		{
#pragma HLS PIPELINE II=1
			if(tmp == element)
			{
				found = true;
				pos = i;
			}
			tmp = array[i+1];
		}
		if(found)
			erase(pos);
	}

	void elementEmplace(T element)
	{
		bool found = false;
		T tmp = array[0];
ELEMENT_EMPLACE_LOOP:
		for(int i = 0; i < size; i++)
		{
#pragma HLS PIPELINE II=1
			if(tmp == element)
			{
				found = true;
			}
			tmp = array[i+1];
		}

		if(!found)
			emplace(element);
	}

	void fillnReset(T element)
	{
#pragma HLS INLINE off
RESET_LOOP:
		for(int i = 0; i < lastPos; ++i)
		{
#pragma HLS PIPELINE II=1
			array[i] = element;
		}
		size = 0;
		lastPos = 0;
	}

	void plusOne(uint32_t pos)
	{
#pragma HLS INLINE off
		uint32_t tmp = array[pos];
		tmp += 1;
		array[pos] = tmp;
	}

	void operator=(const Vector<T>& vec)
	{
		for(int i = 0; i < vec.size; i++)
		{
#pragma HLS LOOP_TRIPCOUNT min=0 max=64
#pragma HLS PIPELINE II=1
			this->array[i] = vec.array[i];
		}
		this->lastPos = vec.lastPos;
		this->size = vec.size;
	}
};


#endif
