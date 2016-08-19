//	--------------------------------------------------------
//	TOPOLOGY.H
//	--------------------------------------------------------
//	Contains the divide-and-conquer Delaunay alogorithm, as well as helper functions and a class to keep track of relevant data
//	Taken from Guibas and Stolfi (1985)
//	--------------------------------------------------------

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

//	--------------------------------------------------------
//	Include files	
//	--------------------------------------------------------

#include "edge.h"
#include "linal.h"
#include "quadedge.h"
#include "math.h"
#include <tuple>
#include <vector>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <unordered_set>

//	--------------------------------------------------------
//	Some typedefs for readability
//	--------------------------------------------------------

typedef std::vector<Edge*>					EdgeList;
typedef std::vector<Vert*>					PointsList;
typedef std::vector<QuadEdge*>				QuadList;
typedef std::tuple<EdgeList, EdgeList>		EdgePartition;
typedef std::tuple<PointsList, PointsList>	PointsPartition;

//	--------------------------------------------------------
// The class, creatively named, that will house our methods
//	--------------------------------------------------------

class Delaunay
{
private:
	// Components of the graph
	PointsList								vertices_;
	QuadList								edges_;

	// Helper to create a bunch of random vertices
	void									GenerateRandomVerts(int n);

	// Helper to cut the array of points in half
	PointsPartition							SplitPoints(const PointsList& points);

	// Functions that create or remove edges
	Edge*									MakeEdgeBetween(int a, int b, const PointsList& points);
	Edge*									Connect(Edge* a, Edge* b);
	void									Kill(Edge* edge);

	// Functions for generating primitive shapes that we'll merge together
	EdgePartition							LinePrimitive(const PointsList& points);
	EdgePartition							TrianglePrimitive(const PointsList& points);

	// Refactored subroutines to make the big algorithm more readable
	Edge*									LowestCommonTangent(Edge*& left_inner, Edge*& right_inner);
	Edge*									LeftCandidate(Edge* base_edge);
	Edge*									RightCandidate(Edge* base_edge);
	void									MergeHulls(Edge*& base_edge);

	// The main attraction
	EdgePartition							Triangulate(const PointsList& points);

public:
	// Constructor
	Delaunay(int n);

	// Triangulate the vertices
	QuadList								GetTriangulation();
	
	// Build the Voronoi diagram corresponding to the triangulation
	QuadList								GetVoronoi();

	// Build a minimum spanning tree across the vertices
	EdgeList								GetMST();
};

//	--------------------------------------------------------
//	Constructor
//	--------------------------------------------------------

Delaunay::Delaunay(int n)
{
	// For the moment, we generate the vertices
	edges_ = QuadList();
	GenerateRandomVerts(n);
}

void Delaunay::GenerateRandomVerts(int n)
{
	// Generate a field of random vertices for debug/demonstration

	srand(time(NULL));

	std::vector<std::vector<int>> buffer;

	// Build a buffer list
	for (int i = 0; i < n; i++)
	{
		std::vector<int> xy = { rand() % 512, rand() % 512 };
		buffer.push_back(xy);
	}

	// Sort it lexicographically; we need this step
	std::sort(buffer.begin(), buffer.end());
	buffer.erase(std::unique(buffer.begin(), buffer.end()), buffer.end());

	// Turn it into Verts for the convenience of our algorithm
	for (int i = 0; i < buffer.size(); i++)
	{
		vertices_.push_back(new Vert(buffer[i][0], buffer[i][1]));
	}
}

//	--------------------------------------------------------
//	Functions for managing the QuadEdges
//	--------------------------------------------------------

void Delaunay::Kill(Edge* edge)
{
	// Fix the local mesh
	Splice(edge, edge->Oprev());
	Splice(edge->Sym(), edge->Sym()->Oprev());

	// Free the quad edge that the edge belongs to
	QuadEdge* raw = (QuadEdge*)(edge - (edge->index()));
	edges_.erase(std::remove(edges_.begin(), edges_.end(), raw));
	delete raw;
}

//	--------------------------------------------------------
//	Helper functions
//	--------------------------------------------------------

// Split the list of vertices in the center
// Thsi relies on the assumption that they're ordered lexicographically
PointsPartition Delaunay::SplitPoints(const PointsList& points)
{
	int halfway = (points.size() / 2);

	PointsList left(points.begin(), points.begin() + halfway);
	PointsList right(points.begin() + halfway, points.end());

	return PointsPartition(left, right);
}

