#include <boost/functional/hash.hpp>
#include <climits>
#include <cmath>
#include <string>
#include <map>

#include "gtsa.hpp"
#include "utils.h"

typedef __int128 int128_t;

const char PLAYER_1 = '1';
const char PLAYER_2 = '2';

enum class Direction {
	N, S, NE, NW, SE, SW,
};

Direction all_directions[6] = {N, S, NE, NW, SE, SW};

struct YinshState : public State<YinshState, YinshMove> {

	Board board_1, board_2;
	int64_t no_of_markers_remaining;
	int64_t no_of_rings_placed_1, no_of_rings_placed_2;
	int64_t no_of_rings_removed_1, no_of_rings_removed_2;
	std::vector<int128_t> rings_1, rings_2;
	std::vector<int128_t> rows_formed_1, rows_formed_2;

	YinshState() : State(PLAYER_1) {
		no_of_markers_remaining = 51;
		no_of_rings_placed_1 = 0;
		no_of_rings_placed_2 = 0;
		no_of_rings_removed_1 = 0;
		no_of_rings_removed_2 = 0;
	}

	YinshState clone() const override {
		YinshState clone = YinshState();
		clone.board_1 = Board(board_1);
		clone.board_2 = Board(board_2);
		clone.no_of_markers_remaining = no_of_markers_remaining;
		clone.no_of_rings_placed_1 = no_of_rings_placed_1;
		clone.no_of_rings_placed_2 = no_of_rings_placed_2;
		clone.no_of_rings_removed_1 = no_of_rings_removed_1;
		clone.no_of_rings_removed_2 = no_of_rings_removed_2;
		clone.rows_formed_1 = rows_formed_1;
		clone.rows_formed_2 = rows_formed_2;
		clone.rings_1 = rings_1;
		clone.rings_2 = rings_2;
		clone.player_to_move = player_to_move;
		return clone;
	}

	vector<YinshMove> get_legal_moves(int max_moves = INF) const override {
		auto &board = player_to_move == PLAYER_1 ? board_1 : board_2;
		auto &rings = player_to_move == PLAYER_1 ? rings_1 : rings_2;
		auto &all_rings = player_to_move == PLAYER_1 ? all_rings_1 : all_rings_2;
		vector<YinshMove> moves;
		for(auto ring: rings) {
			for (auto dir: all_directions) {
				bool jump = false;
				while(true) {
					if(next_element[dir].isValid(ring) > 0) {
						int128_t next = next_element[dir][ring];
						if(board & next == next &&
						   all_rings.count(next) > 0) {
							break;
						}
						else if(board & next == 0) {
							moves.push_back(YinshMove(dir, next));
							if (jump) {
								break;
							}
						}
						else if(board & next == next &&
								all_rings.count(next) == 0) {
							if(!jump) {
								jump = true;
							}
						}
					}
					else {
						break;
					}
				};
			}
		}
		return moves;
	}

	char get_enemy(char player) const override {
		return (player == PLAYER_1) ? PLAYER_2 : PLAYER_1;
	}

	bool is_terminal() const override {
		return is_winner(player_to_move) ||
		       is_winner(get_enemy(player_to_move));
	}

	bool is_winner(char player) const override {
		int64_t no_of_rings_removed =
		    (player == PLAYER_1) ? no_of_rings_removed_1 : no_of_rings_removed_2;
		if (no_of_rings_removed >= 3)
			return true;
		return false;
	}
};
