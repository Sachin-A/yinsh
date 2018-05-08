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

/*
	Two types of moves:
	Type1: AddRing - done in first ten moves
	Type2: RemoveRowAddMarkerRemoveRow:
		In your turn, first check if there are rows formed for your color. If so, you've to remove them.
		Then you've to add a marker.
		Then if rows are formed for your color, you've to remove them.

		Type2 is a jumbo move, because otherwise the turn-switching logic doesn't work well with Minimax implementation
		of gtsa.hpp, and also undo becomes a little complex.
*/
struct YinshMove : public Move<YinshMove> {
	int type; // 1,2
	uint128_t ring_pos; // type1 uses only this
	uint128_t ring_dest; // type2 uses ring_pos and ring_dest
	std::vector<uint128_t> rows1, rows2; // type2 also uses rows1, rings1, row2, rings2
	std::vector<uint128_t> rings1, rings2;

	// Constructors - common for all types
	YinshMove() {}

	YinshMove(uint128_t ring_pos_) : ring_pos(ring_pos_) {
		type = 1;
	}
	YinshMove(std::vector<uint128_t> rows1_, std::vector<uint128_t> rings1_,
				uint128_t ring_pos_, uint128_t ring_dest_,
				std::vector<uint128_t> rows2_, std::vector<uint128_t> rings2_)
				: rows1(rows1_), rings1(rings1_),
				  ring_pos(ring_pos_), ring_dest(ring_dest_),
				  rows2(rows2_), rings2(rings2_) {
		type = 2;
	}

	void read(istream &stream = cin) override {
		if (type == 1)
			read1(stream);
		else if (type == 2)
			read2(stream);
	}

	ostream &to_stream(ostream &os) const override {
		if (type == 1)
			return to_stream1(os);
		return to_stream2(os);
	}

	bool operator==(const YinshMove &rhs) const override {
		if (type == 1)
			return eq1(rhs);
		return eq2(rhs);
	}

	size_t hash() const override {
		if (type == 1)
			return hash1();
		return hash2();
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
		int no;

		// read rings1, rows1
		if (&stream == &cin) {
			cout << "Enter number of rows/rings of your color made by opponent's move";
			cin >> no;
		}
		std::vector<uint128_t> ring_points;
		int first, second;
		for(int i = 0; i < no * 5; i++) {
			cout << "Row: " << (i / no) + 1 << ", Point: " << (i % no) + 1 << "\n";
			stream >> first >> second;
			rows1[i] |= xy2bitboard(first, second);
		}
		for(int i = 0; i < no; i++) {
			cout << "Ring: " << i + 1 << "\n";
			stream >> first >> second;
			rings1.push_back(xy2bitboard(first, second));
		}

		// Position of marker, and new position of the ring
		if (&stream == &cin) {
			cout << "Enter X1, Y1 followed by X2, Y2 for ring src and dest: ";
		}
		int r1, c1, r2, c2;
		stream >> r1 >> c1 >> r2 >> c2;
		ring_pos = xy2bitboard(r1, c1);
		ring_dest = xy2bitboard(r2, c2);

		// read rings2, rows2
		if (&stream == &cin) {
			cout << "Enter number of rows/rings of your color made by opponent's move";
			cin >> no;
		}
		for(int i = 0; i < no * 5; i++) {
			cout << "Row: " << (i / no) + 1 << ", Point: " << (i % no) + 1 << "\n";
			stream >> first >> second;
			rows2[i] |= xy2bitboard(first, second);
		}
		for(int i = 0; i < no; i++) {
			cout << "Ring: " << i + 1 << "\n";
			stream >> first >> second;
			rings2.push_back(xy2bitboard(first, second));
		}
	}

	ostream &to_stream2(ostream &os) const {
		return os << "Move3: Ring src: " << ring_pos << " Ring dest: " << ring_dest << " and is a type 2 move!\n";
	}

	bool eq2(const YinshMove &rhs) const {
		return rows1 == rhs.rows1 && rings1 == rhs.rings1 &&
				ring_pos == rhs.ring_pos && ring_dest == rhs.ring_dest &&
				rows1 == rhs.rows1 && rings2 == rhs.rings2;
	}

