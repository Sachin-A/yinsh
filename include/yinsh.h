#include <boost/functional/hash.hpp>
#include <climits>
#include <cmath>
#include <string>
#include <utility>
#include <map>

#include "gtsa.hpp"
#include "utils.h"

typedef __uint128_t uint128_t;

const char PLAYER_1 = '1';
const char PLAYER_2 = '2';

enum class Direction {
	N, S, NE, NW, SE, SW,
};

Direction all_directions[6] = {N, S, NE, NW, SE, SW};

struct YinshMove1: public Move<YinshMove1>{
	int type;
	uint128_t ring_pos;

	YinshMove1() {}

	YinshMove1(uint128_t ring_pos_) : ring_pos(ring_pos_) {
		type = 1;
	}

	void read(istream &stream = cin) override {
		if (&stream == &cin) {
			cout << "Enter X1, Y1 for ring position: ";
		}
		int row, column;
		stream >> row >> column;
		ring_pos = xytoint(row, column);
	}

	ostream &to_stream(ostream &os) const override {
		return os << "Ring destination: " << ring_pos << " and is a type 1 move!\n";
	}

	bool operator==(const YinshMove1 &rhs) const override {
		return ring_pos == rhs.ring_pos;
	}

	size_t hash() const override {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(type));
		hash_combine(seed, hash_value(ring_pos));
		return seed;
	}

};

struct YinshMove2: public Move<YinshMove2>{
	int type;
	uint128_t ring_pos, ring_dest;

	YinshMove2(uint128_t ring_pos_, uint128_t ring_dest_) : ring_pos(ring_pos_), ring_dest(ring_dest_) {
		type = 2;
	}

	void read(istream &stream = cin) override {
		if (&stream == &cin) {
			cout << "Enter X1, Y1 followed by X2, Y2 for ring src and dest: ";
		}
		int r1, c1, r2, c2;
		stream >> r1 >> c1 >> r2 >> c2;
		ring_pos = xytoint(r1, c1);
		ring_dest = xytoint(r2, c2);
	}

	ostream &to_stream(ostream &os) const override {
		return os << "Ring src: " << ring_pos << " Ring dest: " << ring_dest << " and is a type 2 move!\n";
	}

	bool operator==(const YinshMove2 &rhs) const override {
		return ring_pos == rhs.ring_pos && ring_dest == rhs.ring_dest;
	}

	size_t hash() const override {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(type));
		hash_combine(seed, hash_value(ring_pos));
		hash_combine(seed, hash_value(ring_dest));
		return seed;
	}
};

struct YinshMove3: public Move<YinshMove3>{
	int type;
	std::vector<uint128_t> rows;
	std::vector<uint128_t> rings;

	YinshMove3(std::vector<uint128_t> rows_, std::vector<uint128_t> rings_) : rows(rows_), rings(rings_) {
		type = 3;
	}

	void read(istream &stream = cin) override {
		if (&stream == &cin) {
			cout << "Enter number of rows/rings";
		}
		int no;
		std::vector<std::vector<std::pair<int, int> > > row_points;
		std::vector<std::pair<int, int> > ring_points;
		int first, second;
		for(int i = 0; i < no * 5; i++) {
			cout << "Row: " << (i / no) + 1 << ", Point: " << (i % no) + 1 << "\n";
			stream >> first >> second;
			row_points[i].push_back(std::make_pair(first, second));
		}
		for(int i = 0; i < no; i++) {
			cout << "Ring: " << i + 1 << "\n";
			stream >> first >> second;
			rings.push_back(xytoint(first, second));
		}
		for(int i = 0; i < no; i++) {
			rows[i] = xysettoint(row_points[i]);
		}
	}

	ostream &to_stream(ostream &os) const override {
		return os << "Type 3 move!\n";
	}

	bool operator==(const YinshMove3 &rhs) const override {
		return rows == rhs.rows && rings == rhs.rings;
	}

