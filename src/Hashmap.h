#ifndef HASHMAP_H
#define HASHMAP_H


#include "Defines.h"


struct entry
{
    Lint syndrome;
	Lint correction;
	bool full = false;

	unsigned int frequency = 0;
};

typedef struct entry Entry;


class HashMap
{
private:
	Entry map[MAX_SIZE];

	Lint hash(int syndrome[SYN_LEN]);

public:

	void insert(Lint correction, int syndrome[SYN_LEN]);

	Lint retrieve(int syndrome[SYN_LEN]);




};

#endif
