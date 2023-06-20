#ifndef HASHMAP_H
#define HASHMAP_H


#include "../Defines.h"


struct entry
{
    ap_uint<SYN_LEN> syndrome;
	ap_uint<CORR_LEN> correction;
	bool full = false;

};

typedef struct entry Entry;


class HashMap
{
private:

    unsigned int lastBlockUsed = 0;


public:

    ap_uint<SYN_LEN> binToDec(bool syndrome[SYN_LEN]);

	int hash(ap_uint<SYN_LEN> synDec);

	Entry map[MAX_SIZE];

	void insert(ap_uint<CORR_LEN> correction, bool syndrome[SYN_LEN]);

	bool retrieve(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction);

};

#endif
