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

void GameState::DisplayBoard() {
	for (int i = 0; i <= board.current_board.size(); i++) {
		for (int j = 0; j < board.current_board[0].size(); j++) {
			if (i == 0) {
				if (j == 0) {
					std::cout << "            ";
				}
				std::cout << "     " << j << "     ";
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
		std::cout << std::endl << std::endl;
	}
}

bool GameState::AddRing(Point p, int player_id) {
	if(board.IsValid(p.x, p.y)) {
		if(player_id == 0 && whiteRings > 0) {
			if(board.GetElementAt(p.x, p.y) == E) {
				board.AddElementAt(p, W_RING);
				whiteRings--;
				return true;
			}
			return false;
		}
		else if(player_id == 1 && blackRings > 0) {
			if(board.GetElementAt(p.x, p.y) == E) {
				board.AddElementAt(p, B_RING);
				blackRings--;
				return true;
			}
			return false;
		}
	}
	return false;
}

}

int main() {

	state::GameState G;
	int x, y;
	int player_id = 0;

	G.DisplayBoard();

	while(state::whiteRings > 0 || state::blackRings > 0) {
		std::cin >> x >> y;
		state::Point p = {x, y};
		bool check = G.AddRing(p, player_id);
		if(check)
			player_id = !player_id;

		G.DisplayBoard();
	}
}
