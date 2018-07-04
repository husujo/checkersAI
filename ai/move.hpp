#ifndef MOVE_H
#define MOVE_H


#include <sstream>
#include <string>
#include <iostream>
#include <vector>

using namespace std;



struct Move {

	double value; // will be calculated by the state search

	int start_file;
	int start_row;
    std::vector<int> dest_files;
    std::vector<int> dest_rows;
	int num_moves;
	bool takes;

	Move(string str);
	Move(int sf, int sr, int df, int dr, bool t);
	string toString(void);
	Move();
};

#endif
