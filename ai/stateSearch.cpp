#include "stateSearch.hpp"
#include <fstream>
#include <chrono>



unordered_map<long,Transposition> TTable[2];

int max_depth = 9;

int numPly;

short rnd = 0;

// wrapper for calculate move
Move makeMove(Board &b, short min_max) {
	if (b.possibleMoves.empty()) {
		cout << "no possible moves" << endl;
		exit(1);
	}

	numPly = 0;
	rnd++;
	if (b.numPieces() < 13)
		max_depth=10;
	if (b.numPieces() < 11)
		max_depth=11;
	if (b.numPieces() < 9)
		max_depth=12;
	if (b.numPieces() < 7)
		max_depth=13;
	if (b.numPieces() < 5)
		max_depth=14;
	if (b.numPieces() < 4)
		max_depth=15;
	cout << "DEBUG depth: " << max_depth << endl;

	TTable[0].erase(TTable[0].begin(), TTable[0].end());
	TTable[1].erase(TTable[1].begin(), TTable[1].end());

	//return b.possibleMoves.front();
	Move decision;
	//auto time_start = chrono::system_clock::now();
	decision = calculateMove(b, 0, min_max, !!(min_max+1));
	//auto time_end = chrono::system_clock::now();
	//auto time_elapsed = chrono::duration_cast<chrono::milliseconds>(time_end - time_start);
	//ofstream timefile;
	//timefile.open("logs/time.txt", std::ios::app);
	//timefile<<time_elapsed.count()<<endl;
	//timefile.close();

	//ofstream plyfile;
	//plyfile.open("logs/ply.txt", std::ios::app);
	//plyfile<<numPly<<endl;
	//plyfile.close();

	return decision;
}

// min_max: 1=maximizing, -1=minimizing
Move calculateMove(Board &pstate, short depth, short min_max, short to_move) {

	numPly++;

	double best = 100000*-min_max; // worst

	Move best_move;// = NULL;

	double alpha = -100000;
	double beta =   100000;

	double temp; // TODO put this in, instead of using it.value

	// evaluate the moves, TODO keep track of the max move score
	for (Move it : pstate.possibleMoves){
		// cout << "checking move " << (*it)->toString() << endl;
		Board b = Board(pstate,it);

#ifdef TTABLE
		if (TTable[to_move].find(b.hash) == TTable[to_move].end()) {
			it.value = getValue(b,depth+1, -min_max, !to_move, alpha, beta);
			TTable[to_move][b.hash] = {it.value,rnd};
		} else if ( (rnd - TTable[to_move][b.hash].rnd) > 0 ) { // we found this before this round
			it.value = getValue(b,depth+1, -min_max, !to_move, alpha, beta);
			TTable[to_move][b.hash] = {it.value,rnd};
		} else {
			it.value = TTable[to_move][b.hash].value;
		}
#else
		it.value = getValue(b,depth+1, -min_max, !to_move, alpha, beta);
#endif

		if (min_max==1) {

			if (best < it.value) {
				best = it.value;
				best_move = it;
			}
			if (best > alpha) {
				alpha = best;
			}

		} else if (min_max==-1) {

			if (best > it.value) {
				best = it.value;
				best_move = it;
			}
			if (best < beta) {
				beta = best;
			}

		}

		//it.value = getValue(bb,depth+1,-min_max, !to_move, alpha,beta); // pass -1 in for min_max bc want the best move for red
		
		// cout << "value:" << (*it)->value << endl;
		// if (it.value * min_max >= best * min_max) {
		// 	best = it.value;
		// 	best_move = it;
		// }
	}

	// ofstream valuefile;
	// valuefile.open("logs/value.txt", std::ios::app);
	// valuefile << best_move.value << endl;
	// valuefile.close();

	return best_move;
}


int getValue(Board &pstate, short depth, short min_max, short to_move, double a, double b) {

	numPly++;

	// reached depth, must statically evaluate
    if (depth==max_depth || pstate.gameover) {
		// evaluate pstate, return that value
		return pstate.staticEvaluation();
	}

	double alpha = a;
	double beta = b;

	// worst
	double best = 100000 * -min_max;
	double temp;

	for (Move it : pstate.possibleMoves) {
		Board b = Board(pstate, it);
		
#ifdef TTABLE
		if (TTable[to_move].find(b.hash) == TTable[to_move].end()) {
			temp = getValue(b,depth+1, -min_max, !to_move, alpha, beta);
			TTable[to_move][b.hash] = {temp,rnd};
		} else if ( (rnd - TTable[to_move][b.hash].rnd) > 0 ) {// we found this before this round
			// cout << "DEBUG found old hashed" << endl;
			temp = getValue(b,depth+1, -min_max, !to_move, alpha, beta);
			TTable[to_move][b.hash] = {temp,rnd};
		} else {
			// cout << "DEBUG found hashed" << endl;
			temp = TTable[to_move][b.hash].value;
		}
#else
		temp = getValue(b,depth+1, -min_max, !to_move, alpha, beta);
#endif
			
		

		if (min_max==1) {

			if (best < temp) {
				best = temp;
			}
			if (best > alpha) {
				alpha = best;
			}
			if (beta < alpha) { // <=? TODO
				break;	// beta cut off
			}

		} else if (min_max==-1) {

			if (best > temp) {
				best = temp;
			}
			if (best < beta) {
				beta = best;
			}
			if (beta < alpha) { // <=? TODO
				break; // alpha cut off
			}
		}

	}

	return best;
}