	size_t hash2() const {
		using boost::hash_combine;
		using boost::hash_value;
		size_t seed = 0;

		hash_combine(seed, hash_value(type));
		for (auto row : rows1) {
			hash_combine(seed, hash_value(row));
		}
		for (auto ring : rings1) {
			hash_combine(seed, hash_value(ring));
		}

		hash_combine(seed, hash_value(type));
		hash_combine(seed, hash_value(ring_pos));
		hash_combine(seed, hash_value(ring_dest));

		hash_combine(seed, hash_value(type));
		for (auto row : rows2) {
			hash_combine(seed, hash_value(row));
		}
		for (auto ring : rings2) {
			hash_combine(seed, hash_value(ring));
		}

		return seed;
	}
};

int __move__count__ = 0;
// The following are just for readability
inline YinshMove AddRingMove(uint128_t pos) {
	//if (__move__count__) {
	//	cout << __move__count__ << " AddRing(" << pos << ")\n";
	//}
	__move__count__++;
	return YinshMove(pos);
}
inline YinshMove RemoveRowAddMarkerRemoveRow(std::vector<uint128_t> rows1, std::vector<uint128_t> rings1,
											 uint128_t src, uint128_t dest,
											 std::vector<uint128_t> rows2, std::vector<uint128_t> rings2) {
	//if (__move__count__) {
//		cout << __move__count__ << " RemoveRing(" << (rows1.size() ? rows1[0] : -1) << "," << rows1.size() << ")\t";
//		cout << __move__count__ << " AddMarker(" << src << "," << dest << ")\t";
//		cout << __move__count__ << " RemoveRing(" << (rows2.size() ? rows2[0] : -1) << "," << rows2.size() << ")\n";
	//}
	__move__count__++;
	return YinshMove(rows1, rings1, src, dest, rows2, rings2);
}
/*
inline YinshMove RemoveRingMove(std::vector<uint128_t> rows, std::vector<uint128_t> rings) {
	//if (__move__count__) {
		cout << __move__count__ << " RemoveRing(" << rows[0] << "," << rows.size() << ")\n";
	//}
	__move__count__++;
	return YinshMove(rows, rings);
}
*/
struct Board {
	uint128_t board = 0;

	std::unordered_map<uint128_t, bool> rings;

	Board() {}

	Board(const Board &other) { board = other.board; rings = other.rings; }

	bool operator==(const Board &other) const {
		return board == other.board &&
				rings == other.rings;
	}

	bool isValid(uint128_t x) const { return (x & valid_positions) != 0; }

	bool add_ring(uint128_t ring_pos) {
		// TODO: assert(no_of_rings_placed_cp < 5);
		assert(rings.find(ring_pos) == rings.end());
		// TODO: assert(all_rings.find(ring_pos) != all_rings.end());
		assert((board & ring_pos) == static_cast<uint128_t>(0));

		board |= ring_pos;
		rings[ring_pos] = 1;
		// TODO: all_rings[ring_pos] = 1;
	}

	bool remove_ring(uint128_t ring_pos) {
		if (rings.find(ring_pos) == rings.end()) {
			cout << "lotcha! " << ring_pos << "\n";
		}
		assert(rings.find(ring_pos) != rings.end());
		// assert(all_rings.find(ring_pos) != all_rings.end());

		board &= (~ring_pos);
		rings.erase(ring_pos);
		//all_rings.erase(ring_pos);
	}

	// asserts that there _isn't_ a ring at marker_pos
	bool add_marker(uint128_t marker_pos) {
		assert(rings.find(marker_pos) == rings.end());
		// TODO: assert(all_rings.find(marker_pos) != all_rings.end());
		//all_rings[ring_pos] = 1;
		board |= marker_pos;
	}

	// asserts that there _isn't_ a ring at marker_pos
	bool remove_marker(uint128_t marker_pos) {
		assert(rings.find(marker_pos) == rings.end());
		// TODO: assert(all_rings.find(marker_pos) == all_rings.end());

		board &= (~marker_pos);
	}

	bool remove_row_and_ring(const std::vector<uint128_t>& rows,
							 const std::vector<uint128_t>& rings) {
		uint128_t kill_mask = 0;

		for(auto row: rows) {
			kill_mask |= row;
		}

		for(auto ring: rings) {
			kill_mask |= ring;
		//	cout << "remove row and ring\n";
			remove_ring(ring);
		}

		board &= (~kill_mask);
	}