	size_t hash() const override {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(type));
		for (auto row : rows) {
			hash_combine(seed, hash_value(row));
		}
		for (auto ring : rings) {
			hash_combine(seed, hash_value(ring));
		}
		return seed;
	}
};

struct YinshState : public State<YinshState, YinshMove> {

	Board board_1, board_2;
	uint64_t no_of_markers_remaining;
	uint64_t no_of_rings_placed_1, no_of_rings_placed_2;
	uint64_t no_of_rings_removed_1, no_of_rings_removed_2;
	std::vector<uint128_t> rings_1, rings_2;
	std::vector<uint128_t> rows_formed_1, rows_formed_2;

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

	void get_non_intersecting_rows (
		std::vector<std::vector<uint128_t> >& choices,
		int n) {
		for(int i = n; i < choices.size(); ) {
			bool split = false;
			for (int j = 0; j < choices[i].size() - 1; j++) {
				for (int k = j + 1; k < choices[i].size(); k++) {
					if((choices[i][j] & choices[i][k]) != 0) {
						choices.push_back(choices[i]);

						std::vector<uint128_t>::iterator pos1 = std::find(choices[i].begin(), choices[i].end(), choices[i][k]);
						if (pos1 != choices[i].end()) {
							choices[i].erase(pos1);
						}
						std::vector<uint128_t>::iterator pos2 = std::find(choices[choices.size() - 1].begin(),
																	choices[choices.size() - 1].end(), choices[i][j]);
						if (pos2 != choices[choices.size() - 1].end()) {
							choices[choices.size() - 1].erase(pos2);
						}
						split = true;
						break;
					}
					if(split) {
						break;
					}
				}
				if(split) {
						break;
				}
			}
			if(!split) {
				i++;
			}
		}
	}

	// Not handling rings < rows
	void combinations(int offset, int k,
					  std::vector<uint128_t>& rings,
					  std::vector<std::vector<uint128_t> >& all_sets,
					  std::vector<uint128_t> current_set) {
		if (k == 0) {
			all_sets.push_back(current_set);
			return;
		}
		for (int i = offset; i <= rings.size() - k; ++i) {
			current_set.push_back(rings[i]);
			combinations(i+1, k-1, rings, all_sets, current_set);
			current_set.pop_back();
		}
	}

	void update_rows_formed() {
		for(int pl = 1; i <= 2; i++) {
			auto &board = pl == PLAYER_1 ? board_1 : board_2;
			auto &rows_formed = pl == PLAYER_1 ? rows_formed_1 : rows_formed_2;
			rows_formed.clear();
			for(auto row_mask: all_row_masks) {
				if(board & row_mask == row_mask) {
					rows_formed.push_back(row_mask);
				}
			}
		}
	}

