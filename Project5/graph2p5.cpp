// Project 5 Graph Coloring 2: Solving graph coloring 
// using local search and 
// Created by Patrick Hanbury and Cassandra Smith
// Submitted 6/25/2018
//
// Code to read graph instances from a file.  Uses the Boost Graph Library (BGL).

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <fstream>
#include <time.h>

#include <boost/graph/adjacency_list.hpp>

#define LargeValue 99999999

using namespace std;
using namespace boost;

int const NONE = -1;  // Used to represent a node that does not exist

struct VertexProperties;
struct EdgeProperties;

typedef adjacency_list<vecS, vecS, undirectedS, VertexProperties, EdgeProperties> Graph;

void printSolution(Graph &g, int numConflicts, string filePath_output);
int getConflicts(Graph &g);
void randGraph(Graph &g, int numColors, int &numConflicts);
void greedyColoring(Graph &g, int numColors, int &numConflicts);
void placeColor(Graph &g, Graph::vertex_iterator node, int &numConflicts, const int numColors);
void simAnnealing(Graph &g, int numColors, int &numConflicts, int t);
Graph getRandNeighbor(Graph &g, int numColors);
bool chance(int th);
void steepDescent(Graph &g, int numColors, int &numConflicts, clock_t startT, int t);
void getBestNeighbor(Graph &g, int numColors, int &numConflicts);

struct VertexProperties
{
	pair<int, int> cell; // maze cell (x,y) value
	Graph::vertex_descriptor pred;
	bool visited;
	bool marked;
	int weight;
	int color;
};

// Create a struct to hold properties for each edge
struct EdgeProperties
{
	int weight;
	bool visited;
	bool marked;
};

void initializeGraph(Graph &g, ifstream &fin)
// Initialize g using data from fin.
{
	int n, e;
	int j, k;

	fin >> n >> e;
	Graph::vertex_descriptor v;

	// Add nodes.
	for (int i = 0; i < n; i++)
		v = add_vertex(g);

	for (int i = 0; i < e; i++)
	{
		fin >> j >> k;
		add_edge(j, k, g);  // Assumes vertex list is type vecS
	}
}

void setNodeWeights(Graph &g, int w)
// Set all node weights to w.
{
	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);

	for (Graph::vertex_iterator vItr = vItrRange.first; vItr != vItrRange.second; ++vItr)
	{
		g[*vItr].weight = w;
	}
}

int main()
{
	char x;
	ifstream fin;
	string fileName;

	// Read the name of the graph from the keyboard or
	// hard code it here for testing.

	fileName = "color/color192-8.input";

	//   cout << "Enter filename" << endl;
	//   cin >> fileName;

	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}
	try
	{
		cout << "Reading graph" << endl;
		Graph g;
		int numColors;
		int numConflicts = 0;
		fin >> numColors;
		initializeGraph(g, fin);

		cout << "Num nodes: " << num_vertices(g) << endl;
		cout << "Num edges: " << num_edges(g) << endl;
		cout << endl;

		//greedyColoring(g, numColors, numConflicts);
		randGraph(g, numColors, numConflicts);
		cout << "NumConflicts: " << numConflicts << endl;
		simAnnealing(g, numColors, numConflicts, 300);
		
		string output = "/color_SA_output/color192-8.output";
		printSolution(g, numConflicts, output);
		cout << "Best number of conflicts is " << numConflicts << endl;
		system("pause");

	}
	catch (indexRangeError &ex)
	{
		cout << ex.what() << endl; exit(1);
	}
	catch (rangeError &ex)
	{
		cout << ex.what() << endl; exit(1);
	}
}

//Prints the best graph coloring calculated
void printSolution(Graph &g, int numConflicts, string filePath_output) {
	ofstream myfile;
	myfile.open(filePath_output.c_str());

	myfile << "Total Conflicts: " << numConflicts << endl;

	for (int counter = 0; counter < num_vertices(g); counter++) {
		myfile << counter << ": " << g[counter].color << endl;
	}

	myfile.close();
}

int getConflicts(Graph &g) {
	int numConflicts = 0;

	pair<Graph::edge_iterator, Graph::edge_iterator> eItrRange = edges(g);

	for (Graph::edge_iterator eItr = eItrRange.first; eItr != eItrRange.second; ++eItr) {
		Graph::vertex_descriptor v1 = target(*eItr, g);
		Graph::vertex_descriptor v2 = source(*eItr, g);

		if (g[v1].color == g[v2].color) {
			numConflicts++;
		}
	}
	return numConflicts;
}

void randGraph(Graph &g, int numColors, int &numConflicts) {
	srand(time(0));
	//gets start and end vertex iterators (which allow you to access the vertex)
	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);

	Graph::vertex_iterator firstNode = vItrRange.first;
	Graph::vertex_iterator lastNode = vItrRange.second;

	//assign all vertcies to random colors
	for (Graph::vertex_iterator vItr = firstNode; vItr != lastNode; ++vItr) {
		g[*vItr].color = rand() % numColors;
	}
	numConflicts = getConflicts(g);
}

void greedyColoring(Graph &g, int numColors, int &numConflicts) {
	//gets start and end vertex iterators
	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);

	Graph::vertex_iterator firstNode = vItrRange.first;
	Graph::vertex_iterator lastNode = vItrRange.second;

	//assign all vertex colors to 0
	for (Graph::vertex_iterator vItr = firstNode; vItr != lastNode; ++vItr) {
		g[*vItr].color = 0;
	}

	//assign vertex colors using the greedy algorithm
	for (Graph::vertex_iterator vItr = firstNode; vItr != lastNode; ++vItr) {
		placeColor(g, vItr, numConflicts, numColors);
	}
}


