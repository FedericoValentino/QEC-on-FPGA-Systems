#include "Hashmap.h"
#include <hls_math.h>

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
	return synDec % MAX_SIZE;
}



void HashMap::insert(ap_uint<CORR_LEN> correction, int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
    int index = this->hash(synDec);
    int counter = 0;
HASHMAP_INSERT_LOOP:
    while(map[index].full && counter < MAX_SIZE)
    {
    	index++;
    	index %= MAX_SIZE;
    	counter++;
    }

    map[index].correction = correction;
    map[index].syndrome = synDec;
    map[index].full = true;


}

bool HashMap::retrieve(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction)
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
	int index = this->hash(synDec);
	int counter = 0;
HASHMAP_RETRIEVE_LOOP:
	while(synDec != map[index].syndrome && counter < MAX_SIZE)
	{
		index++;
	  	index %= MAX_SIZE;
	   	counter++;
    }

	if(counter == MAX_SIZE)
	{
		return 0;
	}
	else
	{
		*correction = map[index].correction;
		return 1;
	}


}