	bool add_row_and_ring(const std::vector<uint128_t>& rows,
						  const std::vector<uint128_t>& rings) {
		uint128_t save_mask = 0;

		for(auto row: rows) {
			save_mask |= row;
		}

		for(auto ring: rings) {
			save_mask |= ring;
			add_ring(ring);
		}

		board |= (save_mask);
	}
};

bool flip_markers(uint128_t ring_pos, uint128_t ring_dest, Board& board1, Board& board2) {
	assert(flip_bitmasks.count(ring_pos) != 0);
	assert(flip_bitmasks.find(ring_pos)->second.count(ring_dest) != 0);
	auto flip_mask = flip_bitmasks.find(ring_pos)->second.find(ring_dest)->second;
	auto b1 = board1.board & flip_mask;
	auto b2 = board2.board & flip_mask;
	board1.board = (board1.board & (~flip_mask)) | b2;
	board2.board = (board2.board & (~flip_mask)) | b1;
}

size_t hash_value(const Board &board) {
	using boost::hash_combine;
	using boost::hash_value;
	size_t seed = 0;

	hash_combine(seed, hash_value(board.board));

	for (auto ring : board.rings) {
		hash_combine(seed, hash_value(ring));
	}

	return seed;
}

struct YinshState : public State<YinshState, YinshMove> {

	Board board_1, board_2;

	uint64_t no_of_markers_remaining;
	uint64_t no_of_rings_placed_1, no_of_rings_placed_2;
	uint64_t no_of_rings_removed_1, no_of_rings_removed_2;
	std::unordered_map<uint128_t, bool> all_rings;

	// cp stands for current player. These will point to the correct player's variables
	//Board& board_cp;
	//uint64_t& no_of_rings_placed_cp;
	//uint64_t& no_of_rings_removed_cp;
	//std::unordered_map<uint128_t, bool> rings_cp;

	YinshState() : State(PLAYER_1)/*,
				   board_cp(board_1),
				   no_of_rings_placed_cp(no_of_rings_placed_1),
				   no_of_rings_removed_cp(no_of_rings_removed_1),
				   rings_cp(board_1.rings) */{
		no_of_markers_remaining = 51;
		no_of_rings_placed_1 = 0;
		no_of_rings_placed_2 = 0;
		no_of_rings_removed_1 = 0;
		no_of_rings_removed_2 = 0;
	}

	/*YinshState (const YinshState &other) override {
		board_1 = other.board_1;
		board_2 = other.board_2;
		player_to_move = other.player_to_move;
		no_of_markers_remaining = other.no_of_markers_remaining;
		no_of_rings_placed_1 = other.no_of_rings_placed_1;
		no_of_rings_placed_2 = other.no_of_rings_placed_2;
		no_of_rings_removed_1 = other.no_of_rings_removed_1;
		no_of_rings_removed_2 = other.no_of_rings_removed_2;
		player_to_move = other.player_to_move;
		set_player_vars();
	}*/

	YinshState operator=(const YinshState &other) {
		board_1 = other.board_1;
		board_2 = other.board_2;
		player_to_move = other.player_to_move;
		no_of_markers_remaining = other.no_of_markers_remaining;
		no_of_rings_placed_1 = other.no_of_rings_placed_1;
		no_of_rings_placed_2 = other.no_of_rings_placed_2;
		no_of_rings_removed_1 = other.no_of_rings_removed_1;
		no_of_rings_removed_2 = other.no_of_rings_removed_2;
		player_to_move = other.player_to_move;
		//set_player_vars();
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
		clone.player_to_move = player_to_move;
		//clone.set_player_vars();
		return clone;
	}

	/*void set_player_vars(int pl = -1) {
		if (pl == -1)
			pl = player_to_move;

/*		if (pl == PLAYER_1) {
			board_cp = board_1;
			no_of_rings_placed_cp = no_of_rings_placed_1;
			no_of_rings_removed_cp = no_of_rings_removed_1;
			rings_cp = board_1.rings;
		} else {
			board_cp = board_2;
			no_of_rings_placed_cp = no_of_rings_placed_2;
			no_of_rings_removed_cp = no_of_rings_removed_2;
			rings_cp = board_2.rings;
		}* /


	/*template<class T>
	T& get_cp_var(T& a, T& b) const {
		return (player_to_move == PLAYER_1) ? a : b;
	}*/

