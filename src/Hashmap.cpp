#include "Hashmap.h"

Lint HashMap::hash(int syndrome[SYN_LEN])
{
	Lint sum = 0;
BINARY_TO_DECIMAL_LOOP:
	for(int i = 0; i < SYN_LEN; i++)
	{
		sum += syndrome[i] * pow(2, i);
	}
	return sum;
}


void HashMap::insert(Lint correction, int syndrome[SYN_LEN])
{
	Lint index = this->hash(syndrome);
	this->map[index].correction = correction;
	this->map[index].full = true;
}

Lint HashMap::retrieve(int syndrome[SYN_LEN])
{
	Lint index = this->hash(syndrome);
	if(this->map[index].full)
	{
		++this->map[index].frequency;
		return this->map[index].correction;
	}
	return 0;
}
