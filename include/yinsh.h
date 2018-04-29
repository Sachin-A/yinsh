#include <boost/functional/hash.hpp>
#include <climits>
#include <cmath>
#include <string>
#include <utility>
#include <map>

#include "./uint128.h"
#include "./mappings.h"
#include "./gtsa.hpp"

const char PLAYER_1 = '1';
const char PLAYER_2 = '2';

/**
 * @brief      Output format for printing different elements
 */
std::string blankFormat			= "         ";
std::string emptyFormat			= "    \033[1;31m*\033[0m    ";
std::string blackRingFormat		= "    \033[1;30;47mR\033[0m    ";
std::string whiteRingFormat		= "    \033[1;37mR\033[0m    ";
std::string blackMarkerFormat	= "    \033[1;37;40mM\033[0m    ";
std::string whiteMarkerFormat	= "    \033[1;37mM\033[0m    ";

uint128_t valid_positions = ~uint128_t(0) >> 43;

struct YinshMove : public Move<YinshMove> {
	int type; // 1,2,3
	uint128_t ring_pos; // type1 uses only this
	uint128_t ring_dest; // type2 uses ring_pos and ring_dest
	std::vector<uint128_t> rows; // type3 uses rows and rings
	std::vector<uint128_t> rings;

	// Constructors - common for all types
	YinshMove() {}

	YinshMove(uint128_t ring_pos_) : ring_pos(ring_pos_) {
		type = 1;
	}
	YinshMove(uint128_t ring_pos_, uint128_t ring_dest_) : ring_pos(ring_pos_), ring_dest(ring_dest_) {
		type = 2;
	}
	YinshMove(std::vector<uint128_t> rows_, std::vector<uint128_t> rings_) : rows(rows_), rings(rings_) {
		type = 3;
	}

	void read(istream &stream = cin) override {
		if (type == 1)
			read1(stream);
		else if (type == 2)
			read2(stream);
		else if (type == 3)
			read3(stream);
	}

	ostream &to_stream(ostream &os) const override {
		if (type == 1)
			return to_stream1(os);
		if (type == 2)
			return to_stream2(os);

		return to_stream3(os);
	}

	bool operator==(const YinshMove &rhs) const override {
		if (type == 1)
			return eq1(rhs);
		if (type == 2)
			return eq2(rhs);

		return eq3(rhs);
	}

	size_t hash() const override {
		if (type == 1)
			return hash1();
		if (type == 2)
			return hash2();

		return hash3();
	}

	// Type 1 stuff
	void read1(istream &stream = cin) {
		if (&stream == &cin) {
			cout << "Enter X1, Y1 for ring position: ";
		}
		int row, column;
		stream >> row >> column;
		ring_pos = xy2bitboard(row, column);
	}

	ostream &to_stream1(ostream &os) const {
		return os << "Ring destination: " << ring_pos << " and is a type 1 move!\n";
	}

	bool eq1(const YinshMove &rhs) const {
		return ring_pos == rhs.ring_pos;
	}

