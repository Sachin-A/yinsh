/**
 * @file state.cpp
 * Function definitions for state manipulation
*/
#include<vector>
#include<iostream>
#include "../include/state.h"

namespace state {

/**
 * North, North-East, North-West, South, South-East, South-West
 */
Point dirs[6] = {{-2, 0 }, {-1, 1}, {-1, -1}, {2, 0}, {1, 1}, {1, -1}}

Point Point::operator+ (const Point& p) {
	Point temp;
	temp.x = x + p.x;
	temp.y = y + p.y;
	return temp;
}

bool Point::operator== (const Point& p) {
	if(x == p.x && y == p.y) {
		return true;
	}
	else {
		return false;
	}
}

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

bool Board::FlipMarkers(Point p, Point q, Point dir) {
	if(Board::IsValid(p.x, p.y) && Board::IsValid(q.x, q.y)) {
		
		float x = q.x - p.x;
		float y = q.y - p.y;
		
		if ((x == dir.x || (x / dir.x == int(x / dir.x))) &&
		    (y == dir.y || (y / dir.y == int(y / dir.y)))) {
			
			for(Point i = p; ; i = i + dir) {
				Element current = Board::GetElementAt(i.x, i.y);
				if(current == B_MARKER) {
					current_board[i.x][i.y] = W_MARKER;
				}
				else if(current == W_MARKER) {
					current_board[i.x][i.y] = B_MARKER;
				}
				if(i == q) {
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

void GameState::DisplayBoard() {
	for (int i = 0; i <= board.current_board.size(); i++) {
		for (int j = 0; j < board.current_board[0].size(); j++) {
			if (i == 0) {
				if (j == 0) {
					std::cout << "            ";
				}
				std::cout << "    " << j << "    ";
				continue;
			}
			if (j == 0) {
				if (i <= 10) {
					std::cout << " ";
				}
				std::cout << "     " << i - 1 << "     ";
			}
			switch(board.current_board[i-1][j]) {
				case I: 		std::cout << blankFormat;
								break;
				case E: 		std::cout << emptyFormat;
								break;
				case B_RING: 	std::cout << blackRingFormat;
								break;
				case W_RING: 	std::cout << whiteRingFormat;
								break;
				case B_MARKER: 	std::cout << blackMarkerFormat;
								break;
				case W_MARKER: 	std::cout << whiteMarkerFormat;
								break;
			}
		}
		std::cout << "\n\n";
	}
}

bool GameState::AddRing(Point ring_pos, int player_id) {
	if(board.IsValid(ring_pos.x, ring_pos.y)) {
		if(player_id == 0 && whiteRings > 0) {
			if(board.GetElementAt(ring_pos.x, ring_pos.y) == E) {
				board.AddElementAt(ring_pos, W_RING);
				whiteRings--;
				return true;
			}
			return false;
		}
		else if(player_id == 1 && blackRings > 0) {
			if(board.GetElementAt(ring_pos.x, ring_pos.y) == E) {
				board.AddElementAt(ring_pos, B_RING);
				blackRings--;
				return true;
			}
			return false;
		}
	}
	return false;
}

std::pair<int, std::vector<Point>> GameState::ValidPoints(Point ring_pos, Point dir) {
	if(board.IsValid(ring_pos.x, ring_pos.y)) {
		Point i = ring_pos;
		std::vector<Point> V;
		bool first_jump = false;
		bool no_jump = false;
		do {
			i = i + dir;
			if(board.GetElementAt(i.x, i.y) == W_RING ||
		            board.GetElementAt(i.x, i.y) == B_RING) {
				return std::make_pair(0, V);
			}
			else if(board.GetElementAt(i.x, i.y) == E) {
				V.push_back(i);
				if (first_jump) {
					return std::make_pair(1, V);
				}
				else {
					no_jump = true;
				}
			}
			else if(board.GetElementAt(i.x, i.y) == W_MARKER ||
		            board.GetElementAt(i.x, i.y) == B_MARKER) {
				if(no_jump) {
					return std::make_pair(2, V);
				}
				else {
					first_jump = true;
				}
			}

		} while(board.IsValid(i.x, i.y));
	}
}

}

int main() {

	state::GameState G;
	int x, y;
	int player_id = 0;

	clear;
	G.DisplayBoard();

	while(state::whiteRings > 0 || state::blackRings > 0) {
		std::cin >> x >> y;
		state::Point p = {x, y};
		bool check = G.AddRing(p, player_id);
		if(check)
			player_id = !player_id;

		clear;
		G.DisplayBoard();
	}
}
