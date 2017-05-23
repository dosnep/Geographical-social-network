#include "graph.h"

Graph::Graph(int nbNodes, int nbUsers, string graphFile,string roadsFile){
	this->datas = new MetasDatas;
	this->datas->nbNodes = nbNodes;
	this->datas->nbUsers = nbUsers;
	this->datas->poisVisited = new vector<uint>[nbUsers];
	this->datas->tabNodes = new Node*[nbNodes];
	initGraphFromFile(graphFile);
	recordPoisVisitedFromFile(roadsFile);

	for(int i = 0;i<this->datas->nbUsers;i++)
	{
		sort(this->datas->poisVisited[i].begin(),this->datas->poisVisited[i].end());
		this->datas->poisVisited[i].erase(unique(this->datas->poisVisited[i].begin(),this->datas->poisVisited[i].end()),this->datas->poisVisited[i].end());
	}

}

Graph::~Graph(){

	for(short int i = 0;i<this->datas->poisVisited->size();i++)
	{
		this->datas->poisVisited[i].clear();
	}

	delete [] this->datas->poisVisited;

	this->datas->poi_id.clear();
	this->datas->reverse_poi_id.clear();
	this->datas->user_id.clear();
	this->datas->reverse_user_id.clear();

	for(short int i = 0;i<this->datas->nbUsers;i++)
	{
		deleteEdgeList(this->datas->tabNodes[i]->edgeOutList);
		deleteEdgeList(this->datas->tabNodes[i]->edgeInList);

	}

	delete [] this->seuils;
	delete [] this->datas->tabNodes;
	delete this->datas;

}

void Graph::exploreEdgeList(EdgeList *list)
{
	Edge *tmp = list->firstElement;
	while(tmp)
	{
		printf("id : %d---%f---%d\n",this->datas->poi_id[tmp->firstNode],tmp->score,this->datas->poi_id[tmp->secondNode]);
		tmp = tmp->nextEdge;
	}

	delete tmp;
}

void Graph::exploreAllGraph()
{

	for(int i = 0; i<this->datas->nbNodes;i++)
		exploreEdgeList(this->datas->tabNodes[i]->edgeOutList);


}

void Graph::exploreAllRoads()
{

	for(int i = 0; i<this->datas->nbUsers;i++)
	{
		printf("----User : %d----\n",this->datas->user_id[i]);

		for(int j = 0;j<this->datas->poisVisited[i].size();j++)
		{
			printf("poi : %d\n",this->datas->poi_id[this->datas->poisVisited[i][j]]);
		}

	}
}

bool sortScores (pair<uint,double> i,pair<uint,double> j)
{
	return ((i.second > j.second));
}

void Graph::deleteEdgeList(EdgeList *list)
{
	Edge *current = new Edge;
	Edge *next = new Edge;
	current = list->firstElement;
	next = current->nextEdge;

	while(next)
	{
		delete current;
		current = next;
		next = current->nextEdge;
	}

	delete current;
	delete next;

}



void Graph::addEdge(EdgeList *list,int firstID,int secondID,double score)
{
	Edge *newEdge = new Edge;
	newEdge->firstNode = firstID;
	newEdge->secondNode = secondID;
	newEdge->nextEdge = list->firstElement;
	newEdge->score = score;
	list->firstElement = newEdge;

}



