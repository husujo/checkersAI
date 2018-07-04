#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include "piece.hpp"
#include "move.hpp"
#include <forward_list> 
#include <algorithm> // copy


#define BSIZE 8
#define ISIZE (BSIZE + 2) // +2 for 1 border all around

using namespace std;


struct Board {

	char board[ISIZE][ISIZE];

	long hash;

	// generate bit strings for: empty, red, redk, white, whitek, padded
	static long bitstringtable[ISIZE][ISIZE][6]; // random bitstrings for the Zhash
	static int num;

	// Board *parent;
	short value; // which player does this state favor, and by how much?
	short colorToMove; // 1 = white, -1 = red
	forward_list<CheckersPiece> white_plist; // white piece list
	forward_list<CheckersPiece> black_plist; // black piece list
	forward_list<Move> possibleMoves;

	bool gameover;


	Board();

    Board(Board &pstate, Move &m);

    Board(char pboard[10][10], short color);


	int numPieces();

    // Move *calculateMove(int color);

    void apply(Move &m, short color);

    bool sendVerification(Move &mm);

    void printBoard();

    void fillPossibleMoves();
	
	// template <typename T> void printList(forward_list<T> l);

// template function must be defined in .hpp file for some reason
template <typename T>
void printList(forward_list<T> l) {
	cout << "printing list" << endl;
	// why typename??
	for (typename forward_list<T>::iterator it = l.begin(); it != l.end(); ++it ) {
		cout << it->toString() << endl;
	}
}


	bool doubleJumpHelper(short row, short file, string sofar, bool canAdd, piece p);

	bool doubleJumpHelperKing(short row, short file, string sofar, bool canAdd, piece p, char board[10][10]);

	// calculate the value of the board
	double staticEvaluation();

	// fill the bitstring table
	void generateBitstrings();

	// hash this board from scratch
	long Zhash();

	// update the zhash value with a move
	long updateZhash(Move &m);

private:
    /*
      ideas
      8x8 Board using the enumeration above as piece names. Should make coding simpler.
      TODO: Improve cache effectiveness by reducing the board size (using 3-4bits per square only?. (8 bit currently)
      	=> Move logic possibly becomes significantly more complicated. Worth it?
      Use two extra spaces in each side for padding.      ///// I think we only need 1 extra padding boundary?
     */
};



#endif



