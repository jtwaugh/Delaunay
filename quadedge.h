//	--------------------------------------------------------
//	QUADEDGE.H
//	--------------------------------------------------------
//	Contains a data structure for representing the algebra of edges on an orientable manifold.
//	Introduced by Guibas and Stolfi (1985)
//	(Adapted from code presented on Paul Heckbert's webpage)
//	http://www.cs.cmu.edu/afs/andrew/scs/cs/15-463/2001/pub/src/a2/quadedge.html
//	--------------------------------------------------------

#ifndef QUADEDGE_H
#define QUADEDGE_H

//	--------------------------------------------------------
//	Include files
//	--------------------------------------------------------

#include "edge.h"
#include <memory>

//	--------------------------------------------------------
//	The class
//	--------------------------------------------------------

class QuadEdge
{
public:
	Edge edges[4];
	QuadEdge();
};

//	--------------------------------------------------------
//	Constructor
//	--------------------------------------------------------

QuadEdge::QuadEdge()
{
	// Make sure the edges know their own indices for memory magic
	edges[0].setIndex(0);
	edges[1].setIndex(1);
	edges[2].setIndex(2);
	edges[3].setIndex(3);

	// By default, don't render them
	edges[0].draw = false;
	edges[1].draw = false;
	edges[2].draw = false;
	edges[3].draw = false;

	// Set them up to point to each other
	edges[0].setNext((edges + 0));
	edges[1].setNext((edges + 3));
	edges[2].setNext((edges + 2));
	edges[3].setNext((edges + 1));
}

//	--------------------------------------------------------
//	This function depends on the definition of QuadEdge, so has to sit here
//	--------------------------------------------------------

Edge* Edge::Make(std::vector<QuadEdge*>& list)
{
	// To create a new Edge, make sure to call this function
	// Create a QuadEdge to hold our new Edge and make aure we keep trck of it
	list.push_back(new QuadEdge());
	// Return the index of the 0th edge
	return list.back()->edges;
}

//	--------------------------------------------------------

#endif