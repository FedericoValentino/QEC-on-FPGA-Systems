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
    Entry map[MAX_SIZE/2];

    unsigned int lastBlockUsed = 0;

	ap_uint<SYN_LEN> binToDec(int syndrome[SYN_LEN]);

	int hash(ap_uint<SYN_LEN> synDec);

public:

	void insert(ap_uint<CORR_LEN> correction, int syndrome[SYN_LEN]);

	ap_uint<CORR_LEN> retrieve(int syndrome[SYN_LEN]);

};

#endif
