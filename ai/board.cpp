#include "board.hpp"

using namespace std;

// this has to be here to initialize static vars
long Board::bitstringtable[ISIZE][ISIZE][6] = {};
int Board::num = 0;
 
Board::Board(){

	gameover = false;
	value = 0;
    
    for (int i = 1; i <= BSIZE; i++) {
    	for (int j = 1; j <= BSIZE; j++) {
			board[i][j] = empty;
		}
    }
	
    for (int i = 0; i < ISIZE; i++) {
		board[0][i] = padded;
		board[ISIZE-1][i] = padded;
		board[i][0] = padded;
		board[i][ISIZE-1] = padded;
    }

    // WHITE STARTS ON THE LOW-NUMBERED ROWS
    //For each column, fill in the red and white pieces.
    for (int j = 1; j <=8; j++) {
    	
       	if (j % 2 !=0) {
		    board[1][j] = white;
		    white_plist.push_front(CheckersPiece(white,1,j,1));
		    board[3][j] = white;
		    white_plist.push_front(CheckersPiece(white,3,j,1));
		    board[7][j] = red;
		    black_plist.push_front(CheckersPiece(red,7,j,-1));
		} else {
		    board[2][j] = white;
		    white_plist.push_front(CheckersPiece(white,2,j,1));
		    board[6][j] = red;
		    black_plist.push_front(CheckersPiece(red,6,j,-1));
		    board[8][j] = red;
		    black_plist.push_front(CheckersPiece(red,8,j,-1));
		}
    }

    // do the hash, check the values
    generateBitstrings();
    hash = Zhash();
    cout << "DEBUG \t\t" << hash << endl;
    cout << "DEBUG bitstrings: " << endl;
    for (int i=1; i<ISIZE-1; i++) {
    	for (int j=1; j<ISIZE-1; j++) {
    		for (int k=1; k<5; k++) {
    			cout << "DEBUG " << i << "," << j << " " << piece_strings[k] << "  :\t" << bitstringtable[i][j][k] << endl;
    		}
    	}
    }

    colorToMove = 1;
    // parent = NULL;
	fillPossibleMoves();

	// printPieceList(white_plist);
}

Board::Board(Board &pstate, Move &m) {

	gameover = false;
	// parent = pstate;

	copy(&(pstate.board)[0][0],&(pstate.board)[0][0]+100,&board[0][0]); // copy this board into this.board

	// recalculate the hash
	// hash = updateZhash(m);
	

	for (CheckersPiece cp : pstate.white_plist) {
		white_plist.push_front(cp /*CheckersPiece(cp.p,cp.row,cp.file,cp.color) */ );
	}

	for (CheckersPiece cp : pstate.black_plist) {
		black_plist.push_front(cp);
	}

	// check if m is a valid move for the parent state
	bool valid = false;

	for (Move mm : pstate.possibleMoves) {
		if (mm.toString()==m.toString()) {
			valid = true;
			break;
		}
	}


	if (!valid) {
		cout << "invalid move " << m.toString() << " from parent" << endl;
		exit(1);
	}

	apply(m,pstate.colorToMove); // applying the move to the state

	hash = Zhash();


	colorToMove = -(pstate.colorToMove);
	fillPossibleMoves();

	


}

				// char[][], 	color to move
Board::Board(char pboard[10][10], short color) {
	gameover = false;
	copy(&(pboard)[0][0],&(pboard)[0][0]+100,&board[0][0]);
	// get the pieces from the board

	for(int i=0; i<10; i++) {
		for (int j = 0; j < 10; ++j) {
			switch (board[i][j]) {
				case white:
					white_plist.push_front(CheckersPiece(white,i-1,j-1,1));
					break;
				case red:
					black_plist.push_front(CheckersPiece(red,i-1,j-1,-1));
					break;
				case whitek:
					white_plist.push_front(CheckersPiece(whitek,i-1,j-1,1));
					break;
				case redk:
					black_plist.push_front(CheckersPiece(redk,i-1,j-1,-1));
					break;
			}
		}
	}


	colorToMove = color;
	fillPossibleMoves();
}

