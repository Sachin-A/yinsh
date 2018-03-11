/**
 * @file state.h
 * Function declarations for Yinsh's game state
 */
#ifndef STATE_STATE_H
#define STATE_STATE_H

#include <vector>

namespace state {

struct Point {
	int x;
	int y;
};

enum Element {
	I, E, B_RING, W_RING, B_MARKER, W_MARKER
};

/**
 * Game board for the simulation
 */
class Board {
public:
	std::vector< std::vector <Element > > current_board = 
	{
		{I, I, I, I, E, I, E, I, I, I, I},
		{I, I, I, E, I, E, I, E, I, I, I},
		{I, I, E, I, E, I, E, I, E, I, I},
		{I, E, I, E, I, E, I, E, I, E, I},
		{I, I, E, I, E, I, E, I, E, I, I},
		{I, E, I, E, I, E, I, E, I, E, I},
		{E, I, E, I, E, I, E, I, E, I, E},
		{I, E, I, E, I, E, I, E, I, E, I},
		{E, I, E, I, E, I, E, I, E, I, E},
		{I, E, I, E, I, E, I, E, I, E, I},
		{E, I, E, I, E, I, E, I, E, I, E},
		{I, E, I, E, I, E, I, E, I, E, I},
		{E, I, E, I, E, I, E, I, E, I, E},
		{I, E, I, E, I, E, I, E, I, E, I},
		{I, I, E, I, E, I, E, I, E, I, I},
		{I, E, I, E, I, E, I, E, I, E, I},
		{I, I, E, I, E, I, E, I, E, I, I},
		{I, I, I, E, I, E, I, E, I, I, I},
		{I, I, I, I, E, I, E, I, I, I, I}
	};
	bool IsValid(int, int);
	Element GetElementAt(int, int);
	bool AddElementAt(Point, Element);
	bool RemoveElementAt(Point);
	bool MoveElement(Point from, Point to);
	bool FlipMarker(Point);
};

}

#endif //STATE_STATE_H