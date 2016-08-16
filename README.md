# Delaunay
A C++ implementation of Guibas and Stolfi's divide-and-conquer algorithm from their 1985 paper. Uses SFML for rendering.

Yes, I used Visual Studio. You will have to include stdafx.h.

# Running the Program

In its current state, the program requires SFML and also some way to compile it. I don't have a makefile for you; sorry about that. It's currently set to choose 999 random pixels in a 512x512 window, remove duplicates, and render the Delaunay triangulation of those points. It has been called "beautiful."

# Intellectual Property Concerns

As mentioned, the algorithm itself is given in Guibas and Stolfi's paper.

I feel obligated to mention that some of the code is taken/adapted from Paul Heckbert's course webpage here: http://www.cs.cmu.edu/afs/andrew/scs/cs/15-463/2001/pub/src/a2/quadedge.html. I can't find a license on the code anywhere, so please let me know if my project being on GitHub here in any way violates some terms I haven't seen yet.

With that uncertainty as the only caveat, I'd be thrilled to know if others find use for this in their own projects. Please use and redistribute and let me know if you do anything cool with it.
