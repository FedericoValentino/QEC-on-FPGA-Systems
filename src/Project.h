#ifndef PROJECT_H
#define PROJECT_H


#include "LUT/Hashmap.h"
#include "UnionFind/Decoder.h"



void decoderTop(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction_in, ap_uint<CORR_LEN>* correction_out, bool insert);

#endif
