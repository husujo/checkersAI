#include <iostream>
#include <fstream>
#include "board.hpp"
#include "stateSearch.hpp"
#include <unistd.h>

using namespace std;

void exportBoard(Board &CheckerBoard, string name);

void log(string filename, string msg);

template <typename T> string exportList(forward_list<T> l);


int main(int argc, char ** argv) {

    Board CheckerBoard = Board();

    bool web = false;
	if (argc > 1) {
		web=true;
	}

	string input,output; // moves made by player, moves made by the ai
	// int AIcolor = b;

	exportBoard(CheckerBoard, "logs/position.txt");

	Move m;
	while (true) {

		if (web) { log("logs/moves.txt",exportList(CheckerBoard.possibleMoves)); }

		// get player's move from input
		do {
			cin >> input; 
			m = Move(input);
		} while (!CheckerBoard.sendVerification(m));
		cout << "valid move" << endl;
		
		CheckerBoard = Board(CheckerBoard,m); // apply the move

		if (CheckerBoard.gameover == true) {
			usleep(10000);
			cout << "gameover" << endl;
			exit(0);
		}
		exportBoard(CheckerBoard, "logs/position.txt");
		if (!web) {
			CheckerBoard.printBoard();
		} else {
			log("logs/moves2.txt",exportList(CheckerBoard.possibleMoves));
		}

		// cout << "getting ai move" << endl;
		// get the AI player's move
		m = makeMove(CheckerBoard,-1); // alpha beta state search,
		
		// cout << "ai making move: " << m->toString() << endl;

		// cout << "applying ai move" << endl;
		CheckerBoard = Board(CheckerBoard,m); // make computer's move
		// cout << "after creating new ai state" << endl;
		cout << m.toString() << endl;
		if (CheckerBoard.gameover == true) {
			// cout << "gameover" << endl;
			exit(0);
		}

		exportBoard(CheckerBoard, "logs/position2.txt");
		if (!web) {
			CheckerBoard.printBoard();
		}

		// send the move the AI has made
		// cout << m->toString() << endl;
	}



	return 0;
}






void exportBoard(Board &CheckerBoard, string name) {
		ofstream outputFile;
		outputFile.open(name);

		// cout << "exporting board" << endl;
		// outputFile << "   01234567   " << endl;
		// outputFile << "              " << endl;
		for (int i = 9; i >=0; i--) {
			// outputFile << 9-i << "  ";
			for (int j = 0; j < 9; ++j) {
				if (CheckerBoard.board[i][j] == red) { // red
					outputFile << "\e[1m" << "p" << "\e[0m";
				} else if (CheckerBoard.board[i][j] == white) {
					outputFile << "p";
				} else if (CheckerBoard.board[i][j] == redk) {
					outputFile << "\e[1m" << "k" << "\e[0m";
				} else if (CheckerBoard.board[i][j] == whitek) {
					outputFile << "k";
				}

				else { // white
					outputFile << " ";
				} 
				// else empty
				
			}
			outputFile << endl;
			// outputFile << "  " << 9-i << endl;
		}
		// outputFile << "              " << endl;
		// outputFile << "   01234567   " << endl;
		outputFile.close();

	}


// 
// string exportList(forward_list<T> l) {
// 	stringstream ss;
// 	for (typename forward_list<T>::iterator it = l.begin(); it != l.end(); ++it ) {
// 		ss << it.toString() << ", ";
// 	}
// 	return ss.str();
// }

template <typename T>
string exportList(forward_list<T> l) {
	stringstream ss;
	for (T t : l) {
		ss << t.toString() << ", ";
	}
	return ss.str();
}

void log (string filename, string msg) {
	ofstream outputFile;
	outputFile.open(filename);
	outputFile << msg << endl;
	outputFile.close();
}











