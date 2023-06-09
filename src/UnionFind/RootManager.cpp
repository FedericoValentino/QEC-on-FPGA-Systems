#include "RootManager.h"

void RootManager::initializeRoots(Vector<uint32_t> roots)
{
ROOT_INITIALIZE:
	for(int i = 0; i < roots.getSize(); ++i)
	{
#pragma HLS PIPELINE
		uint32_t tmp = roots.at(i);
		this->roots.elementEmplace(tmp);
		this->oddRoots.elementEmplace(tmp);
		this->sizes.add(tmp, 1);
		this->parity.add(tmp, 1);
	}
}

bool RootManager::hasOddRoots()
{
	return oddRoots.getSize() > 0;
}

Vector<uint32_t>* RootManager::oddRoots_()
{
	return &oddRoots;
}

uint32_t RootManager::size(uint32_t root)
{
	if(!isRoot(root))
	{
		return 0;
	}
	auto size = sizes.find(root);
	return size;
}

void RootManager::growSize(uint32_t root)
{
	if(isRoot(root))
	{
		auto size = sizes.find(root);
		size += 1;
		sizes.update(root, size);
	}

}

bool RootManager::isRoot(uint32_t root)
{
	uint32_t count = 0;
	uint32_t tmp = roots.at(0);
IS_ROOT:
	for(int i = 0; i < roots.getSize(); ++i)
	{
		if(root == tmp)
		{
			++count;
		}
		tmp = roots.at(i+1);
	}
	return count==1;
}


void RootManager::merge(uint32_t r1, uint32_t r2)
{
	uint32_t p1 = parity.find(r1);
	uint32_t p2 = parity.find(r2);
	uint32_t newParity = p1 + p2;

	if((newParity % 2)== 1)
	{
		oddRoots.elementEmplace(r1);
	}
	else
	{
		oddRoots.elementErase(r1);
	}


	if(isRoot(r2))
	{
		uint32_t s1 = sizes.find(r1);
		uint32_t s2 = sizes.find(r2);
		sizes.update(r1, s1+s2);
	}
	parity.update(r1, newParity);

	EraseFromAll(r2);

}

void RootManager::EraseFromAll(uint32_t root)
{
#pragma HLS DATAFLOW
	oddRoots.elementErase(root);
	sizes.erase(root);
	parity.erase(root);
	roots.elementErase(root);
}

void RootManager::clear()
{
#pragma HLS DATAFLOW
	roots.fillnReset(0);
	oddRoots.fillnReset(0);

	sizes.reset();
	parity.reset();
}


















