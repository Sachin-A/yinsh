#ifndef yinsh_mappings
#define yinsh_mappings

#include <unordered_map>
#include "./uint128.h"

/*
    Two mappings:

    (x,y) <==> 128bit number
*/

struct xy_coord_t {
    int x, y;
    xy_coord_t(): x(0), y(0) {}
    xy_coord_t(int _x, int _y) :x(_x), y(_y) {}

    xy_coord_t operator+(const xy_coord_t &other) const
    {
        xy_coord_t p = { x+other.x, y+other.y };
        return p;
    }
    xy_coord_t operator*(const int k) const
    {
        xy_coord_t p = { k*x, k*y };
        return p;
    }
    bool operator==(const xy_coord_t &other) const
    {
        return (x == other.x && y == other.y);
    }
};
xy_coord_t N(-2,0), NE(-1,1), SE(1,1), S(2,0), SW(1,-1), NW(-1,-1);
xy_coord_t directions[] = { N, NE, SE, S, SW, NW };

namespace std {
// hash for xy_coord_t
template<>
struct hash<xy_coord_t> {
    std::size_t operator()(const xy_coord_t& p) const
    {
        return 19*p.x + p.y; // this will always be unique for our points!
    }
};

// hash for uint128_t
template<>
struct hash<uint128_t> {
    std::size_t operator()(const uint128_t& x) const
    {
        return (static_cast<unsigned long long>(x >> 64)) ^ (static_cast<unsigned long long>(x));
    }
};
}

typedef uint128_t bitboard_coord_t;

const int E = 1; // just for visually showing the board in syntax-highlighting editors
int xyCoordsBoard[19][11] = {
//   0  1  2  3  4  5* 6  7  8  9  10
    {0, 0, 0, 0, E, 0, E, 0, 0, 0, 0}, // 0
    {0, 0, 0, E, 0, E, 0, E, 0, 0, 0}, // 1
    {0, 0, E, 0, E, 0, E, 0, E, 0, 0}, // 2
    {0, E, 0, E, 0, E, 0, E, 0, E, 0}, // 3
    {0, 0, E, 0, E, 0, E, 0, E, 0, 0}, // 4
    {0, E, 0, E, 0, E, 0, E, 0, E, 0}, // 5
    {E, 0, E, 0, E, 0, E, 0, E, 0, E}, // 6
    {0, E, 0, E, 0, E, 0, E, 0, E, 0}, // 7
    {E, 0, E, 0, E, 0, E, 0, E, 0, E}, // 8
    {0, E, 0, E, 0, E, 0, E, 0, E, 0}, // 9**********
    {E, 0, E, 0, E, 0, E, 0, E, 0, E}, // 10
    {0, E, 0, E, 0, E, 0, E, 0, E, 0}, // 11
    {E, 0, E, 0, E, 0, E, 0, E, 0, E}, // 12
    {0, E, 0, E, 0, E, 0, E, 0, E, 0}, // 13
    {0, 0, E, 0, E, 0, E, 0, E, 0, 0}, // 14
    {0, E, 0, E, 0, E, 0, E, 0, E, 0}, // 15
    {0, 0, E, 0, E, 0, E, 0, E, 0, 0}, // 16
    {0, 0, 0, E, 0, E, 0, E, 0, 0, 0}, // 17
    {0, 0, 0, 0, E, 0, E, 0, 0, 0, 0}  // 18
};

bool isValidXYCoord(const xy_coord_t &p)
{
    return (0 <= p.x && 19 > p.x &&
            0 <= p.y && 11 > p.y &&
            xyCoordsBoard[p.x][p.y] == E);
}

const unordered_map<xy_coord_t, bitboard_coord_t> xy2BitboardMap = [] {
    unordered_map<xy_coord_t, bitboard_coord_t> m;

    int count = 0;
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 11; j++) {
            if (xyCoordsBoard[i][j] == E) {
                const xy_coord_t p = { i, j };
                m[ p ] = (static_cast<bitboard_coord_t>(1) << (count++));
            }
        }
    }
    return m;
}();

const unordered_map<bitboard_coord_t, xy_coord_t> bitboard2XYMap = [] {
    unordered_map<bitboard_coord_t, xy_coord_t> m;
    for (auto it = xy2BitboardMap.begin(); it != xy2BitboardMap.end(); it++) {
        m[it->second] = it->first;
    }
    return m;
}();

