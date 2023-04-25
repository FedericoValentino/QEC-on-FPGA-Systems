#ifndef ROOTMANAGER_H
#define ROOTMANAGER_H

#include "../Defines.h"
#include "../Utility/Vector.h"


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

	Vector<ClusterNFO> sizes;
	Vector<ClusterNFO> parity;

public:

	uint32_t findSize(int Vertex);
	uint32_t findParity(int Vertex);

	void initializeRoots(Vector<uint32_t> roots);
};


#endif