	size_t hash1() const {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(type));
		hash_combine(seed, hash_value(ring_pos));
		return seed;
	}


	// Type 2 stuff
	void read2(istream &stream = cin) {
		if (&stream == &cin) {
			cout << "Enter X1, Y1 followed by X2, Y2 for ring src and dest: ";
		}
		int r1, c1, r2, c2;
		stream >> r1 >> c1 >> r2 >> c2;
		ring_pos = xy2bitboard(r1, c1);
		ring_dest = xy2bitboard(r2, c2);
	}

	ostream &to_stream2(ostream &os) const {
		return os << "Ring src: " << ring_pos << " Ring dest: " << ring_dest << " and is a type 2 move!\n";
	}

	bool eq2(const YinshMove &rhs) const {
		return ring_pos == rhs.ring_pos && ring_dest == rhs.ring_dest;
	}

	size_t hash2() const {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(type));
		hash_combine(seed, hash_value(ring_pos));
		hash_combine(seed, hash_value(ring_dest));
		return seed;
	}

	// Type 3 stuff

	void read3(istream &stream = cin) {
		int no;
		if (&stream == &cin) {
			cout << "Enter number of rows/rings";
			cin >> no;
		}
		std::vector<uint128_t> ring_points;
		int first, second;
		for(int i = 0; i < no * 5; i++) {
			cout << "Row: " << (i / no) + 1 << ", Point: " << (i % no) + 1 << "\n";
			stream >> first >> second;
			rows[i] |= xy2bitboard(first, second);
		}
		for(int i = 0; i < no; i++) {
			cout << "Ring: " << i + 1 << "\n";
			stream >> first >> second;
			rings.push_back(xy2bitboard(first, second));
		}
	}

	ostream &to_stream3(ostream &os) const {
		return os << "Type 3 move!\n";
	}

	bool eq3(const YinshMove &rhs) const {
		return rows == rhs.rows && rings == rhs.rings;
	}

	size_t hash3() const {
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

int __move__count__ = 0;
// The following are just for readability
inline YinshMove AddRingMove(uint128_t pos) {
	//if (__move__count__) {
		cout << __move__count__ << " AddRing(" << pos << ")\n";
	//}
	__move__count__++;
	return YinshMove(pos);
}
inline YinshMove AddMarkerMove(uint128_t src, uint128_t dest) {
	//if (__move__count__) {
		cout << __move__count__ << " AddMarker(" << src << "," << dest << ")\n";
	//}
	__move__count__++;
	return YinshMove(src, dest);
}
inline YinshMove RemoveRingMove(std::vector<uint128_t> rows, std::vector<uint128_t> rings) {
	//if (__move__count__) {
		cout << __move__count__ << " RemoveRing(" << rows[0] << "," << rows.size() << ")\n";
	//}
	__move__count__++;
	return YinshMove(rows, rings);
}

struct Board {
	uint128_t board = 0;

	Board() {}

	Board(const Board &other) { board = other.board; }

	bool operator==(const Board &other) const { return board == other.board; }

	bool isValid(uint128_t x) const { return (x & valid_positions) != 0; }
};

size_t hash_value(const Board &board) {
	hash<uint128_t> hash_fn;
	return hash_fn(board.board);
}

struct YinshState : public State<YinshState, YinshMove> {

	Board board_1, board_2;
	uint64_t no_of_markers_remaining;
	uint64_t no_of_rings_placed_1, no_of_rings_placed_2;
	uint64_t no_of_rings_removed_1, no_of_rings_removed_2;
	std::unordered_map<uint128_t, bool> rings_1, rings_2;
	std::unordered_map<uint128_t, bool> rows_formed_1, rows_formed_2;
	std::unordered_map<uint128_t, bool> all_rings;

	// cp stands for current player. These will point to the correct player's variables
	Board& board_cp;
	uint64_t& no_of_rings_placed_cp;
	uint64_t& no_of_rings_removed_cp;
	std::unordered_map<uint128_t, bool> rings_cp;
	std::unordered_map<uint128_t, bool> rows_formed_cp;

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
		clone.set_player_vars();
		return clone;
	}

	void set_player_vars(int pl = -1) {
		if (pl == -1)
			pl = player_to_move;

		if (pl == PLAYER_1) {
			board_cp = board_1;
			no_of_rings_removed_cp = no_of_rings_placed_1;
			no_of_rings_removed_cp = no_of_rings_removed_1;
			rings_cp = rings_1;
			rows_formed_cp = rows_formed_1;
		} else {
			board_cp = board_2;
			no_of_rings_removed_cp = no_of_rings_placed_2;
			no_of_rings_removed_cp = no_of_rings_removed_2;
			rings_cp = rings_2;
			rows_formed_cp = rows_formed_2;
		}
	}

	int get_goodness() const override { return 0; }

/*	int get_goodness() const override {
		if (is_terminal()) {
			if (is_winner(player_to_move)) {
				return INT_MAX;
			} else if (is_winner(get_enemy(player_to_move))) {
				return INT_MIN;
			} else {
				return 0;
			}
		}

		int score = get_reserve_value(pieces_left_1) - get_reserve_value(pieces_left_2);

		auto pieces_board_1 = no_of_set_bits(board_1.board);
		auto pieces_board_2 = no_of_set_bits(board_2.board);
		score += (pieces_board_1 - pieces_board_2) * 230;

		int pieces_dead_1 = 15 - pieces_left_1 - pieces_board_1;
		int pieces_dead_2 = 15 - pieces_left_2 - pieces_board_2;
		score += (pieces_dead_2 - pieces_dead_1) * (pieces_dead_2 + pieces_dead_1) * 10;

		if (player_to_move == PLAYER_2) {
			score *= -1;
		}

		return score;
	}*/

	void get_non_intersecting_rows (std::vector<std::vector<uint128_t> >& choices) const {
		for(int i = 0; i < choices.size(); ) {
			for (int j = 0; j < choices[i].size() - 1; j++) {
				for (int k = j + 1; k < choices[i].size(); k++) {
					if((choices[i][j] & choices[i][k]) != 0) {
						choices.push_back(choices[i]);
						int last = choices[i].size()-1;

						choices[i].erase(choices[i].begin() + k);
						choices[last].erase(choices[last].begin() + j);

						goto split;
					}
				}
			}
			split:
				continue;
				i++;
		}
	}

	// Not handling rings < rows
	void combinations(int offset, int k,
					  const std::vector<uint128_t>& rings,
					  std::vector<std::vector<uint128_t> >& all_sets,
					  std::vector<uint128_t> current_set) const {
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
		for(int pl = 1; pl <= 2; pl++) {
			for(auto& outer_map: five_in_a_row_bitmasks) {
				for(auto& inner_map: outer_map.second) {
					if(board_cp.board & inner_map.second == inner_map.second) {
						uint128_t row_mask = inner_map.second;
						rows_formed_cp.push_back(row_mask);
					}
				}
			}
		}
	}

	std::vector<YinshMove> get_legal_moves(int max_moves = INF) const override {
		auto combined_board = board_1.board | board_2.board;

		std::vector<YinshMove> moves;
		if(no_of_rings_placed_cp < 5) {
			int count = 1;
			for(uint128_t i = 1; count < 128; i <<= 1, count++) {
				auto c1 = i & combined_board;
				if(c1 == 0 &&
				   board_1.isValid(i)) {
					moves.push_back(AddRingMove(i));
				}
			}
			return moves;
		}
		else if(!rows_formed.empty()) {
			int k = 0;
			std::vector<std::vector<uint128_t> > choices = { rows_formed_cp };
			get_non_intersecting_rows(choices);
			std::sort(choices.begin(), choices.end(),
				[](const vector<uint128_t> & a, const vector<uint128_t> & b){ return a.size() < b.size(); });
			std::vector<std::vector<uint128_t> > all_sets;
			std::vector<uint128_t> current_set;
			for(auto& choice: choices) {
				if(k != choice.size()) {
					k = choice.size();
					all_sets.clear();
					combinations(0, k, rings_cp, all_sets, current_set);
				}
				for(int i = 0; i < all_sets.size(); i++) {
					moves.push_back(RemoveRingMove(choice, all_sets[i]));
				}
			}
			return moves;
		}
		else {
			for(auto ring: rings_cp) {
				for (auto dir: directions) {
					bool jump = false;
					while(hasNext(dir, ring)) {
						auto next = getNext(dir, ring);
						auto c1 = combined_board & next;

						assert(c1 == 0 || c1 == next); // either the position is occupied, or it isn't.

						// three cases:
						// 1. location is empty
						if(c1 == 0) {
							moves.push_back(AddMarkerMove(ring, next));
							if (jump) {
								break;
							}
						}
						// 2. a ring already exists there
						else if (c1 == next && all_rings.count(next) > 0) {
							break;
						}
						// 3. a marker already exists there
						else if(c1 == next && all_rings.count(next) == 0) {
							if(!jump) {
								jump = true;
							}
						}

						ring = next;
					}
				}
			}
			return moves;
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

	bool remove_ring(uint128_t ring_pos) {
		auto it1 = all_rings.find(ring_pos);
		assert(it1 != all_rings.end());
		all_rings.erase(it1);

		auto it2 = rings_cp.find(ring_pos);
		if (it2 != rings.end()) {
			rings.erase(it2);
		}
	}

	bool flip_markers(uint128_t ring_pos, uint128_t ring_dest, Board& board) {
		auto flip_mask = flip_bitmasks.find(ring_pos)->second.find(ring_dest)->second;
		auto& enemy_board = (player_to_move == PLAYER_1) ? board_2 : board_1;
		auto b1 = board.board & flip_mask;
		auto b2 = enemy_board.board & flip_mask;
		board.board = (board.board & (~flip_mask)) | b2;
		enemy_board.board = (enemy_board.board & (~flip_mask)) | b1;
	}

	bool add_ring(uint128_t ring_pos, Board& board) {
		assert(no_of_rings_placed_cp < 5);
		no_of_rings_placed_cp++;

		board.board |= ring_pos;
		rings_cp[ring_pos] = 1;
		all_rings[ring_pos] = 1;
	}

	bool add_ring_undo(uint128_t ring_pos, Board& board) {
		no_of_rings_placed_cp--;
		board.board ^= ring_pos;
		rings_cp.erase(ring_pos);
		all_rings.erase(ring_pos);
	}

	bool move_ring(uint128_t ring_pos, uint128_t ring_dest, Board& board, int undo) {

		remove_ring(ring_pos, undo);

		flip_markers(ring_pos, ring_dest, board);

		add_ring(ring_dest, board);

		if(undo) {
			no_of_markers_remaining++;
		}
		else {
			no_of_markers_remaining--;
		}
	}

	bool remove_row_and_ring(const std::vector<uint128_t>& rows,
							 const std::vector<uint128_t>& rings,
							 Board& board) {
		uint128_t kill_mask = 0;

		auto &rows_formed = player_to_move == PLAYER_1 ? rows_formed_1 : rows_formed_2;
		rows_formed.clear();
		for(auto row: rows) {
			kill_mask |= row;
		}

		auto &no_of_rings_removed =
			player_to_move == PLAYER_1 ? no_of_rings_removed_1 : no_of_rings_removed_2;
		for(auto ring: rings) {
			kill_mask |= ring;
			remove_ring(ring, 0);
			no_of_rings_removed++;
		}

		board.board &= (~kill_mask);
	}

	bool add_row_and_ring(const std::vector<uint128_t>& rows,
						  const std::vector<uint128_t>& rings,
						  Board& board) {
		uint128_t save_mask = 0;

		auto &rows_formed = player_to_move == PLAYER_1 ? rows_formed_1 : rows_formed_2;
		for(auto row: rows) {
			save_mask |= row;
		}

		auto &no_of_rings_removed =
			player_to_move == PLAYER_1 ? no_of_rings_removed_1 : no_of_rings_removed_2;
		for(auto ring: rings) {
			save_mask |= ring;
			add_ring(ring, board);
			no_of_rings_removed--;
		}

		board.board |= (save_mask);
		update_rows_formed();
	}

	void make_move(const YinshMove& move) override {
		set_player_vars();

		int type = move.type;
		switch(type) {
			case 1:		{
				add_ring(move.ring_pos, board_cp);
				player_to_move = get_enemy(player_to_move);
				break;
			}
			case 2: 	{
				move_ring(move.ring_pos, move.ring_dest, board_cp, 0);
				update_rows_formed();
				if(rows_formed.empty()) {
					player_to_move = get_enemy(player_to_move);
				}
				break;
			}
			case 3: 	{
				remove_row_and_ring(move.rows, move.rings, board_cp);
				rows_formed_cp.clear();
				player_to_move = get_enemy(player_to_move);
				break;
			}
			default:	assert(false);
		}
	}

	void undo_move(const YinshMove& move) override {
		player_to_move = get_enemy(player_to_move);
		set_player_vars();

		auto& board = (player_to_move == PLAYER_1) ? board_1 : board_2;
		int type = move.type;
		switch(type) {
			case 1:		{
				remove_ring(move.ring_pos, 1);
				no_of_rings_placed_cp--;
				break;
			}
			case 2: 	{
				move_ring(move.ring_dest, move.ring_pos, board, 1);
				break;
			}
			case 3: 	{
				add_row_and_ring(move.rows, move.rings, board);
				break;
			}
			default:	break;
		}
	}

	ostream &to_stream(ostream &os) const override {
		for (int i = 0; i <= 19; i++) {
			for (int j = 0; j < 11; j++) {
				if (i == 0) {
					if (j == 0) {
						os << "            ";
					}
					os << "    " << j << "    ";
					continue;
				}
				if (j == 0) {
					if (i <= 10) {
						os << " ";
					}
					os << "     " << i - 1 << "     ";
				}

				if(isValidXYCoord(xy_coord_t(i - 1, j))) {
					uint128_t p = xy2bitboard(i - 1, j);
					if(p & board_1.board) {
						if(all_rings_1.count(p) > 0) {
							os << whiteRingFormat;
							break;
						}
						else {
							os << whiteMarkerFormat;
							break;
						}
					}
					else if(p & board_2.board) {
						if(all_rings_2.count(p) > 0) {
							os << blackRingFormat;
							break;
						}
						else {
							os << blackMarkerFormat;
							break;
						}
					}
					os << emptyFormat;
					break;
				}
				else {
					os << blankFormat;
					break;
				}
			}
			os << "\n\n";
		}
		os << "Player to move: " << player_to_move << "\n";
		os << "Player 1 rings to be placed: " << no_of_rings_placed_1 << "\n";
		os << "Player 1 rings removed: " << no_of_rings_removed_1 << "\n";
		os << "Player 2 rings to be placed: " << no_of_rings_placed_2 << "\n";
		os << "Player 2 rings removed: " << no_of_rings_removed_2 << "\n";
		os << "Markers remaining: " << no_of_markers_remaining << "\n\n";
		return os;
	}

	bool operator==(const YinshState &other) const override {
		return board_1 == other.board_1 && board_2 == other.board_2 &&
			   player_to_move == other.player_to_move &&
			   no_of_markers_remaining == other.no_of_markers_remaining &&
			   no_of_rings_placed_1 == other.no_of_rings_placed_1 &&
			   no_of_rings_placed_2 == other.no_of_rings_placed_2 &&
			   no_of_rings_removed_1 == other.no_of_rings_removed_1 &&
			   no_of_rings_removed_2 == other.no_of_rings_removed_2 &&
			   player_to_move == other.player_to_move &&
			   rings_1 == other.rings_1 && rows_formed_1 == other.rows_formed_1 &&
			   rings_2 == other.rings_2 && rows_formed_2 == other.rows_formed_2;
	}

	size_t hash() const {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;
		hash_combine(seed, hash_value(board_1));
		hash_combine(seed, hash_value(board_2));
		hash_combine(seed, hash_value(no_of_markers_remaining));
		hash_combine(seed, hash_value(no_of_rings_placed_1));
		hash_combine(seed, hash_value(no_of_rings_placed_2));
		hash_combine(seed, hash_value(no_of_rings_removed_1));
		hash_combine(seed, hash_value(no_of_rings_removed_2));
		hash_combine(seed, hash_value(player_to_move));
		for (auto ring_1 : rings_1) {
			hash_combine(seed, hash_value(ring_1));
		}
		for (auto ring_2 : rings_2) {
			hash_combine(seed, hash_value(ring_2));
		}
		for (auto row_1 : rows_formed_1) {
			hash_combine(seed, hash_value(row_1));
		}
		for (auto row_2 : rows_formed_2) {
			hash_combine(seed, hash_value(row_2));
		}
		return seed;
	}
};
