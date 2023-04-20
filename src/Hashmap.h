#ifndef HASHMAP_H
#define HASHMAP_H


#include "Defines.h"


struct entry
{
    Lint syndrome;
	Lint correction;
	bool full = false;

    struct entry* right;
    struct entry* left;

};

typedef struct entry Entry;


class HashMap
{
private:
	Entry blocks[MAX_SIZE];
    Entry* map[MAX_SIZE/2] = {nullptr};

    unsigned int lastBlockUsed = 0;

	Lint hash(int syndrome[SYN_LEN]);

public:

	void insert(Lint correction, int syndrome[SYN_LEN]);

	Lint retrieve(int syndrome[SYN_LEN]);




};

#endif