int Board::numPieces() {
	int total = 0;
	for (CheckersPiece &cp : white_plist) {
		total++;
	}
	for (CheckersPiece &cp : black_plist) {
		total++;
	}
	return total;
}


// TODO could be a problem here
void Board::apply(Move &m, short color) {  // check validity & make move
	
	// TODO might need to check this
	piece boardpiece = (piece)board[m.start_row][m.start_file];

	// if (m->toString()=="33->24") {
	// 	cout << "trying to move piece " << piece_strings[boardpiece] << endl;
	// }
	// cout << "trying to move piece " << piece_strings[boardpiece] << endl;

	// TODO this makes it gone, but doesn't put it into it's new place
	board[m.start_row][m.start_file] = empty;


	int srow = m.start_row;
	int sfile = m.start_file;
	int drow,dfile;
	int rrow,rfile; // coords of piece to remove

	// remove the piece from the piece list if one's taken
	if (m.takes==true) {

		// find every piece that was taken:
		for (int i=0; i < m.num_moves; i++) {
			// calculate the coordinates of the piece taken
			drow = m.dest_rows[i];
			dfile = m.dest_files[i];

			rrow = (srow+drow)/2;
			rfile = (sfile+dfile)/2;

			// cout << "searching for taken piece with row=" << rrow << " and file=" << rfile << endl;


			// this is needed for erase_after. this will keep track of the previous iterator position, so if we need to erase the current element, 
			// we call erase_after(prev_it)
			forward_list<CheckersPiece>::iterator prev_it;	
			int first = 1;

			for (forward_list<CheckersPiece>::iterator it = (color==1)?black_plist.begin():white_plist.begin(); it != ((color==1)?black_plist.end():white_plist.end()); ++it ) {
			// for (CheckersPiece cp : (color==1)?black_plist:white_plist) {
				CheckersPiece cp = (*it);

				// CheckersPiece cp = CheckersPiece()

				if (cp.row==rrow && cp.file==rfile) {
					// cout << "removing piece: " << temp->toString() << endl;

					if (color==1) {
						if (cp.p==red) {
							value += 10;
						} else if (cp.p==redk) {
							value += 20;
						}
						if (first==1) {
							black_plist.pop_front();
						} else {
							// black_plist.remove(cp);
							black_plist.erase_after(prev_it);
						}
						

					} else {
						if (cp.p==white) {
							value -= 10;
						} else if (cp.p==whitek) {
							value -= 20;
						}
						if (first==1) {
							white_plist.pop_front();
							first=0;
						} else {
							// white_plist.remove(cp);
							white_plist.erase_after(prev_it);
						}
					}
					

					board[rrow][rfile] = empty;
					break;
				}

				first = 0;
				prev_it = it;
			}


			srow = drow;
			sfile = dfile;

		}
	}

	// update the coords of this piece in the list

	// cout << "searching for piece to update with row=" << m.start_row-1 << " and file=" << m.start_file-1 << endl;
	// for (forward_list<CheckersPiece>::iterator it = (color==1)?white_plist.begin():black_plist.begin(); it != ((color==1)?white_plist.end():black_plist.end()); ++it) {

	for (CheckersPiece &cp : (color==1)?white_plist:black_plist) {
		// CheckersPiece temp = (*it);

		if (cp.row==m.start_row && cp.file==m.start_file) {
			

			// cout << "updating piece with move " << m.toString() << endl;
			cp.file=m.dest_files[m.num_moves-1];
			cp.row=m.dest_rows[m.num_moves-1];
			// cout << "updating board" << endl;

			if (boardpiece==white && cp.row == 8) {
				boardpiece=whitek;
				value += 10;
			} else if (boardpiece==red && cp.row == 1) {
				boardpiece=redk;
				value -= 10;
			}

			board[cp.row][cp.file] = boardpiece; // add it to the board
			cp.p = boardpiece;

			// cout << (*it)->toString() << endl;
			break;
		}
	}

	// for (CheckersPiece cp : (color==1)?white_plist:black_plist) {
	// 	cout << "debug " << cp.toString() << endl;
	// }


}




