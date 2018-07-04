#include "move.hpp"


using namespace std;

Move::Move() {
    dest_files.resize(1);
    dest_rows.resize(1);

}


Move::Move(string str) {

	
		// 02->13 = column,row,column,row
		// 02xx24xx46

	// cout << "DEBUG " << str << endl;

	int len = str.length()-6; // 6,10,14 // -> 0,4,8
     dest_files.resize(1+(len/4));
      dest_rows.resize(1+(len/4));
	//dest_files = new int[1+(len/4)];
	//dest_rows = new int[1+(len/4)];

	// add the offset from the border (+1)
	start_file = 1+(str[0]-48);
	start_row = 1+(str[1]-48);
	
	bool jump = false;
	num_moves = 0;
	takes = (str[2]=='x')?true:false;

	do {
		num_moves++;

		// this might be wrong? out of bounds?
		jump = (((4*(num_moves+1) - 2) < str.length())  &&  (str[4*(num_moves+1) - 2]=='x'))?true:false;  // 2,6,10

		dest_files[num_moves-1] = 1+(str[num_moves*4]-48);  // 4,8
		dest_rows[num_moves-1] = 1+(str[num_moves*4 + 1]-48);

	} while (jump);


	// cout << "making move " << toString() << endl;
		
}


// used for simple moves: one move, or one take. for more than 1 move you must use another constructor.
Move::Move(int sf, int sr, int df, int dr, bool t) {
	start_file = sf;
	start_row = sr;
    dest_files.resize(1);
	dest_rows.resize(1);
	dest_files[0] = df;
	dest_rows[0] = dr;
	takes = t;
	num_moves = 1;

	// cout << "making move " << toString() << endl;
}


string Move::toString(void) {
	stringstream ss;
	ss << (start_file-1) << (start_row-1) << (takes? "xx":"->") << (dest_files[0]-1) << (dest_rows[0]-1);
	for (int i = 1; i < num_moves; ++i) {
		ss << "xx" << (dest_files[i]-1) << (dest_rows[i]-1);
	}
	return ss.str();
}