	std::vector<YinshMove> get_legal_moves(int max_moves = INF) const override {
		auto &board = player_to_move == PLAYER_1 ? board_1 : board_2;
		auto &rings = player_to_move == PLAYER_1 ? rings_1 : rings_2;
		auto &all_rings = player_to_move == PLAYER_1 ? all_rings_1 : all_rings_2;
		auto &rows_formed = player_to_move == PLAYER_1 ? rows_formed_1 : rows_formed_2;
		auto &no_of_rings_placed =
			player_to_move == PLAYER_1 ? no_of_rings_placed_1 : no_of_rings_placed_2;

		std::vector<YinshMove> moves;
		if(no_of_rings_placed < 5) {
			int count = 1;
			for(uint128_t i = 1; c < 128; i <<= 1) {
				if(i & board == 0 &&
				   board.isValid(i)) {
					moves.push_back(YinshMove(i));
				}
			}
			no_of_rings_placed--;
			return moves;
		}
		else {
			if(!rows_formed.empty()) {
				int k = 0;
				std::vector<std::vector<uint128_t> > choices = { rows_formed };
				get_non_intersecting_rows(choices);
				std::sort(choices.begin(), choices.end(),
					[](const vector<int> & a, const vector<int> & b){ return a.size() < b.size(); });
				std::vector<std::vector<uint128_t> > all_sets;
				std::vector<uint128_t> current_set;
				for(choice: choices) {
					if(k != choice.size()) {
						k = choice.size();
						all_sets.clear();
						combinations(0, k, rings, all_sets, current_set);
					}
					for(int i = 0; i < all_sets.size(); i++) {
						moves.push_back(YinshMove(choice, all_sets[i]));
					}
				}
				return moves;
			}
			else {
				for(auto ring: rings) {
					for (auto dir: all_directions) {
						bool jump = false;
						while(true) {
							if(next_element[dir].count(ring) > 0) {
								uint128_t next = next_element[dir][ring];
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
				update_rows_formed();
				return moves;
			}
		}
	}

	char get_enemy(char player) const override {
		return (player == PLAYER_1) ? PLAYER_2 : PLAYER_1;
	}

	bool is_terminal() const override {
		return is_winner(player_to_move) ||
		       is_winner(get_enemy(player_to_move));
	}

	bool is_winner(char player) const override {
		uint64_t no_of_rings_removed =
		    (player == PLAYER_1) ? no_of_rings_removed_1 : no_of_rings_removed_2;
		if (no_of_rings_removed >= 3)
			return true;
		return false;
	}

	bool add_ring(uint128_t ring_pos, Board& board) {
		auto& no_of_rings_placed =
			(player_to_move == PLAYER_1) ? no_of_rings_placed_1 : no_of_rings_placed_2;
		if(no_of_rings_placed < 5) {
			no_of_rings_placed++;
		}
		auto& rings = (player_to_move == PLAYER_1) ? rings_1 : rings_2;
		board |= ring_pos;
		rings.push_back(ring_pos);
		std::pair< map<uint128_t, int>::iterator, bool> result;
		ptr = all_rings.emplace(ring_pos, 1);
		if (ptr.second) {
			return true;
		}
		else {
			return false;
		}
	}

	bool move_ring(uint128_t ring_pos, uint128_t ring_dest, Board& board) {
		auto it1 = all_rings.find(ring_pos);
		all_rings.erase(it);
		auto& rings = (player_to_move == PLAYER_1) ? rings_1 : rings_2;
		auto it2 = std::find(rings.begin(), rings.end(), ring_pos);
		if (it2 != rings.end()) {
			rings.erase(it2);
		}

		auto flip_mask = flip_bitmasks[ring_pos][ring_dest];
		auto& enemy_board = (player_to_move == PLAYER_1) ? board_2 : board_1;
		auto b1 = board & flip_mask;
		auto b2 = enemy_board & flip_mask;
		board = (board & (~flip_mask)) | b2;
		enemy_board = (enemy_board & (~flip_mask)) | b1;

		add_ring(ring_dest, board);
	}

	bool remove_row_and_ring(std::vector<uint128_t>& rows,
							 std::vector<uint128_t>& rings,
							 Board& board) {
		kill_mask = 0;
		for(auto row: rows) {
			kill_mask |= row;
		}
		for(auto ring: rings) {
			kill_mask |= ring;
		}
		board &= (~kill_mask);
	}

	void make_move(const YinshMove &move) override {
		auto& board = (player_to_move == PLAYER_1) ? board_1 : board_2;
		int type = move.type;
		switch(type) {
			case 1:		add_ring(move.ring_pos, board);
						player_to_move = get_enemy(player_to_move);
						break;
			case 2: 	move_ring(move.ring_pos, move.ring_dest, board);
						auto& rows_formed =
							(player_to_move == PLAYER_1) ? rows_formed_1 : rows_formed_2;
						if(rows_formed.empty()) {
							player_to_move = get_enemy(player_to_move);
						}
						break;
			case 3: 	remove_row_and_ring(move.rows, move.rings, board);
						player_to_move = get_enemy(player_to_move);
						break;
			default:	break;
		}
	}

};
