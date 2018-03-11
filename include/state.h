/**
 * @file state.h
 * Definitions for Yinsh's game state
 */
#ifndef STATE_STATE_H
#define STATE_STATE_H

#include <vector>

namespace state {

enum Element {
	I, E, B_RING, W_RING, B_MARKER, B_MARKER
}

/**
 * Game board for the simulation
 */
class Board {
private:
	std::vector< std::vector <Element > > current_board = 
	{
		{I, I, I, E, I, E, I, E, I, I, I}
		{I, I, E, I, E, I, E, I, E, I, I}
		{I, E, I, E, I, E, I, E, I, E, I}
		{I, I, E, I, E, I, E, I, E, I, I}
		{I, E, I, E, I, E, I, E, I, E, I}
		{E, I, E, I, E, I, E, I, E, I, E}
		{I, E, I, E, I, E, I, E, I, E, I}
		{E, I, E, I, E, I, E, I, E, I, E}
		{I, E, I, E, I, E, I, E, I, E, I}
		{E, I, E, I, E, I, E, I, E, I, E}
		{I, E, I, E, I, E, I, E, I, E, I}
		{E, I, E, I, E, I, E, I, E, I, E}
		{I, E, I, E, I, E, I, E, I, E, I}
		{I, I, E, I, E, I, E, I, E, I, I}
		{I, E, I, E, I, E, I, E, I, E, I}
		{I, I, E, I, E, I, E, I, E, I, I}
		{I, I, I, E, I, E, I, E, I, I, I}
		{I, I, I, I, E, I, E, I, I, I, I}
	};
public:
	Element GetElementAt(Point);
	void AddElementAt(Point, Element);
	void RemoveElementAt(Point);
	void MoveElement(Point from, Point to);
	void FlipMarkerColor(Point);
};

}

#endif