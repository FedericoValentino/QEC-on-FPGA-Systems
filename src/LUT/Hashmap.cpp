#include "Hashmap.h"

ap_uint<SYN_LEN> HashMap::binToDec(int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> sum = 0;
BINARY_TO_DECIMAL_LOOP:
	for(int i = 0; i < SYN_LEN; i++)
	{
#pragma HLS UNROLL
		sum += syndrome[i] * (1<<i);
	}
	return sum;
}

int HashMap::hash(ap_uint<SYN_LEN> synDec)
{
	int i = 0;
	int hash = (synDec+3*i) % MAX_SIZE/2;

	if(this->map[hash].full)
	{
HASH_LOOP:
		for(i = 0; i <= MAX_SIZE/2; i++)
		{
#pragma HLS PIPELINE II=1
			hash = (synDec+3*i) % MAX_SIZE/2;
			if(this->map[hash].syndrome == synDec || !this->map[hash].full)
			{
				return hash;
			}
		}
		return -1;
	}
	else
	{
		return hash;
	}
}


void HashMap::insert(ap_uint<CORR_LEN> correction, int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
    int index = this->hash(synDec);

    if(index!=-1){
    	this->map[index].syndrome = synDec;
    	this->map[index].correction = correction;
    	this->map[index].full = true;
    	++this->lastBlockUsed;
    }


}

ap_uint<CORR_LEN> HashMap::retrieve(int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
	int index = this->hash(synDec);

    if(index!=-1)
    	return this->map[index].correction;
    else
    	return 0;

}