// TODO, do this in the JS instead?
// TODO, test keep jumping
bool Board::sendVerification(Move &mm) {

	bool jump_request; // the Move mm is a jump
	bool jump_available; // there is a move that is mm+additional jumps available. You must make that move.

	// set jump_request
	jump_request = mm.takes;
	jump_available = false;

	// for (forward_list<Move>::iterator it = possibleMoves.begin(); it != possibleMoves.end(); ++it ) {
	for (Move m : possibleMoves) {
		// cout << "checking move " << (*it)->toString() << endl;
		if (m.toString() == mm.toString()) { // valid move

			return true;
		} else if (jump_request) {
			// figure out if (*it) is a jump extending this one

			int mmlen = mm.toString().length();
			string sub = m.toString().substr(0,mmlen);

			if (mm.toString() == sub) { // TODO does this work?
				// jump_available = true;
				cout << "keep jumping" << endl;
				return false;
			}
		}
	}


	cout << "invalid move" << endl;
	return false;
}

void Board::printBoard(){
    cout<<"Printing"<<endl;
    for (int i = ISIZE-1; i >=0; i--) {
		for (int j = 0; j < ISIZE; j++){

		    if (board[i][j] == red) { // red
				cout << "\e[1m" << "p" << "\e[0m";
			} else if (board[i][j] == white) {
				cout << "p";
			} else if (board[i][j] == redk) {
				cout << "\e[1m" << "k" << "\e[0m";
			} else if (board[i][j] == whitek) {
				cout << "k";
			}

			else { // white
				cout << " ";
			} 
		}
		cout<<endl;
    }
    cout << "finished printing the board" << endl;
}

											// the move so far, = have we constructed a move along this path?
bool Board::doubleJumpHelper(short row, short file, string sofar, bool canAdd, piece p) {
	bool ret1 = false;
	bool ret2 = false;

	// cout << "checking move for jumps so far: " << sofar << endl;
	switch(p) {
		case white:
			if ((board[row+1][file-1] == red || board[row+1][file-1] == redk) && board[row+2][file-2] == empty) { // jump up left
				stringstream ss1;
				ss1 << sofar << "xx" << (file-2)-1 << (row+2)-1;
				ret1 = doubleJumpHelper(row+2,file-2,ss1.str(),true,p);
			}
			if ((board[row+1][file+1] == red || board[row+1][file+1] == redk) && board[row+2][file+2] == empty) { // jump up right
				stringstream ss2;
				ss2 << sofar << "xx" << (file+2)-1 << (row+2)-1;
				ret2 = doubleJumpHelper(row+2,file+2,ss2.str(),true,p);
			}
			if (canAdd && !ret1 && !ret2) { // if you can't add anymore compulsory jumps, and the last move was ok to add, add this move.
				Move mm = Move(sofar);
				possibleMoves.push_front(mm);
				return true;
			}
			break;
		case red:
			if ((board[row-1][file-1] == white || board[row-1][file-1] == whitek) && board[row-2][file-2] == empty) { // jump down left
				stringstream ss1;
				ss1 << sofar << "xx" << (file-2)-1 << (row-2)-1;

				ret1 = doubleJumpHelper(row-2,file-2,ss1.str(),true,p);
			}
			if ((board[row-1][file+1] == white || board[row-1][file+1] == whitek) && board[row-2][file+2] == empty) { // jump down right
				stringstream ss2;
				ss2 << sofar << "xx" << (file+2)-1 << (row-2)-1;
				ret2 = doubleJumpHelper(row-2,file+2,ss2.str(),true,p);
			}
			if (canAdd && !ret1 && !ret2) { // if you can't add anymore compulsory jumps, and the last move was ok to add, add this move.
				Move mm = Move(sofar);
				possibleMoves.push_front(mm);
				return true;
			}
			break;

		default:
			break;

	}

	return canAdd || ret1 || ret2; // did we make a jump?
}