// given start and end location, gives a bitmask with only those bits set which correspond to the positions
// in between (exclusive) start and end. start and end must be positions where the new marker is being added and where the
// ring will be placed next respectively
// e.g. flip_bitmasks[start][end] => gives bitmask of all points between start and end (exclusive of both)
const unordered_map<bitboard_coord_t, unordered_map<bitboard_coord_t, bitboard_coord_t>> flip_bitmasks = [] {
    unordered_map<bitboard_coord_t, unordered_map<bitboard_coord_t, bitboard_coord_t>> m;
    
    // logic: loop through each valid point, and then in each direction go as far as you can, add bitmasks while doing this

    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 11; j++) {
            if (xyCoordsBoard[i][j] != E)
                continue;

            xy_coord_t start(i, j);
            auto start_bb = xy2BitboardMap.find(start)->second;

            uint128_t bitmask = 0;
            for (auto dir : directions) {
                auto next = start + dir;
                xy_coord_t prev(0, 0); // 0 means no prev point (ugly special case to maintain exclusive boundaries)

                while (isValidXYCoord(next)) {
                    if (prev.x == 0 && prev.y == 0) {
                        // bitmask stays 0
                    } else {
                        auto prev_bb = xy2BitboardMap.find(prev)->second;
                        bitmask |= prev_bb;
                    }
                    auto next_bb = xy2BitboardMap.find(next)->second;
                    m[start_bb][next_bb] = bitmask;
                    prev = next;
                    next = next + dir;
                }
            }
        }
    }

    return m;
}();

// given start and end location, gives a bitmask with only those bits set which correspond to the positions
// in between (inclusive) start and end. start and end are 5-units apart. So if you iterator through this map,
// you can list all possible 5-in-a-row bitmasks.
// e.g. five_in_row_bitmasks[start][end] => gives bitmask of all points between start and end inclusive of both
const unordered_map<bitboard_coord_t, unordered_map<bitboard_coord_t, bitboard_coord_t>> five_in_a_row_bitmasks = [] {
    unordered_map<bitboard_coord_t, unordered_map<bitboard_coord_t, bitboard_coord_t>> m;
    
    // logic: loop through each valid point, and then in each direction go as far as you can, add bitmasks if dist=5
    // while doing this

    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 11; j++) {
            if (xyCoordsBoard[i][j] != E)
                continue;

            xy_coord_t start(i, j);
            auto start_bb = xy2BitboardMap.find(start)->second;

            uint128_t bitmask = 0;
            for (auto dir : directions) {
                auto end = start + dir*4;
                if (!isValidXYCoord(end)) {
                    continue;
                }
                for (int k = 0; k < 5; k++) {
                    auto bb_point = xy2BitboardMap.find(start + dir*k)->second;
                    bitmask |= bb_point;
                }
                auto end_bb = xy2BitboardMap.find(end)->second;
                m[start_bb][end_bb] = bitmask;
            }
        }
    }

    return m;
}();

// returns the element following p in d direction: next_element[d][p]
const unordered_map<xy_coord_t, unordered_map<bitboard_coord_t, bitboard_coord_t>> next_element = [] {
    unordered_map<xy_coord_t, unordered_map<bitboard_coord_t, bitboard_coord_t>> m;
    
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 11; j++) {
            if (xyCoordsBoard[i][j] != E)
                continue;

            xy_coord_t start(i, j);
            auto start_bb = xy2BitboardMap.find(start)->second;

            uint128_t bitmask = 0;
            for (auto dir : directions) {
                auto next = start + dir;

                while (isValidXYCoord(next)) {
                    auto next_bb = xy2BitboardMap.find(next)->second;
                    m[dir][start_bb] = next_bb;
                    next = next + dir;
                }
            }
        }
    }

    return m;
}();

/*
    Utilities
*/

// Should be called only if p is known to be a valid xy coord
bitboard_coord_t xycoord2bitboard(const xy_coord_t &p) {
    assert(xy2BitboardMap.find(p) != xy2BitboardMap.end());
    return xy2BitboardMap.find(p)->second;
}

bitboard_coord_t xy2bitboard(const int& x, const int& y) {
    xy_coord_t p(x, y);
    assert(xy2BitboardMap.find(p) != xy2BitboardMap.end());
    return xy2BitboardMap.find(p)->second;
}

// Should be called only if p is known to be a valid bitboard coord
xy_coord_t bitboard2xy(const bitboard_coord_t& p) {
    assert(bitboard2XYMap.find(p) != bitboard2XYMap.end());
    return bitboard2XYMap.find(p)->second;
}

// check if next_element[dir][p] exists
bool hasNext(const xy_coord_t& dir, const bitboard_coord_t& p) {
    assert(next_element.find(dir) != next_element.end()); // direction provided should always be valid
    return next_element.find(dir)->second.count(p) > 0;
}

// call only if you're sure next_element[dir][p] exists
bitboard_coord_t getNext(const xy_coord_t& dir, const bitboard_coord_t& p) {
    assert(hasNext(dir, p));
    return next_element.find(dir)->second.find(p)->second;
}
#endif