void placeColor(Graph &g, Graph::vertex_iterator node, int &numConflicts, const int numColors) {
	int color, minConflicts = numColors;
	vector<int> conflicts;
	conflicts.resize(numColors);

	// Get a pair containing iterators
	pair<Graph::adjacency_iterator, Graph::adjacency_iterator> adjRange = adjacent_vertices(*node, g);

	// Loop over adjacent nodes in the graph
	for (Graph::adjacency_iterator Itr = adjRange.first; Itr != adjRange.second; ++Itr) {
		conflicts[g[*Itr].color] += 1;
	}

	//assign color corresponding to minimum number of conflicts
	for (int i = 0; i < numColors; i++) {
		if (conflicts[i] < minConflicts) {
			minConflicts = conflicts[i];
			color = i;
		}
	}

	//place color
	g[*node].color = color;

	//check if this node contains the max conflicts
	if (minConflicts > numConflicts) {
		numConflicts++;
	}
}

void simAnnealing(Graph &g, int numColors, int &numConflicts, int t) {
	clock_t startT = clock();
	Graph bestSol = Graph(g);
	int bestConflicts = numConflicts;
	//threshold value
	float th = 1;
	float alpha = 0.9;

	//number of times to increment
	int totalIncrements = 20;
	//increment num
	int thIncrement = 0;

	while (thIncrement <= totalIncrements) {
		//Exit function if time is exceeded
		if (((float)(clock() - startT) / CLOCKS_PER_SEC) >= t) {
			break;
		}
		Graph temp = getRandNeighbor(g, numColors);
		int tempConflicts = getConflicts(temp);
		//cout << "Random graph, tempConflicts: " << tempConflicts << endl;
		steepDescent(temp, numColors, tempConflicts, startT, t);
		//cout << "After steepest descent, tempConflicts: " << tempConflicts << endl;
		if (tempConflicts < bestConflicts) {
			bestSol = temp;
			bestConflicts = tempConflicts;
			g = temp;
			numConflicts = tempConflicts;
		}
		else if (tempConflicts == numConflicts) {
			g = temp;
			//numConflicts = tempConflicts;
		}
		else if (chance(th)) {
			cout << "Chose worse graph" << endl;
			system("pause");
			g = temp;
			numConflicts = tempConflicts;
		}
		//Exit function if time is exceeded
		if (((float)(clock() - startT) / CLOCKS_PER_SEC) >= t) {
			break;
		}
		th *= alpha;
		thIncrement++;
	}
	g = bestSol;
	numConflicts = bestConflicts;
}

//Chooses a random node and changes it to a random color
Graph getRandNeighbor(Graph &g, int numColors) {
	Graph temp = Graph(g);
	srand(time(0));

	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(temp);

	Graph::vertex_iterator firstNode = vItrRange.first;
	Graph::vertex_iterator lastNode = vItrRange.second;

	int randNode = rand() % num_vertices(temp);
	Graph::vertex_iterator vItr = firstNode;
	for (int i = 0; i < randNode; i++) {
		++vItr;
	}

	temp[*vItr].color = rand() % numColors;

	return temp;
}

bool chance(int th) {
	srand(time(0));
	if ((rand() % 1000) <= (th * 1000)) {
		return true;
	}
	else {
		return false;
	}
}

void steepDescent(Graph &g, int numColors, int &numConflicts, clock_t startT, int t) {
	Graph bestSol = Graph(g);
	int bestConflicts = numConflicts;
	bool done = false;

	while (!done) {
		if (((float)(clock() - startT) / CLOCKS_PER_SEC) >= t) {
			break;
		}
		getBestNeighbor(g, numColors, numConflicts);

		if (bestConflicts == numConflicts) {
			done = true;
		}
		else {
			bestSol = g;
			bestConflicts = numConflicts;
		}
	}
}

//Switches the colors of every pair of adjacent nodes and returns the 
//graph with the least number of conflicts
void getBestNeighbor(Graph &g, int numColors, int &numConflicts) {
	Graph bestN = Graph(g);
	int bestConflicts = numConflicts, temp;

	//gets start and end vertex iterators (which allow you to access the vertex)
	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
	Graph::vertex_iterator firstNode = vItrRange.first;
	Graph::vertex_iterator lastNode = vItrRange.second;
	
	/*for (Graph::vertex_iterator vItr = firstNode; vItr != lastNode; ++vItr) {
		temp = g[*vItr].color;
		for (int i = 1; i < numColors; i++) {
			g[*vItr].color = (temp + i) % numColors;
			if (getConflicts(g) < bestConflicts) {
				bestN = Graph(g);
				bestConflicts = getConflicts(g);
			}
		}
		g[*vItr].color = temp;
	}*/


	for (Graph::vertex_iterator vItr = firstNode; vItr != lastNode; ++vItr) {
		// Get iterators pointing to the beginning and end of the list of nodes adjacent to node v
		pair<Graph::adjacency_iterator, Graph::adjacency_iterator> adjRange = adjacent_vertices(*vItr, g);
		// Loop over adjacent nodes
		for (Graph::adjacency_iterator Itr = adjRange.first; Itr != adjRange.second; ++Itr) {
			temp = g[*vItr].color;
			g[*vItr].color = g[*Itr].color;
			g[*Itr].color = temp;

			if (getConflicts(g) < bestConflicts) {
				bestN = Graph(g);
				bestConflicts = getConflicts(bestN);
			}
			g[*Itr].color = g[*vItr].color;
			g[*vItr].color = temp;
		}
	}
	g = Graph(bestN);
	numConflicts = bestConflicts;
}