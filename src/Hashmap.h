#include <math.h>
#include "Defines.h"


struct entry
{
    Lint syndrome;
	Lint correction;
	bool full = false;

	size_t frequency = 0;
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
