#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include <fstream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

typedef struct node Node;
typedef struct nodeList NodeList;
typedef struct edge Edge;
typedef struct edgeList EdgeList;
typedef struct metas MetasDatas;

using namespace std;


struct edge{
	int firstNode;
	int secondNode;
	double score;
	Edge *nextEdge;
};


struct edgeList{
	Edge *firstElement;
};


struct node{

	EdgeList *edgeOutList;
	EdgeList *edgeInList;
	int id; //Node ID
	int degreO;
	int degreI;
};


struct metas{

	int nbUsers;
	int nbNodes; //Nb of POIs
	Node **tabNodes; // tab[Node ID] = Node
	vector<uint> *poisVisited; // tab[user ID] = POIs visited
	map<uint,uint>poi_id; //New_id ---> id from file
	map<uint,uint>reverse_poi_id; // id from file ---> New_id
	map<uint,uint>user_id; //New_id ---> id from file
	map<uint,uint>reverse_user_id; // id from file ---> New_id
};


class Graph{

public :
	MetasDatas *datas;
	double *seuils;

public :
	Graph(int nbNodes, int nbUsers, string graphFile,string roadsFile);
	~Graph();

	//INIT
	void initGraphFromFile(string path);
	void recordPoisVisitedFromFile(string path);

	//List Handle
	void addEdge(EdgeList *list,int firstID,int secondID,double score);
	void deleteEdgeList(EdgeList *list);
	int getEdgeID(int poi1, int poi2);

	//weigthed Jaccard
	void initWeightNode(int poiId,double *weight, bool *isVisited, bool extend);
	void computeWeightedJacc_alt(string path, int thread,int firstId, int secondId,bool left_extend, bool right_extend);

	//Print
	void exploreEdgeList(EdgeList *list);
	void exploreAllGraph();
	void exploreAllRoads();


};