// Creates an edge between the vertices at the given indices
// This is accomplished by creating a new QuadEdge, setting its 0th edge to originate at points[a] and setting its 2nd edge to originate at points[b]
Edge* Delaunay::MakeEdgeBetween(int a, int b, const PointsList& points)
{
	// Create the QuadEdge and return the memory address of its 0th edge
	Edge* e = Edge::Make(edges_);
	
	// Set it to originate from the Vert at index a
	e->setOrigin(points[a]);

	// Set its twin to originate from the Vert at index b
	Vert* points_b = points[b];
	e->setDestination(points_b);

	// Return a pointer to our new edge
	return e;
}

// Connects the ends of two edges to form a coherently oriented triangle
Edge* Delaunay::Connect(Edge* a, Edge* b)
{
	// See Guibas and Stolfi for more

	// Create a new QuadEdge and return the memory address of its 0th edge
	Edge* e = Edge::Make(edges_);

	// Set it to originate at the end point of b
	e->setOrigin(a->destination());

	// Set it to end at the beginning of a, thus giving it a coherent orientation
	e->setDestination(b->origin());

	// Perform splice operations -- I'm still not quite sure why
	Splice(e, a->Lnext());
	Splice(e->Sym(), b);

	// Return a pointer to our new edge
	return e;
}

// Connects two vertices into an edge
EdgePartition Delaunay::LinePrimitive(const PointsList& points)
{
	// Build a line primitive
	// And return it twice?
	Edge* e = MakeEdgeBetween(0, 1, points);
	Edge* e_sym = e->Sym();
	return EdgePartition({ e }, { e_sym });
}

// Connects three vertices into a coherently oriented triangle
EdgePartition Delaunay::TrianglePrimitive(const PointsList& points)
{
	// Build our first two edges here
	Edge* a = MakeEdgeBetween(0, 1, points);
	Edge* b = MakeEdgeBetween(1, 2, points);

	// Do the splice thing; I'm not sure why
	Splice(a->Sym(), b);

	// We want a consistent face orientation, so determine which way we're going here
	if (CCW(points[0], points[1], points[2]))
	{
		Edge* c = Connect(b, a);
		return EdgePartition({ a }, { b->Sym() });
	}
	else if (CCW(points[0], points[2], points[1]))
	{
		Edge* c = Connect(b, a);
		return EdgePartition({ c->Sym() }, { c });
	}
	else
	{
		// The points are collinear
		return EdgePartition({ a }, { b->Sym() });
	}
}

Edge* Delaunay::LowestCommonTangent(Edge*& left_inner, Edge*& right_inner)
{
	// Compute the lower common tangent of the two halves
	// Note the pointer references; we want to keep track of where the new inner edges end up

	// Until we can't do it anymore, take turns rotating along the hulls of the two shapes we're connecting
	while (true)
	{
		if (LeftOf(left_inner, right_inner->origin()))
		{
			left_inner = left_inner->Lnext();
		}
		else if (RightOf(right_inner, left_inner->origin()))
		{
			right_inner = right_inner->Rprev();
		}
		else
		{
			break;
		}
	}

	// Create the base edge once we hit the bottom
	Edge* base_edge = Connect(right_inner->Sym(), left_inner);
	return base_edge;
}

Edge* Delaunay::LeftCandidate(Edge* base_edge)
{
	// Picks out a "candidate" edge from the left half of the domain
	Edge* left_candidate = base_edge->Sym()->Onext();

	if (Valid(left_candidate, base_edge))
	{
		while (InCircle(base_edge->destination(), base_edge->origin(), left_candidate->destination(), left_candidate->Onext()->destination()))
		{
			Edge* t = left_candidate->Onext();
			Kill(left_candidate);
			left_candidate = t;
		}
	}

	return left_candidate;
}

Edge* Delaunay::RightCandidate(Edge* base_edge)
{
	// Picks out a "candidate" edge from the right half of the domain
	Edge* right_candidate = base_edge->Oprev();

	if (Valid(right_candidate, base_edge))
	{
		while (InCircle(base_edge->destination(), base_edge->origin(), right_candidate->destination(), right_candidate->Oprev()->destination()))
		{
			Edge* t = right_candidate->Oprev();
			Kill(right_candidate);
			right_candidate = t;
		}
	}

	return right_candidate;
}