	int get_goodness() const override { return rand() % 100; }

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
						int last = choices.size()-1;

						choices[i].erase(choices[i].begin() + k);
						choices[last].erase(choices[last].begin() + j);

						goto split;
					}
				}
			}
			i++;
			split:
				continue;
		}
	}

	// Not handling rings < rows
	// need to handle only cases for which k = 1 or 2.
	// if k = 3, then just return any three rings. This function will be called
	// only to choose k rings for removing along with k rows. If 3 rows are being removed
	// we're done anyway
	auto combinations(int k, const std::unordered_map<uint128_t, bool>& rings) const {
		std::vector<std::vector<uint128_t>> ret;
		if (k == 0) {
			return ret;
		}
		if (k == 1) {
			for (auto const& kv : rings) {
				ret.push_back({ kv.first });
			}
			return ret;
		}
		if (k == 2) {
			std::vector<uint128_t> rings_vec;
			for (auto it = rings.begin(); it != rings.end(); it++) {
				rings_vec.push_back(it->first);
			}

			for (int i = 0; i < rings_vec.size(); i++) {
				for (int j = i+1; j < rings_vec.size(); j++) {
					ret.push_back({ rings_vec[i], rings_vec[j] });
				}
			}

			return ret;
		}
		// k > 2? just return one choice, with k rings.
		ret.push_back(std::vector<uint128_t>());
		for(auto it = rings.begin(); it != rings.end() && k > 0; it++, k--) {
			ret[0].push_back(it->first);
		}
		return ret;
	}

	auto get_rows_formed(const Board& board) const {
		std::unordered_map<uint128_t, bool> rows_formed;
		auto bitboard = board.board;
		for (const auto& kv : board.rings)
			bitboard &= (~kv.first);

		for(auto& outer_map: five_in_a_row_bitmasks) {
			for(auto& inner_map: outer_map.second) {
				auto c1 = bitboard & inner_map.second;
				if(c1 == inner_map.second) {
					uint128_t row_mask = inner_map.second;
					rows_formed[row_mask] = true;
				}
			}
		}

		return rows_formed;
	}

	typedef std::vector<std::pair<std::vector<uint128_t>, std::vector<uint128_t> > > ring_row_choices_t;

	// returns a vector of pairs. Each pair is (row_choice, ring_choice)
	// where row_choice and ring_choice is a vector. Each pair denotes a valid RemoveRingMove
	// row_choice.size() == ring_choice.size();
	auto get_ring_row_choices(const Board& board, std::unordered_map<uint128_t, bool> rows_formed) const {
		int k = 0;

		// default value is a choice
		ring_row_choices_t choices;

		if (rows_formed.size() == 0) {
			choices = { make_pair(vector<uint128_t>(), vector<uint128_t>()) };
			return choices;
		}

		std::vector<std::vector<uint128_t> > row_choices(1);
		for (const auto& kv: rows_formed) {
			row_choices[0].push_back(kv.first);
		}

		get_non_intersecting_rows(row_choices);
		std::sort(row_choices.begin(), row_choices.end(),
			[](const vector<uint128_t> & a, const vector<uint128_t> & b){ return a.size() < b.size(); });

		std::vector<std::vector<uint128_t> > all_sets;
		for(auto& row_choice: row_choices) {
			if(k != row_choice.size()) {
				k = row_choice.size();
				all_sets.clear();
				//combinations(0, k, rings_cp, all_sets);
				all_sets = combinations(k, board.rings);
			}
			for(int i = 0; i < all_sets.size(); i++) {
				choices.push_back(make_pair(all_sets[i], row_choice));
			}
		}

		return choices;
	}

	std::vector<YinshMove> get_legal_moves(int max_moves = INF) const override {
//		cout << "Have to get legal moves\n";
		auto combined_board = board_1.board | board_2.board;

		std::vector<YinshMove> moves;
		auto no_rings_placed = (player_to_move == PLAYER_1 ? no_of_rings_placed_1 : no_of_rings_placed_2);
		if(no_rings_placed < 5) {
			int count = 1;
			for(uint128_t i = 1; count < 128; i <<= 1, count++) {
				auto c1 = i & combined_board;
				if(c1 == 0 &&
				   board_1.isValid(i)) {
					moves.push_back(AddRingMove(i));
				}
			}
//			cout << "Done get_legal_moves1\n";
			return moves;
		}

		// calculate the rows formed.
		// generate all possible places where you can move the ring
		// calculate the rows formed for each placement of marker-ring pair
		// permute the rows formed before placing the marker, positions of the marker-ring pair, and rows formed after placing them
		auto rows_formed = get_rows_formed((player_to_move == PLAYER_1 ? board_1 : board_2));
		const ring_row_choices_t ring_row_choices_before = get_ring_row_choices((player_to_move == PLAYER_1 ? board_1 : board_2), rows_formed);
		std::vector<std::pair<uint128_t, uint128_t> > marker_moves;

		for (auto& old_ring_row_choice : ring_row_choices_before) {
			// for each ring check in all directions - check wherever the ring can move to
			// for each such possible position, we can get a move by
			auto board_copy = (player_to_move == PLAYER_1 ? board_1 : board_2);
			auto enemy_board_copy = (player_to_move == PLAYER_1 ? board_2 : board_1);
			board_copy.remove_row_and_ring(old_ring_row_choice.second, old_ring_row_choice.first);

			for(const auto& ringkv: board_copy.rings) {
				for (auto dir: directions) {
					auto ring = ringkv.first;
					bool jump = false;
					while(hasNext(dir, ring)) {
						auto next = getNext(dir, ring);
						auto c1 = combined_board & next;

						assert(c1 == 0 || c1 == next); // either the position is occupied, or it isn't.

						// three cases:
						// 1. location is empty
						if(c1 == 0) {
							// markermove is (ringkv.first, next);
							//cout << "simulating remove_ring\n";
							auto board_copy2 = board_copy;
							auto enemy_board_copy2 = (player_to_move == PLAYER_1 ? board_2 : board_1);
							board_copy2.remove_ring(ringkv.first);
							board_copy2.add_marker(ringkv.first);
							//cout << "simulated remove_ring\n";
			//				cout << "Might fail for " << marker_move.first << " " << marker_move.second << "\n";
							flip_markers(ringkv.first, next, board_copy2, enemy_board_copy2);
							board_copy2.add_ring(next);

							auto new_rows_formed = get_rows_formed(board_copy2);
							auto new_ring_row_choices = get_ring_row_choices(board_copy2, new_rows_formed);

							for (auto& new_ring_row_choice: new_ring_row_choices) {
								moves.push_back(
									RemoveRowAddMarkerRemoveRow(
										old_ring_row_choice.second,
										old_ring_row_choice.first,
										ringkv.first,
										next,
										new_ring_row_choice.second,
										new_ring_row_choice.first
									)
								);
							}

							if (jump) {
								break;
							}
						}
						// 2. a ring already exists there
						else if (c1 == next && (board_copy.rings.count(next) > 0 || enemy_board_copy.rings.count(next) > 0)) {
							break;
						}
						// 3. a marker already exists there
						else if(c1 == next && (board_copy.rings.count(next) == 0 && enemy_board_copy.rings.count(next) == 0)) {
							if(!jump) {
								jump = true;
							}
						}

						ring = next;
					}
				}
			}
		}

//		cout << "Done get_legal_moves2\n";
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
		uint64_t no_of_rings_removed = (player_to_move == PLAYER_1 ? no_of_rings_removed_1 : no_of_rings_removed_2);
		if (no_of_rings_removed >= 3)
			return true;
		return false;
	}


	void make_move(const YinshMove& move) override {
		//cout << "gotta make move\n";
		auto& board_cp = (player_to_move == PLAYER_1 ? board_1 : board_2);
		auto& board_enemy = (player_to_move == PLAYER_1 ? board_2 : board_1);

		int type = move.type;
		switch(type) {
			case 1:		{
				//cout << "Adding " << move.ring_pos << " on " << player_to_move << " " << board_cp.board << "\n";
				board_cp.add_ring(move.ring_pos);
				//cout << "ADDED " << move.ring_pos << " on " << player_to_move << " " << board_cp.board << "\n";
				player_to_move == PLAYER_1 ? no_of_rings_placed_1++ : no_of_rings_placed_2++;
				break;
			}
			case 2: 	{
//				cout << "Move2" << "\n";
				board_cp.remove_row_and_ring(move.rows1, move.rings1);
//				cout << "making move row and ring\n";
				board_cp.remove_ring(move.ring_pos);
//				cout << "made move move row and ring\n";
				board_cp.add_marker(move.ring_pos); // do it after remove_ring!
				board_cp.add_ring(move.ring_dest);
				flip_markers(move.ring_pos, move.ring_dest, board_cp, board_enemy);
				board_cp.remove_row_and_ring(move.rows2, move.rings2);
//				cout << "made made move row and ring\n";
				no_of_markers_remaining = no_of_markers_remaining - 1 + 5 * (move.rows1.size() + move.rows2.size());
				if (player_to_move == PLAYER_1)
					no_of_rings_removed_1 += move.rows1.size() + move.rows2.size();
				else
					no_of_rings_removed_2 += move.rows1.size() + move.rows2.size();
				break;
			}
			default:	assert(false);
		}

		player_to_move = get_enemy(player_to_move);
		//cout << "Done make move\n";
	}

	void undo_move(const YinshMove& move) override {
		//cout << "gotta undo move\n";
		player_to_move = get_enemy(player_to_move);
		auto& board_cp = (player_to_move == PLAYER_1 ? board_1 : board_2);
		auto& board_enemy = (player_to_move == PLAYER_1 ? board_2 : board_1);

		int type = move.type;
		switch(type) {
			case 1:		{
				//cout << "Removing " << move.ring_pos << " on " << (board_cp == board_1 ? "1" : "2") << " " << board_cp.board << "\n";
				board_cp.remove_ring(move.ring_pos);
				//cout << "REMOVED " << move.ring_pos << " on " << (board_cp == board_1 ? "1" : "2") << " " << board_cp.board << "\n";
				player_to_move == PLAYER_1 ? no_of_rings_placed_1-- : no_of_rings_placed_2--;
				break;
			}
			case 2: 	{
				board_cp.add_row_and_ring(move.rows2, move.rings2);
				flip_markers(move.ring_dest, move.ring_pos, board_cp, board_enemy);
//				cout << "undoing row and ring\n";
				board_cp.remove_ring(move.ring_dest);
//				cout << "undone row and ring\n";
				board_cp.remove_marker(move.ring_pos);
				board_cp.add_ring(move.ring_pos);
				board_cp.add_row_and_ring(move.rows1, move.rings1);
				no_of_markers_remaining = no_of_markers_remaining + 1 - 5 * (move.rows1.size() + move.rows2.size());
				if (player_to_move == PLAYER_1)
					no_of_rings_removed_1 -= move.rows1.size() + move.rows2.size();
				else
					no_of_rings_removed_2 -= move.rows1.size() + move.rows2.size();
				break;
			}
			default:	assert(false);
		}
		//cout << "Done undo move\n";
	}

	ostream &to_stream(ostream &os) const override {
		/*os << "Hello world " << __move__count__ << "\n";
		return os;*/
		os << "\033[2J";
		for (int i = 0; i <= 19; i++) {
			for (int j = 0; j <= 11; j++) {
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

				if(isValidXYCoord(xy_coord_t(i - 1, j-1))) {
					uint128_t p = xy2bitboard(i - 1, j-1);
					if(p & board_1.board) {
						if(board_1.rings.count(p) > 0) {
							os << whiteRingFormat;
							//break;
						}
						else {
							os << whiteMarkerFormat;
							//break;
						}
					}
					else if(p & board_2.board) {
						if(board_2.rings.count(p) > 0) {
							os << blackRingFormat;
							//break;
						}
						else {
							os << blackMarkerFormat;
							//break;
						}
					}
					else os << emptyFormat;
					//break;
				}
				else {
					os << blankFormat;
					//break;
				}
			}
			os << "\n\n";
		}
		os << "Player to move: " << player_to_move << "\t";
		os << "P1 rings placed: " << no_of_rings_placed_1 << "\t";
		os << "P1 rings removed: " << no_of_rings_removed_1 << "\t";
		os << "P2 rings placed: " << no_of_rings_placed_2 << "\t";
		os << "P2 rings removed: " << no_of_rings_removed_2 << "\t";
		os << "Markers remaining: " << no_of_markers_remaining << "\t";
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
			   player_to_move == other.player_to_move;
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
		return seed;
	}
};
