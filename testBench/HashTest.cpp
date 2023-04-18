#include "../src/Project.cpp"
#include <assert.h>


int main()
{
	int syndrome[SYN_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,0};
	Lint correction = 20;

	decoderTop(syndrome, &correction, true);

	correction = 19;

	assert(decoderLUT.retrieve(syndrome) == 20);
}