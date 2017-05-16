//============================================================================
// Name        : GSG.cpp
// Author      : Dosne
//============================================================================

#include <cstdlib>

#include "graph.h"

int main(int c, char* argv[]) {


	//<nbNodes> <nbUsers>  <path graph> <path usersRoads>
	graph *g = new graph(atoi(argv[1]),atoi(argv[2]),argv[3],argv[4]);

	//<destination_file><number of threads><user id to begin><user id to end>
	g->computeWeightedJacc_alt(argv[5],atoi(argv[6]),atoi(argv[7]),atoi(argv[8]),true,false);

	return 0;
}
