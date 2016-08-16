// Delaunay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "topology.h"
#include <iostream>

void DrawEdge(Edge* e, sf::RenderWindow& window)
{
	Edge* sym = e->Sym();
	Vertex* destination = e->destination();
	Vertex org = *(e->origin());
	Vertex dest = *(e->destination());

	sf::VertexArray v(sf::Lines, 2);

	v[0].position = sf::Vector2f(org.x(), org.y());
	v[1].position = sf::Vector2f(dest.x(), dest.y());

	window.draw(v);

	//std::cout << "org: " << org.x() << ", " << org.y() << "; dest: " << dest.x() << ", " << dest.y() << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<QuadEdge*> quads;
	Delaunay test = Delaunay(quads);

	EdgeList edges = test.GetTriangulation();

	sf::RenderWindow window(sf::VideoMode(512, 512), "Delaunay Triangulator");

	std::vector<std::tuple<sf::Vector2f, sf::Vector2f>> quads_debug;

	for (int i = 0; i < quads.size(); i++)
	{
		quads_debug.push_back(std::make_tuple(quads[i]->edges[0].origin()->getPosition(), quads[i]->edges[2].origin()->getPosition()));
		//quads_debug.push_back(std::make_tuple(quads[i]->edges[1].origin()->getPosition(), quads[i]->edges[3].origin()->getPosition()));
	}

	while (window.isOpen())
	{
		window.clear();

		for (auto i = quads.begin(); i != quads.end(); ++i)
		{
			if ((*i)->edges[0].draw)
			{
				DrawEdge((*i)->edges, window);
			}
			if ((*i)->edges[1].draw)
			{
				DrawEdge((*i)->edges + 1, window);
			}
		}

		window.display();
	}

	return 0;
}

