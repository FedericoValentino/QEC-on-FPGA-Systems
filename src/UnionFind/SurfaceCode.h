#ifndef SURFACECODE_H
#define SURFACECODE_H

#include "../Defines.h"
#include "../Utility/Vector.h"
#include "../Utility/Map.h"


struct Edge
{
	uint32_t u;
	uint32_t v;

	bool operator==(const Edge& other) const
	{
		return (u == other.u && v == other.v);
	}
};


struct Coord
{
	uint32_t x;
	uint32_t y;
};

class SurfaceCode
{
private:
	uint32_t L = D;

public:

	uint32_t to_vertex_index(int row, int col)
	{
		if(row < 0)
		{
			row += L;
		}
		else if(row >= L)
		{
			row -= L;
		}
		if(col < 0)
		{
			col += L;
		}
		else if(col >= L)
		{
			col -= L;
		}


		return L * row + col;
	}

	uint32_t vertex_connection_count(uint32_t vertex)
	{
		return 4;
	}

	Vector<uint32_t> vertex_connections(uint32_t v)
	{
		int row = v/L;
		int col = v%L;

		Vector<uint32_t> vector;
#pragma HLS ARRAY_PARTITION variable=vector.array type=complete

		uint32_t idx1 = to_vertex_index(row - 1, col);
		vector.emplace(idx1);
		uint32_t idx2 = to_vertex_index(row + 1, col);
		vector.emplace(idx2);
		uint32_t idx3 = to_vertex_index(row, col - 1);
		vector.emplace(idx3);
		uint32_t idx4 = to_vertex_index(row, col + 1);
		vector.emplace(idx4);

		return vector;
	}

	uint32_t num_vertices()
	{
		return L*L;
	}

	uint32_t num_edges()
	{
		return 2*L*L;
	}

	uint32_t edge_idx(Edge e)
	{
		return to_edge_idx(L, e);
	}

	uint32_t to_edge_idx(uint32_t L, Edge e)
	{
		if(is_horizontal(L, e))
				{
					Coord tmp=vertex_to_coord(L,left(L,e));
					return L*(L+tmp.x)+tmp.y;
				}
				else
				{
					return lower(L,e);
				}
	}

	bool is_horizontal(uint32_t L, Edge e)
	{
		return ((e.v - e.u) == 1) || ((e.v - e.u) == (L - 1));
	}

	uint32_t left(uint32_t L, Edge e)
	{
		if((e.v - e.u) == 1)
		{
			return e.u;
		}
		return e.v;
	}

	uint32_t lower(uint32_t L, Edge e)
	{
		if((e.v - e.u) == L)
		{
			return e.v;
		}
		return e.u;
	}

	Coord vertex_to_coord(const uint32_t L, const uint32_t vidx)
	{
		return {vidx / L, vidx % L};
	}
};

#endif
