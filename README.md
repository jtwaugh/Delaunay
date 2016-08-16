# Delaunay
A C++ implementation of Guibas and Stolfi's divide-and-conquer algorithm from their 1985 paper. Uses SFML for rendering and looks like this:

![alt tag](http://i.imgur.com/WZOAxow.png)

Yes, I used Visual Studio. You will have to include stdafx.h.

# What is This?
A Delaunay triangulation of a given set of points in the plane is the set of edge segments between these points such that, for every triangle formed by the edges, the circle circumscribing this triangle contains no vertices in its interior. This program generates random points at integer coordinates and finds the Delaunay triangulation to fit them.
The specifics of the algorithm can be found in Guibas and Stolfi (1985), but I'll go over them here. The idea is to split the set of points in half recursively until we have a binary tree with terminal nodes of sets of two or three points. These are trivial to connect. Moving back up, we merge each pair together while respecting the no-vertices-in-circumcircle criterion. The algorithm uses a number of topological insights from Guibas and Stolfi and features an implementation of their QuadEdge data structure to make efficient the computation of edge adjacencies.

# Running the Program

In its current state, the program requires SFML and also some way to compile it. I don't have a makefile for you; sorry about that. It's currently set to choose 999 random pixels in a 512x512 window, remove duplicates, and render the Delaunay triangulation of those points. It has been called "beautiful."

# Intellectual Property Concerns

As mentioned, the algorithm itself is given in Guibas and Stolfi's paper.

I feel obligated to mention that some of the code is taken/adapted from Paul Heckbert's course webpage here: http://www.cs.cmu.edu/afs/andrew/scs/cs/15-463/2001/pub/src/a2/quadedge.html. I can't find a license on the code anywhere, so please let me know if my project being on GitHub here in any way violates some terms I haven't seen yet.

With that uncertainty as the only caveat, I'd be thrilled to know if others find use for this in their own projects. Please use and redistribute and let me know if you do anything cool with it.
