#include <math.h>

#define MAX_SIZE 2000000

//code definition

#define K 1
#define D 5
#define N 41

#define SYN_LEN 40

struct entry
{
	long long unsigned int correction;
	bool full = false;

	size_t frequency = 0;
};

typedef struct entry Entry;

typedef long long unsigned int Lint;


class HashMap
{
private:
	Entry map[MAX_SIZE];

	Lint hash(int syndrome[SYN_LEN]);

public:

	void insert(Lint correction, int syndrome[SYN_LEN]);

	Lint retrieve(int syndrome[SYN_LEN]);




};
