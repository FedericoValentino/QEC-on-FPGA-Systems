#ifndef ROOTMANAGER_H
#define ROOTMANAGER_H

#include "../Defines.h"
#include "../Utility/Vector.h"
#include "../Utility/Map.h"


struct ClusterNFO
{
	uint32_t vertex;
	uint32_t NFO;
};

typedef struct ClusterNFO ClusterNFO;

class RootManager
{
private:
	Vector<uint32_t> roots;
	Vector<uint32_t> oddRoots;

	Map<uint32_t, uint32_t> sizes;
	Map<uint32_t, uint32_t> parity;

public:

	void initializeRoots(Vector<uint32_t> roots);

	bool hasOddRoots();

	Vector<uint32_t>* oddRoots_();
};


#endif
