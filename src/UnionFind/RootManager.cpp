#include "RootManager.h"

void RootManager::initializeRoots(Vector<uint32_t> roots)
{
	for(int i = 0; i < roots.getSize(); ++i)
	{
		uint32_t tmp = roots.at(i);
		this->roots.emplace(tmp);
		this->oddRoots.emplace(tmp);
		this->sizes.add(tmp, 1);
		this->parity.add(tmp, 1);
	}
}
