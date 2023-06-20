#include "Hashmap.h"


ap_uint<SYN_LEN> HashMap::binToDec(bool syndrome[SYN_LEN])
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
#pragma HLS INLINE
	return synDec % MAX_SIZE;
}



void HashMap::insert(ap_uint<CORR_LEN> correction, bool syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
    int index = this->hash(synDec);
    int counter = 0;
    int finalidx = 0;
    bool found = 0;

HASHMAP_INSERT_LOOP:
	while(counter < MAX_SIZE){
#pragma HLS PIPELINE II=1
		counter++;
		if (!map[index].full && !found){
			finalidx = index;
			found=1;
		}
		index++;
		if(index==MAX_SIZE){
			index=0;
		}
	}

    map[finalidx].correction = correction;
    map[finalidx].syndrome = synDec;
    map[finalidx].full = true;


}

bool HashMap::retrieve(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction)
{
	ap_uint<SYN_LEN> synDec = this->binToDec(syndrome);
	int index = this->hash(synDec);
	int counter = 0;
	int finalidx = 0;
	bool found = 0;



HASHMAP_RETRIEVE_LOOP:
	while(counter < MAX_SIZE){
#pragma HLS PIPELINE II=1
		counter++;
		if (synDec == map[index].syndrome){
			finalidx = index;
			found = 1;
		}
		index++;
		if(index==MAX_SIZE){
			index=0;
		}
	}

	if(!found){
		return 0;
	}
	else{
		*correction = map[index].correction;
		return 1;
	}


}
