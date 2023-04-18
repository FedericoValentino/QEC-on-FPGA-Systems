#include "../src/Decoder.cpp"


int main()
{
	int syndrome[SYN_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,0};
	Lint correction = 20;

	decoder(syndrome, &correction, false);

	correction = 19;

	assert(decoderLUT.retrieve(syndrome) == 20);
}
