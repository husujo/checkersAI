#ifndef STATESEARCH_H
#define STATESEARCH_H

#define TTABLE

#include "board.hpp"
#include <unordered_map>

using namespace std;

struct Transposition {
	double value; // the value of this board state
	short rnd; // the round number this value was generated
};

Move makeMove(Board &pstate, short min_max);

Move calculateMove(Board &b, short depth, short min_max, short to_move);

int getValue(Board &pstate, short depth, short min_max, short to_move, double a, double b);

extern unordered_map<long,Transposition> TTable[2];


#endif
