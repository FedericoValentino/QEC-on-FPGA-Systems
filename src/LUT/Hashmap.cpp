#include "Hashmap.h"

ap_uint<SYN_LEN> HashMap::binToDec(int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> sum = 0;
BINARY_TO_DECIMAL_LOOP:
	for(int i = 0; i < SYN_LEN; i++)
	{
#pragma HLS UNROLL factor=SYN_LEN
		sum += syndrome[i] * (1<<i);
	}
	return sum;
}

int HashMap::hash(ap_uint<SYN_LEN> synDec, int i)
{
	int hash;
	do{
		hash = (synDec+3*i) % MAX_SIZE/2;
		++i;
		if(this->map[hash].syndrome==synDec)
			return hash;
		if(i==MAX_SIZE)
			return -1;
	}while(this->map[hash].full);

	return hash;
}


void HashMap::insert(ap_uint<CORR_LEN> correction, int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
    int index = this->hash(synDec,0);

    if(index!=-1){
    	this->map[index].syndrome = synDec;
    	this->map[index].correction = correction;
    	this->map[index].full = true;
    	++this->lastBlockUsed;
    }
   //it has to be managed the case in which we want to insert but no space available TODO


}

ap_uint<CORR_LEN> HashMap::retrieve(int syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
	int index = this->hash(synDec,0);

    if(index!=-1)
    	return this->map[index].correction;
    else
    	return 0;

}
