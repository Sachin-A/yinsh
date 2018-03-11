/**
 * @file state.cpp
 * Function definitions for state manipulation
*/
#include<vector>
#include "../include/state.h"

namespace state {

bool Board::IsValid(int x, int y) {
	if(current_board[x][y] != I) {
		return true;
	}
	else
		return false;
}

Element Board::GetElementAt(int x, int y) {
	return current_board[x][y];
}

bool Board::AddElementAt(Point p, Element e) {
	if(Board::IsValid(p.x, p.y)) {
		current_board[p.x][p.y] = e;
		return true;
	}
	else
		return false;
}

bool Board::RemoveElementAt(Point p) {
	if(Board::IsValid(p.x, p.y)) {
		current_board[p.x][p.y] = E;
		return true;
	}
	else
		return false;
}

bool Board::MoveElement(Point from, Point to) {
	if(Board::IsValid(to.x, to.y)) {
		current_board[to.x][to.y] = Board::GetElementAt(from.x, from.y);
		current_board[from.x][from.y] = E;
		return true;
	}
	else
		return false;
}

bool Board::FlipMarker(Point p) {
	if(Board::IsValid(p.x, p.y)) {
		Element current = Board::GetElementAt(p.x, p.y);
		if(current == B_MARKER) {
			current_board[p.x][p.y] = W_MARKER;
		}
		else if(current == W_MARKER) {
			current_board[p.x][p.y] = B_MARKER;
		}	
		return true;
	}
	else
		return false;
}
}