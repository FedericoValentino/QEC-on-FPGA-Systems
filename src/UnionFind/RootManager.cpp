#include "RootManager.h"

uint32_t RootManager::findSize(int Vertex)
	{
		for(int i = 0; i < this->sizes.getSize(); i++)
		{
			if(this->sizes.at(i).vertex == Vertex)
			{
				return this->sizes.at(i).NFO;
			}
		}
		return -1;
	}

uint32_t RootManager::findParity(int Vertex)
	{
		for(int i = 0; i < this->parity.getSize(); i++)
		{
			if(this->parity.at(i).vertex == Vertex)
			{
				return this->parity.at(i).NFO;
			}
		}
		return -1;
	}


void RootManager::initializeRoots(Vector<uint32_t> roots)
{
	for(int i = 0; i < roots.getSize(); ++i)
	{
		uint32_t tmp = roots.at(i);
		this->roots.emplace(tmp);
		this->oddRoots.emplace(tmp);
		this->sizes.get(i)->vertex = tmp;
		this->sizes.get(i)->NFO = 1;
		this->parity.get(i)->vertex = tmp;
		this->parity.get(i)->NFO = 1;
	}
}
