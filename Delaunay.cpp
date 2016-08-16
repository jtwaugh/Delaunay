// Delaunay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "topology.h"
#include <iostream>
#include <chrono>

void DrawEdge(Edge* e, sf::RenderWindow& window)
{
	Edge* sym = e->Sym();
	Vert* destination = e->destination();
	Vert org = *(e->origin());
	Vert dest = *(e->destination());

	sf::VertexArray v(sf::Lines, 2);

	v[0].position = sf::Vector2f(org.x(), org.y());
	v[1].position = sf::Vector2f(dest.x(), dest.y());

	window.draw(v);
}

int _tmain(int argc, _TCHAR* argv[])
{

	// Get number of vertices to triangulate
	std::cout << "Enter number of vertices to triangulate:" << std::endl;
	int n;
	std::cin >> n;

	auto t1 = std::chrono::high_resolution_clock::now();
	QuadList quads = Delaunay(n).GetTriangulation();
	auto t2 = std::chrono::high_resolution_clock::now();

	std::cout << "Running time (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;

	// Time the algorithm
	/*
	for (int i = 1; i < 31; i++)
	{
		int ms[100];

		for (int t = 0; t < 100; t++)
		{
			auto t1 = std::chrono::high_resolution_clock::now();

			// Should wrap this up better
			std::vector<QuadEdge*> quads;
			Delaunay test(quads, i * 50);
			EdgeList edges = test.GetTriangulation();

			auto t2 = std::chrono::high_resolution_clock::now();

			ms[t] = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		}

		int time = 0;

		for (int t = 0; t < 100; t++)
		{
			time += ms[t];
		}

		double mean = ((double)time) / 100;

		std::cout << "Vertices: " << i * 50 << ", mean triangulation time (ms): " << mean << std::endl;
	}
	*/
	
	
	// Build the remdering environment
	sf::RenderWindow window(sf::VideoMode(512, 512), "Delaunay Triangulator");

	// Turn the vertices into something we want to render
	// Shoot, I should make them more lightweight in the algorithm
	std::vector<std::tuple<sf::Vector2f, sf::Vector2f>> quads_debug;
	for (int i = 0; i < quads.size(); i++)
	{
		sf::Vector2f a(quads[i]->edges[0].origin()->x(), quads[i]->edges[0].origin()->y());
		sf::Vector2f b(quads[i]->edges[2].origin()->x(), quads[i]->edges[2].origin()->y());
		quads_debug.push_back(std::make_tuple(a, b));
		//quads_debug.push_back(std::make_tuple(quads[i]->edges[1].origin()->getPosition(), quads[i]->edges[3].origin()->getPosition()));
	}

	// Rendering loop
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

