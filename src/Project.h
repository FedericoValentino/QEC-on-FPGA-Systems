#ifndef PROJECT_H
#define PROJECT_H


#include "LUT/Hashmap.h"
#include "UnionFind/Decoder.h"



void decoderTop(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert);

#endif