bool Board::doubleJumpHelperKing(short row, short file, string sofar, bool canAdd, piece p, char temp_board[10][10]) {

	// cout << "double jump problem?" << endl;
	bool ret1 = false;
	bool ret2 = false;
	bool ret3 = false;
	bool ret4 = false;
	char temp_board2[10][10];
	copy(&temp_board[0][0],&temp_board[0][0]+100,&temp_board2[0][0]);

	switch(p) {
		
		case whitek:
			// cout << "looking for bug" << endl;
			if ((temp_board[row+1][file-1] == red || temp_board[row+1][file-1] == redk) && temp_board[row+2][file-2] == empty) { // jump up left
				stringstream ss1;
				ss1 << sofar << "xx" << (file-2)-1 << (row+2)-1;
				// cout << ss1.str() << endl;

				temp_board2[row+1][file-1] = empty;
				ret1 = doubleJumpHelperKing(row+2,file-2,ss1.str(),true,p,temp_board2);
				temp_board2[row+1][file-1] = temp_board[row+1][file-1];
			}
			if ((temp_board[row+1][file+1] == red || temp_board[row+1][file+1] == redk) && temp_board[row+2][file+2] == empty) { // jump up right
				stringstream ss2;
				ss2 << sofar << "xx" << (file+2)-1 << (row+2)-1;
				// cout << ss2.str() << endl;
				temp_board2[row+1][file+1] = empty;
				ret2 = doubleJumpHelperKing(row+2,file+2,ss2.str(),true,p,temp_board2);
				temp_board2[row+1][file+1] = temp_board[row+1][file+1];
			}

			if ((temp_board[row-1][file-1] == red || temp_board[row-1][file-1] == redk) && temp_board[row-2][file-2] == empty) { // jump down left
				stringstream ss3;
				ss3 << sofar << "xx" << (file-2)-1 << (row-2)-1;
				// cout << ss3.str() << endl;
				temp_board2[row-1][file-1] = empty;
				ret3 = doubleJumpHelperKing(row-2,file-2,ss3.str(),true,p,temp_board2);
				temp_board2[row-1][file-1] = temp_board[row-1][file-1];
			}
			if ((temp_board[row-1][file+1] == red || temp_board[row-1][file+1] == redk) && temp_board[row-2][file+2] == empty) { // jump down right
				stringstream ss4;
				ss4 << sofar << "xx" << (file+2)-1 << (row-2)-1;
				// cout << ss4.str() << endl;
				temp_board2[row-1][file+1] = empty;
				ret4 = doubleJumpHelperKing(row-2,file+2,ss4.str(),true,p,temp_board2);
				temp_board2[row-1][file+1] = temp_board[row-1][file+1];
			}
			// cout << "looking for bug2" << endl;

			if (canAdd && !ret1 && !ret2 && !ret3 && !ret4) { // if you can't add anymore compulsory jumps, and the last move was ok to add, add this move.
				Move mm = Move(sofar);
				possibleMoves.push_front(mm);
				return true;
			}
			break;

		case redk:
			// cout << "double jump problem1" << endl;
			if ((temp_board[row+1][file-1] == white || temp_board[row+1][file-1] == whitek) && temp_board[row+2][file-2] == empty) { // jump up left
				stringstream ss1;
				ss1 << sofar << "xx" << (file-2)-1 << (row+2)-1;
				temp_board2[row+1][file-1] = empty;
				ret1 = doubleJumpHelperKing(row+2,file-2,ss1.str(),true,p,temp_board2);
				temp_board2[row+1][file-1] = temp_board[row+1][file-1];
			}
			// cout << "double jump problem2" << endl;
			if ((temp_board[row+1][file+1] == white || temp_board[row+1][file+1] == whitek) && temp_board[row+2][file+2] == empty) { // jump up right
				stringstream ss2;
				ss2 << sofar << "xx" << (file+2)-1 << (row+2)-1;
				temp_board2[row+1][file+1] = empty;
				ret2 = doubleJumpHelperKing(row+2,file+2,ss2.str(),true,p,temp_board2);
				temp_board2[row+1][file+1] = temp_board[row+1][file+1];
			}
			// cout << "double jump problem3" << endl;
			if ((temp_board[row-1][file-1] == white || temp_board[row-1][file-1] == whitek) && temp_board[row-2][file-2] == empty) { // jump down left
				stringstream ss3;
				ss3 << sofar << "xx" << (file-2)-1 << (row-2)-1;
				temp_board2[row-1][file-1] = empty;
				ret3 = doubleJumpHelperKing(row-2,file-2,ss3.str(),true,p,temp_board2);
				temp_board2[row-1][file-1] = temp_board[row-1][file-1];
			}
			// cout << "double jump problem4" << endl;
			if ((temp_board[row-1][file+1] == white || temp_board[row-1][file+1] == whitek) && temp_board[row-2][file+2] == empty) { // jump down right
				stringstream ss4;
				ss4 << sofar << "xx" << (file+2)-1 << (row-2)-1;
				temp_board2[row-1][file+1] = empty;
				ret4 = doubleJumpHelperKing(row-2,file+2,ss4.str(),true,p,temp_board2);
				temp_board2[row-1][file+1] = temp_board[row-1][file+1];
			}
			// cout << "double jump problem5" << endl;
			if (canAdd && !ret1 && !ret2 && !ret3 && !ret4) { // if you can't add anymore compulsory jumps, and the last move was ok to add, add this move.
				Move mm = Move(sofar);
				possibleMoves.push_front(mm);
				return true;
			}
			break;

		default:
			break;
	}

	return canAdd || ret1 || ret2 || ret3 || ret4;

}




