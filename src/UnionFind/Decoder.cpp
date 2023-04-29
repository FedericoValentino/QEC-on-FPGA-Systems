#include "Decoder.h"

ap_uint<CORR_LEN> Decoder::decode(int syndrome[SYN_LEN])
{
	Vector<uint32_t> syndrome_vertices;

	for(uint32_t i = 0; i < SYN_LEN; ++i)
	{
		if(syndrome[i])
		{
			syndrome_vertices.emplace(i);
		}
	}

	init_cluster(syndrome_vertices);

	while(mngr.hasOddRoots())
	{
		for(int i = 0; i < mngr.oddRoots_()->getSize(); ++i)
		{
			grow(*mngr.oddRoots_()->get(i));
		}
	}



	return 0;
}

void Decoder::init_cluster(Vector<uint32_t> roots)
{
	mngr.initializeRoots(roots);
	for(uint32_t i = 0; i < roots.getSize(); ++i)
	{
		Vector<uint32_t> Border;
		Border.emplace(roots.at(i));
		border_vertices.add(roots.at(i), Border);
	}

	for(uint32_t i = 0; i < roots.getSize(); ++i)
	{
		root_of_vertex.insert(i, i);
	}
}


void Decoder::grow(uint32_t root)
{
	Vector<uint32_t>* borders = border_vertices.find(root);
	for(int i = 0; i < borders->getSize(); i++)
	{
	}
}


