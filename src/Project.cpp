#include "Project.h"


HashMap decoderLUT;
Decoder decoderUF;

void test(int syndrome[SYN_LEN])
{
	Vector<uint32_t> test;
#pragma HLS ARRAY_PARTITION variable=test.array type=cyclic factor=16
	test.emplace(0);
	test.emplace(1);
	test.emplace(2);
	test.emplace(3);
	test.emplace(4);
	test.emplace(5);
	test.emplace(6);
	test.emplace(7);

	test.erase(4);

}


void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert)
{
#pragma HLS ARRAY_PARTITION variable=DecoderLUT.map type=complete
	if(insert)
	{
		decoderLUT.insert(*correction, syndrome);
	}
	else
	{
		*correction = decoderLUT.retrieve(syndrome);
		if(*correction == 0)
		{
			*correction = decoderUF.decode(syndrome);
		}
	}

}

void testhash(){
	ap_uint<SYN_LEN> test=100;
	HashMap testingMap;
	int x=testingMap.hash(test);
}
