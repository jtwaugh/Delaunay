# Delaunay
A C++ implementation of Guibas and Stolfi's divide-and-conquer algorithm from their 1985 paper. Uses SFML for rendering.

Yes, I used Visual Studio. You will have to include stdafx.h.

# Running the Program

In its current state, the program requires SFML and also some way to compile it. I don't have a makefile for you; sorry about that. It's currently set to choose 999 random pixels in a 512x512 window, remove duplicates, and render the Delaunay triangulation of those points. It has been called "beautiful."
