/**
 * @file state.cpp
 * Function definitions for state manipulation
*/
#include<vector>
#include<iostream>
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

bool Board::FlipMarkers(Point p, Point q, Dir dir) {
	if(Board::IsValid(p.x, p.y) && Board::IsValid(q.x, q.y)) {
		
		int x = q.x - p.x;
		int y = q.y - p.y;
		
		if ((x == dir.first || (x / dir.first == int(x / dir.first))) && 
		   (y == dir.second || (y / dir.second == int(y / dir.second)))) {		
			
			for(Point i = p; ; i.x+=dir.first, i.y+=dir.second) {
				Element current = Board::GetElementAt(i.x, i.y);
				if(current == B_MARKER) {
					current_board[i.x][i.y] = W_MARKER;
				}
				else if(current == W_MARKER) {
					current_board[i.x][i.y] = B_MARKER;
				}
				if(i.x == q.x && i.y == q.y) {
					break;
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
}