// TODO double jumps for all, king pieces, error
void Board::fillPossibleMoves() { // fill this state's possible move list with all it's possible moves, given the color

	// iterate over the pieces of the color to move


	bool jump_move = false;

	forward_list<Move> nonjumps;
	forward_list<CheckersPiece> plist = (colorToMove==1)?white_plist:black_plist;
	// for (forward_list<CheckersPiece>::iterator it = plist.begin(); it != plist.end(); ++it ) {
	for (CheckersPiece cp : plist) {

		// cout << "looking at " << (*it)->toString() << endl;
		// CheckersPiece temp = cp;
		int row=cp.row;
		int file=cp.file;
		piece p=cp.p;
		int color = cp.color;

		piece dest; // value of potential destination square
		bool takes;

		bool piece_jump = false;
		
		stringstream ss; // for jumps
		switch(p) {
			case white: // white pawn

				ss << file-1 << row-1; // the string version of move takes the visual coordinates
				piece_jump = doubleJumpHelper(row,file,ss.str(),false,p);

				jump_move = jump_move || piece_jump;

				// get the non-jump moves. remember, jumps are compulsory
				if (!jump_move) { // if can jump, have to jump
					if (board[row+1][file-1] == empty) { // move up left
						nonjumps.push_front(Move(file,row,file-1,row+1,false));
					}
					if (board[row+1][file+1] == empty) { // move up right
						nonjumps.push_front(Move(file,row,file+1,row+1,false));
					}
				}
				break;

			case red: // red pawn

				ss << file-1 << row-1; // the string version of move takes the visual coordinates
				piece_jump = doubleJumpHelper(row,file,ss.str(),false,p);

				jump_move = jump_move || piece_jump;

				if (!jump_move) {
					if (board[row-1][file-1] == empty) { // move down left
						nonjumps.push_front(Move(file,row,file-1,row-1,false));
					}
					if (board[row-1][file+1] == empty) { // move down right
						nonjumps.push_front(Move(file,row,file+1,row-1,false));
					}
				}
				break;

			case whitek: // white king
			case redk: // red king

				// cout << "calling jump helper" << endl;

				// calculate the piece jumps
				ss << file-1 << row-1; // the string version of move takes the visual coordinates
				piece_jump = doubleJumpHelperKing(row,file,ss.str(),false,p,board);

				jump_move = jump_move || piece_jump;

				// cout << "got past jump helper" << endl;


				if (!jump_move) {
					if (board[row+1][file-1] == empty) { // move up left
						nonjumps.push_front(Move(file,row,file-1,row+1,false));
					}
					if (board[row+1][file+1] == empty) { // move up right
						nonjumps.push_front(Move(file,row,file+1,row+1,false));
					}
					if (board[row-1][file-1] == empty) { // move down left
						nonjumps.push_front(Move(file,row,file-1,row-1,false));
					}
					if (board[row-1][file+1] == empty) { // move down right
						nonjumps.push_front(Move(file,row,file+1,row-1,false));
					}
				}

				break;

			default:
				break; // error?

		}
	}


	// no jumps? ok then can add the other moves
	if (!jump_move) {
		possibleMoves = nonjumps;
		// printList(possibleMoves);
		// printList(nonjumps);
		// printf("*****\n");
	}


	// printList(possibleMoves);
	

	if (possibleMoves.empty()) {
		// cout << "setting gameover=true" << endl;
		gameover = true;
		//cout << "gameover" << endl;
		// THIS PLAYER LOSES!
	}


	// printPieceList(possibleMoves);
}


