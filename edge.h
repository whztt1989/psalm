/*!
*	@file 	edge.h
*	@brief	Edge class
*/

#ifndef EDGE_H
#define EDGE_H

#include <iostream>

/*!
*	@class edge
*	@brief Data for an oriented edge (u,v) of the mesh
*/

class edge
{
	public:
		edge();
		edge(size_t u, size_t v);

		size_t u;	///< Index of start vertex
		size_t v;	///< Index of end vertex

		size_t f1;
		size_t f2;

		bool operator<(const edge& b) const;
		bool operator==(const edge& b) const;
};

#endif
