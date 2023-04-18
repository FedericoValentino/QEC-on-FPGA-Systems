#ifndef DECODER_H
#define DECODER_H


#include "Defines.h"


class Decoder
{
public:
	Lint decode(int syndrome[SYN_LEN]);
};

#endif
