
#ifndef PIECE_H
#define PIECE_H

#include <sstream>
#include <string>

using namespace std;

enum piece{
    empty,
    red,
    redk,
    white,
    whitek,
    padded
};

extern string piece_strings[]; // extern here is actually necessary

// extern string piece_strings;


struct CheckersPiece {
	piece p;
	int row, file;
	int color; // white = 1, red = -1
	CheckersPiece(piece pc, int r, int f, int c);
	string toString();
	int getValue();
	// CheckersPiece(CheckersPiece &c);
};

#endif
