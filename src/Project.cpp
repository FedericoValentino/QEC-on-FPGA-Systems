#include "Hashmap.h"
#include "Decoder.h"

HashMap decoderLUT;
Decoder decoderUF;

#pragma HLS TOP
void decoderTop(int syndrome[SYN_LEN], Lint* correction, bool insert)
{
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
