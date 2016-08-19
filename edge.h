//	--------------------------------------------------------
//	EDGE.H
//	--------------------------------------------------------
//	Contains edges that rotate themmselves using fancy memory tricks
//	Introduced by Guibas and Stolfi (1985)
//	(Adapted from code presented on Paul Heckbert's webpage)
//	http://www.cs.cmu.edu/afs/andrew/scs/cs/15-463/2001/pub/src/a2/quadedge.html
//	--------------------------------------------------------

#ifndef EDGE_H
#define EDGE_H

//	--------------------------------------------------------
//	Include
//	--------------------------------------------------------

#include <SFML/Graphics.hpp>
#include <memory>

//	--------------------------------------------------------
//	Forward declarations because C++
//	--------------------------------------------------------

class Edge;
class QuadEdge;

//	--------------------------------------------------------
//	The Vert class
//	--------------------------------------------------------

// This extends the SFML class, which, yes, costs some memory for computation
// This is a nominal thing so I don't get a headache; I could rewrite the code to use just int tuples
// (And probably should)
class Vert //: public sf::Vertex
{
private:
	Edge*												edge_;
	float												x_;
	float												y_;
public:
	//Vert(float x, float y);
	Vert(float x, float y);

	Edge*												edge()									{ return edge_; };
	void												AddEdge(Edge* edge)						{ edge_ = edge; };

	//float												x()										{ return position.x; };
	//float												y()										{ return position.y; };
	//float												lengthsquared()							{ return position.x * position.x + position.y * position.y; };

	int													x()										{ return x_; };
	int													y()										{ return y_; };
	int													lengthsquared()							{ return x_ * x_ + y_ * y_; };
	
	//sf::Vector2f										getPosition()							{ return position; };
};

//	--------------------------------------------------------
//	Constructor
//	--------------------------------------------------------

/*
Vert::Vert(float x, float y)
{
	position = sf::Vector2f(x, y);
}
*/

Vert::Vert(float x, float y) : x_(x), y_(y)
{

}

//	--------------------------------------------------------
//	The Edge class
//	--------------------------------------------------------

// Notably this guy points to only one Vert and knows its own index and next edge about its origin Vert
class Edge
{
private:
	Vert*												origin_;
	int													index_;
	Edge*												next_;

	friend QuadEdge;

public:
	Edge(Vert* _origin);
	Edge();
	~Edge();

	bool												draw;

	int index()											{ return index_; };

	// An Edge object has four primitive algebraic operations; see Guibas and Stolfi

	Edge* Rot();
	Edge* InvRot();
	Edge* Sym();
	Edge* Onext()										{ return next_; };

	// These guys can be derived from the four primitive operations

	Edge* Oprev()										{ return Rot()->Onext()->Rot(); };
	Edge* Dnext()										{ return Sym()->Onext()->Sym(); };
	Edge* Dprev()										{ return InvRot()->Onext()->InvRot(); };
	Edge* Lnext()										{ return InvRot()->Onext()->Rot(); };
	Edge* Lprev()										{ return Onext()->Sym(); };
	Edge* Rnext()										{ return Rot()->Onext()->InvRot(); };
	Edge* Rprev()										{ return Sym()->Onext(); };

	// Accessors and mutators

	Vert* origin()										{ return origin_; };
	Vert* destination()									{ return Sym()->origin(); };
		
	void setNext(Edge* next)							{ next_ = next; };
	void setIndex(int index)							{ index_ = index; };
	void setOrigin(Vert* org);
	void setOrigin(sf::Vector2f org);
	void setDestination(Vert* dest);

	// Uses raw pointer because it's returning the array member of a QuadEdge
	static Edge* Make(std::vector<QuadEdge*>& list);
};

//	--------------------------------------------------------
//	Constructors and destructors
//	--------------------------------------------------------

Edge::Edge()
{
}

Edge::~Edge()
{
}

Edge::Edge(Vert* _origin) : origin_(_origin)
{
}

//	--------------------------------------------------------
//	Member functions too complex to inline
//	--------------------------------------------------------

Edge* Edge::Rot()
{
	return (index_ < 3) ? (this + 1) : (this - 3);
};

Edge* Edge::InvRot()
{
	return (index_ > 0) ? (this - 1) : (this + 3);
}	

Edge* Edge::Sym()
{
	return (index_ < 2) ? (this + 2) : (this - 2);
}

void Edge::setOrigin(Vert* origin)
{
	origin->AddEdge(this);
	origin_ = origin;
	draw = true;
}

void Edge::setOrigin(sf::Vector2f origin)
{
	origin_ = new Vert(origin.x, origin.y);
	origin_->AddEdge(this);
	draw = true;
}

void Edge::setDestination(Vert* dest)
{
	Edge* sym = Sym();
	dest->AddEdge(sym);
	sym->setOrigin(dest);
}

//	--------------------------------------------------------
//	Uh, this guy
//	--------------------------------------------------------

void Splice(Edge* a, Edge* b)
{
	// This remains unintelligible to me
	// See Guibas and Stolfi, also Heckbert's code

	Edge* alpha = a->Onext()->Rot();
	Edge* beta = b->Onext()->Rot();

	Edge* t1 = b->Onext();
	Edge* t2 = a->Onext();
	Edge* t3 = beta->Onext();
	Edge* t4 = alpha->Onext();

	a->setNext(t1);
	b->setNext(t2);
	alpha->setNext(t3);
	beta->setNext(t4);
}

//	--------------------------------------------------------

#endif