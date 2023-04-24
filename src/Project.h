#ifndef PROJECT_H
#define PROJECT_H


#include "Hashmap.h"
#include "Decoder.h"

extern HashMap decoderLUT;
extern Decoder decoderUF;

void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert);


#endif