void Graph::initGraphFromFile(string path)
{

	FILE *fileptr = fopen(path.c_str(),"r");
	int poi1, poi2,nbUsers;
	double score = 0;
	int nbPoi = 0;
	int nbEdge = 0;

	while ((fscanf(fileptr, "%d\t%d\t%lf\t%d", &poi1, &poi2, &score, &nbUsers) != EOF)) {
		if(this->datas->reverse_poi_id.count(poi1)==0)
		{
			if(nbPoi > this->datas->nbNodes)
				cout << nbPoi << endl;

			this->datas->poi_id[nbPoi] = poi1;
			this->datas->reverse_poi_id[poi1] = nbPoi;
			nbPoi++;

			//Create new node
			Node *first = new Node;
			first->id = this->datas->reverse_poi_id[poi1];
			first->edgeOutList = new EdgeList;
			first->edgeInList = new EdgeList;
			first->degreI = 0;
			first->degreO = 0;
			//Add node to the graph
			this->datas->tabNodes[this->datas->reverse_poi_id[poi1]] = first;
		}

		//Same thing for the second node
		if(this->datas->reverse_poi_id.count(poi2)==0)
		{

			this->datas->poi_id[nbPoi] = poi2;
			this->datas->reverse_poi_id[poi2] = nbPoi;
			nbPoi++;

			Node *second = new Node;
			second->id = this->datas->reverse_poi_id[poi2];
			second->edgeOutList = new EdgeList;
			second->edgeInList = new EdgeList;
			second->degreI = 0;
			second->degreO = 0;
			this->datas->tabNodes[this->datas->reverse_poi_id[poi2]] = second;
		}


		//Add neighbors

		this->addEdge(this->datas->tabNodes[this->datas->reverse_poi_id[poi1]]->edgeOutList,this->datas->reverse_poi_id[poi1],this->datas->reverse_poi_id[poi2],score);
		this->datas->tabNodes[this->datas->reverse_poi_id[poi1]]->degreO++;
		this->addEdge(this->datas->tabNodes[this->datas->reverse_poi_id[poi2]]->edgeInList,this->datas->reverse_poi_id[poi2],this->datas->reverse_poi_id[poi1],score);
		this->datas->tabNodes[this->datas->reverse_poi_id[poi2]]->degreI++;
	}


}

void Graph::recordPoisVisitedFromFile(string path)
{
	FILE *fileptr = fopen(path.c_str(),"r");
	int user, poi1, poi2,nbUser;
	nbUser = 0;
	while ((fscanf(fileptr, "%d\t%d\t%d", &user, &poi1,&poi2) != EOF)) {


		if(this->datas->reverse_user_id.count(user)==0)
		{
			this->datas->user_id[nbUser] = user;
			this->datas->reverse_user_id[user] = nbUser;
			nbUser++;
		}

		this->datas->poisVisited[this->datas->reverse_user_id[user]].push_back(this->datas->reverse_poi_id[poi1]);
		this->datas->poisVisited[this->datas->reverse_user_id[user]].push_back(this->datas->reverse_poi_id[poi2]);

	}
}



void Graph::initWeightNode(int poiID,double *weight, bool *isVisited, bool extend)
{

	weight[poiID] = 1;
	isVisited[poiID] = true;

	if(extend)
	{
		Edge *tmp = this->datas->tabNodes[poiID]->edgeOutList->firstElement;

		while(tmp)
		{
			if(!isVisited[tmp->secondNode])
			{
				weight[tmp->secondNode] +=tmp->score;
			}
			tmp = tmp->nextEdge;
		}

		delete tmp;
	}

}