void Delaunay::MergeHulls(Edge*& base_edge)
{
	// Zip up the two halves of the hull once we've found the base edge
	while (true)
	{
		// Get our candidate edges (really becaue we care about their vertices)
		Edge* left_candidate = LeftCandidate(base_edge);
		Edge* right_candidate = RightCandidate(base_edge);

		if (!Valid(left_candidate, base_edge) && !Valid(right_candidate, base_edge))
		{
			// If neither is valid, we have nothing more to do because we've reached the top
			break;
		}
		else if (	!Valid(left_candidate, base_edge) ||
					InCircle(left_candidate->destination(), left_candidate->origin(), right_candidate->origin(), right_candidate->destination()))
		{
			// Otherwise, if we can rule out the left guy, connect the right edge to the base and set the new base edge
			// This ruling out comes either from creating an invalid hypothetical triangle or from being beneath the base edge
			base_edge = Connect(right_candidate, base_edge->Sym());
		}
		else
		{
			// If we can't do that, then the left edge must be valid and we connect it to the base and set the new base edge
			base_edge = Connect(base_edge->Sym(), left_candidate->Sym());
		}
	}
}

//	--------------------------------------------------------
//	The main attraction
//	--------------------------------------------------------

EdgePartition Delaunay::Triangulate(const PointsList& points)
{
	// Returns the left and right hulls created by triangulating
	// The ultimate value we care about is actually the edges_ member of the Delaunay class
	// This is recursive because divide-and-conquer is a good way to do this
	// See Guibas and Stolfi

	// (Also, we have to assume that the point set we're given is sorted lexicographically)

	/* Terminal cases */
	
	if (points.size() == 2)
	{
		return LinePrimitive(points);
	}
	if (points.size() == 3)
	{
		return TrianglePrimitive(points);
	}

	// Once we survive the terminal-case filter, split up the points
	PointsPartition partition = SplitPoints(points);

	EdgePartition left = Triangulate(std::get<0>(partition));
	EdgePartition right = Triangulate(std::get<1>(partition));

	/* This part of the code is only reachable once we terminate, at which point the vectors are singleton sets */
	
	// Get the inner "inner" edges
	Edge* right_inner = std::get<0>(right)[0];
	Edge* left_inner = std::get<1>(left)[0];

	// Get the initial "outer" edges
	Edge* left_outer = std::get<0>(left)[0];
	Edge* right_outer = std::get<1>(right)[0];

	// Get the lowest common tangent from our initial inner edges
	Edge* base_edge = LowestCommonTangent(left_inner, right_inner);

	// Correct the base edge
	if (left_inner->origin() == left_outer->origin())
	{
		left_outer = base_edge->Sym();
	}
	if (right_inner->origin() == right_outer->origin())
	{
		right_outer = base_edge;
	}

	// Finish the merge operation by "zipping up the gap"
	// i.e. connect things if they don't violate the Delaunay criterion
	MergeHulls(base_edge);

	// Return the outer edges, because they'll be the new inner edges when we do a higher-level merge
	return EdgePartition({ left_outer }, { right_outer });
}

QuadList Delaunay::GetTriangulation()
{
	// Wrapper for the triangulation function
	// This should make it less confusing to call Triangulate with the right vertex list
	EdgePartition tuple = Triangulate(vertices_);
	return edges_;
}

QuadList Delaunay::GetVoronoi()
{
	for (auto i = edges_.begin(); i != edges_.end(); i++)
	{
		Edge* e = (*i)->edges;

		// If we're not on the exterior
		if (CCW(e[0].origin(), e[0].destination(), e[0].Onext()->destination())
			&& CCW(e[0].origin(), e[0].Oprev()->destination(), e[0].destination()))
		{
			e[1].setOrigin(Circumcenter(e[0].origin(), e[0].destination(), e[0].Onext()->destination()));
			e[3].setOrigin(Circumcenter(e[0].origin(), e[0].Oprev()->destination(), e[0].destination()));
		}
	}

	return edges_;
}

EdgeList Delaunay::GetMST()
{
	// Okay, so we're not weighting it right now
	// I can't really think of a reason to
	EdgeList mst;
	PointsList queue;
	std::map<Vert*, int> distance;

	for (auto i = vertices_.begin(); i != vertices_.end(); i++)
	{
		distance[*i] = -1;
	}

	// So we just do a depth-first search on the linked list
	Vert* root = vertices_[0];
	queue.push_back(root);
	distance[root] = 0;

	while (queue.size() > 0)
	{
		Vert* current = queue.back();
		queue.pop_back();

		// Check out my syntax skills
		for (Edge* e = current->edge(); e != current->edge()->Oprev(); e = e->Onext())
		{
			Vert* dest = e->destination();
			if (distance[dest] == -1)
			{
				distance[dest] = distance[current] + 1;
				queue.push_back(dest);
				mst.push_back(e);
			}
		}
	} 



	return mst;
}

//	--------------------------------------------------------

#endif