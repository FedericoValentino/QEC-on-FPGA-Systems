#include "RootManager.h"

void RootManager::initializeRoots(Vector<uint32_t> roots)
{
	for(int i = 0; i < roots.getSize(); ++i)
	{
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
	return *size;
}

void RootManager::growSize(uint32_t root)
{
	if(isRoot(root))
	{
		auto size = sizes.find(root);
		*size += 1;
	}

}

bool RootManager::isRoot(uint32_t root)
{
	uint32_t count = 0;
	for(int i = 0; i < roots.getSize(); ++i)
	{
		if(root == roots.at(i))
		{
			if(++count > 1)
				return false;

		}
	}
	return count==1;
}


void RootManager::merge(uint32_t r1, uint32_t r2)
{
	uint32_t newParity = *parity.find(r1) + *parity.find(r2);

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
		sizes.update(r1, *sizes.find(r1) + *sizes.find(r2));
	}
	parity.update(r1, newParity);

	oddRoots.elementErase(r2);

	sizes.erase(r2);
	parity.erase(r2);
	roots.elementErase(r2);

}



















