#ifndef PROJECT_H
#define PROJECT_H


#include "LUT/Hashmap.h"
#include "UnionFind/Decoder.h"

static HashMap decoderLUT;
static Decoder decoderUF;

void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert);
void test(int syndrome[SYN_LEN]);

#endif
