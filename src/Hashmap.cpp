#include "Hashmap.h"

ap_uint<SYN_LEN> HashMap::binToDec(int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> sum = 0;
BINARY_TO_DECIMAL_LOOP:
	for(int i = 0; i < SYN_LEN; i++)
	{
		sum += syndrome[i] * (1<<i);
	}
	return sum;
}

unsigned int HashMap::hash(ap_uint<SYN_LEN> synDec)
{
	ap_uint<32> mod = synDec % (MAX_SIZE/2);
	return mod.to_uint();
}


void HashMap::insert(ap_uint<CORR_LEN> correction, int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
    unsigned int index = this->hash(synDec);

    while(this->map[index].full)
    {
    	index += 3;
    	if(index == MAX_SIZE/2)
    	{
    		index = 0;
    	}
    }

    this->map[index].syndrome = synDec;
    this->map[index].correction = correction;
    this->map[index].full = true;

    ++this->lastBlockUsed;

}

ap_uint<CORR_LEN> HashMap::retrieve(int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
	unsigned int index = this->hash(synDec);

    for(unsigned int i = index; i <= MAX_SIZE/2; i += 3)
    {
    	if(i == MAX_SIZE/2)
    	{
    		i = 0;
    	}

    	if(this->map[i].syndrome == synDec)
    	{
    		return this->map[i].correction;
    	}
    }

    return 0;
}