double ratioWeight(int material)
{
    const int maxw = 15;
    int x = material-80;
    if (material < 80)
	return maxw;
    double weight = 0;
    if (x < 0)
	x = -x;
    x = x/10;
    weight = maxw/(x+1);
    return weight;
}


// calculate the value of the board
double Board::staticEvaluation() {
	// give pieces their own values depending on how far pawns are, or if they are kings
	// sum the values

	// if (gameover==true) {
	// 	if (colorToMove==1) {
	// 		return -100000;
	// 	} else {
	// 		return 100000;
	// 	}
	// }

	// double value = 0;

	double white_total = 0;
	double red_total = 0;

	for (CheckersPiece cp : white_plist) {
		white_total += cp.getValue();
	}
	if (white_total==0) {
		return -100000;
	}
	for (CheckersPiece cp : black_plist) {
		red_total += cp.getValue();
	}
	if (red_total==0) {
		return 100000;
	}

	// value = white_total + red_total;

	// double ratio = white_total/(-red_total);
	// double inv_neg_ratio = red_total/white_total;

	if (white_total+red_total > 0) { // white is winning
		return white_total / -red_total;
	} else if (white_total+red_total < 0) { // red is winning
		return red_total / white_total;
	} else {
		return 0;
	}

	// if (ratio > 1) {
	//     value += ratio * ratioWeight(white_total - red_total);
	// } else {
	//     value += inv_neg_ratio * ratioWeight(white_total - red_total);
	// }
	// return value;
}

// return a random long
long lrand() {
    return (static_cast<long>(rand()) << (sizeof(int) * 8)) | rand();
}

void Board::generateBitstrings() {

	for (int i=0; i<ISIZE; i++) {
		for (int j=0; j<ISIZE; j++) {
			for (int k = 1; k < 5; k++) {
				bitstringtable[i][j][k] = lrand();
			}
		}
	}

}

// hash this board from scratch
long Board::Zhash() {

	long h = 0;
	for (int i=0; i<ISIZE; i++) {
		for (int j=0; j<ISIZE; j++) {
			if (board[i][j] != empty && board[i][j] != padded) {
				piece p = (piece)board[i][j];
				h = h ^ bitstringtable[i][j][p];
			}
		}
	}

	return h;
}

// before i update the actual board.
long Board::updateZhash(Move &m) {

	int srow = m.start_row;
	int sfile = m.start_file;
	int drow,dfile;
	int rrow,rfile; // coords of piece to remove
	long h = hash;
	piece moving = (piece) board[srow][sfile];

	// xor out the piece from its position
	h = h ^ bitstringtable[srow][sfile][moving];

	if (m.takes==true) {

		// find every piece that was taken:
		for (int i=0; i < m.num_moves; i++) {
			// calculate the coordinates of the piece taken
			drow = m.dest_rows[i];
			dfile = m.dest_files[i];

			rrow = (srow+drow)/2;
			rfile = (sfile+dfile)/2;

			piece rem = (piece)board[rrow][rfile];

			// xor out the taken piece
			h = h ^ bitstringtable[drow][dfile][rem];

			srow = drow;
			sfile = dfile;

		}

	} else {
		drow = m.dest_rows[0];
		dfile = m.dest_files[0];
	}

	if (moving == white && drow == 8) {
		moving = whitek;
	} else if (moving == red && drow == 1) {
		moving = redk;
	}

	// xor the piece into the dest position
	h = h ^ bitstringtable[drow][dfile][moving];

	return h;
}
