void Graph::computeWeightedJacc_alt(string path, int thread,int firstId, int secondId,bool left_extend, bool right_extend)
{

	double *left_score = new double[this->datas->nbNodes];
	bool *left_visited = new bool[this->datas->nbNodes];
	vector <pair<uint,double>> *scores = new vector <pair<uint,double>>[this->datas->nbUsers];
	vector <pair<uint,double>> *tmpScores = new vector <pair<uint,double>>[thread];

	int avgDegresToApply[] = {30,40,50,60,70,80,90}; //N2
	int avgDegreFromGraph = 0;	//N1

#pragma omp parallel for num_threads(thread) shared(left_score,left_visited)
	for(int j = 0;j<this->datas->nbNodes;j++)
	{
		left_score[j] = 0.0;
		left_visited[j] = false;
	}

	time_t start_time, end_time;
	time(&start_time);

	//For each users u1
	for(int i = firstId;i<secondId;i++)
	{

		//For each trips from u1
		for(int roads = 0;roads<this->datas->poisVisited[i].size();roads++)
			//Init pois's weigths from u1's trips
			initWeightNode(this->datas->poisVisited[i][roads],left_score,left_visited,left_extend);

#pragma omp parallel for num_threads(thread) schedule(dynamic) shared(left_score,left_visited,scores)
		//For each users u2
		for(int j = 0;j<this->datas->nbUsers;j++)
		{
			double *right_score = new double[this->datas->nbNodes];
			bool *right_visited = new bool[this->datas->nbNodes];

			for(int k = 0;k<this->datas->nbNodes;k++)
			{
				right_score[k] = 0;
				right_visited[k] = false;
			}

			float uni, inter;
			uni = inter = 0.0;

			//For each trips from u2
			for(int roads = 0;roads<this->datas->poisVisited[j].size();roads++)
			{	//Init pois's weigths from u2's trips
				initWeightNode(this->datas->poisVisited[j][roads],right_score,right_visited,right_extend);
			}


			//Compare u1 and u2
			for(int k = 0;k<this->datas->nbNodes;k++)
			{
				//Add max
				uni += left_score[k] > right_score[k] ? left_score[k] :right_score[k];
				//Add min
				inter += left_score[k] > right_score[k]? right_score[k] :left_score[k];
				right_score[k] = 0.0;
				right_visited[k] = false;
			}


			delete [] right_visited;
			delete [] right_score;

			//Compute and add Jaccard score bewteen u1 and u2
			if(i!=j && inter !=0)
			{
				tmpScores[omp_get_thread_num()].push_back(make_pair(j,inter/uni));
			}
		}

		for(int j = 0;j<thread;j++)
		{

			scores[i].insert(scores[i].end(),tmpScores[j].begin(),tmpScores[j].end());
			tmpScores[j].clear();

		}


		//sort u1's scores DESC
		sort(scores[i].begin(),scores[i].end(),sortScores);
		//Add all degres from the graph
		avgDegreFromGraph += scores[i].size();

		if(i!=0 && i%100 == 0)
		{
			time(&end_time);
			cout << "Time : " << difftime(end_time, start_time) << endl;
			time(&start_time);
		}

#pragma omp parallel for num_threads(thread)
		for(int k = 0;k<this->datas->nbNodes;k++)
		{
			left_score[k] = 0.0;
			left_visited[k] = false;
		}

		cout << i << endl;
	}


	//Compute avg degre from graph
	avgDegreFromGraph /= this->datas->nbUsers;

	for(int i = 0;i<sizeof(avgDegresToApply)/sizeof(int);i++)
	{
		stringstream file_path;
		file_path << path;
		file_path<< "/score";
		file_path << firstId <<"_to_" << secondId << "_avg_" << avgDegresToApply[i];
		ofstream myfile (file_path.str());

		//New file for each new avg size
		if (myfile.is_open())
		{
			//Compute Filter = N2/N1
			double filter = (double)avgDegresToApply[i]/(double)avgDegreFromGraph;
			if(filter <=1)
			{
				for(int j = 0;j<this->datas->nbUsers;j++)
				{
					//Reshape neighboors of user i. Keep the F*degre(i) best scores
					vector<pair<uint,double>>scoresToKeep(scores[j].begin(),scores[j].begin()+(int)((filter)*scores[j].size()));
					vector<pair<uint,double>> ::iterator it;
					for(it = scoresToKeep.begin(); it != scoresToKeep.end(); it++)
					{

						if(it->second != 0)
							myfile << this->datas->user_id[j] << "\t" << this->datas->user_id[it->first]<<endl;
					}

				}
			}

			myfile.close();
		}
	}


	delete [] left_score;
	delete [] left_visited;
	for(int i = 0;i<this->datas->nbUsers;i++)
		scores[i].clear();
	delete [] scores;
	delete [] tmpScores;
}


