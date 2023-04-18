#include "Hashmap.h"

HashMap decoderLUT;

#pragma HLS TOP
void decoder(int syndrome[SYN_LEN], Lint& correction, bool insert)
{
	if(insert)
	{
		decoderLUT.insert(*correction, syndrome);
	}
	else
	{
		*correction = decoderLUT.retrieve(syndrome);
		if(correction == 0)
		{
			correction = Decoder.decode(syndrome);
		}
	}

}
