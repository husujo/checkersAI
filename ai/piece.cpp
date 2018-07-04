#include "piece.hpp"



string piece_strings[] = {"_","red","redk","white","whitek","X"};

// string piece_strings = "Hello World";


CheckersPiece::CheckersPiece(piece pc, int r, int f, int c) {
		p = pc;
		row = r;
		file = f;
		color = c;
}
string CheckersPiece::toString() {
		stringstream ss;
		ss << piece_strings[p] << (file-1) << (row-1);
		return ss.str();
}


//TODO count center as more? sides as more?
int CheckersPiece::getValue() {

	int myval;

	switch (p) {
		case white:
			myval = 10;
			break;
		case red:
			myval = -10;
			break;
		case whitek:
			myval = 19;
			// if (row > 3 && row < 6 && file > 2 && file < 7) {
			// 	myval += 0.1;
			// }
			break;
		case redk:
			myval = -19;
			// if (row > 3 && row < 6 && file > 2 && file < 7) {
			// 	myval -= 0.1;
			// }
			break;
		default:
			myval = 0;
			break;
	}


	return myval;
}

// CheckersPiece::CheckersPiece(CheckersPiece &c) {
// 	this->p = c.p;
// 	this->row = c.row;
// 	this->file = c.file;
// 	this->color = c.color;
// }
