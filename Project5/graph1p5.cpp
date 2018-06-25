// Project 5 Graph Coloring 1: Solving graph coloring 
// using local search
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
void randGraph(Graph &g, int numColors, int &numConflicts);
void steepDescent(Graph &g, int numColors, int &numConflicts);
void getBestNeighbor(Graph &g, int numColors, int &numConflicts);
int getConflicts(Graph &g);

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
		int numConflicts = -1;
		fin >> numColors;
		initializeGraph(g, fin);

		cout << "Num nodes: " << num_vertices(g) << endl;
		cout << "Num edges: " << num_edges(g) << endl;
		cout << endl;

		randGraph(g, numColors, numConflicts);
		cout << "NumConflicts: " << numConflicts << endl;
		steepDescent(g, numColors, numConflicts);
		string output = "/color_SD_output/color192-8.output";
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

void steepDescent(Graph &g, int numColors, int &numConflicts) {
	Graph bestSol = Graph(g);
	int bestConflicts = numConflicts;
	bool done = false;

	while (!done) {
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

void getBestNeighbor(Graph &g, int numColors, int &numConflicts) {
	Graph bestN = Graph(g);
	int temp;

	//gets start and end vertex iterators (which allow you to access the vertex)
	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
	Graph::vertex_iterator firstNode = vItrRange.first;
	Graph::vertex_iterator lastNode = vItrRange.second;

	for (Graph::vertex_iterator vItr = firstNode; vItr != lastNode; ++vItr) {
		// Get iterators pointing to the beginning and end of the list of nodes adjacent to node v
		pair<Graph::adjacency_iterator, Graph::adjacency_iterator> adjRange = adjacent_vertices(*vItr, g);
		// Loop over adjacent nodes
		for (Graph::adjacency_iterator Itr = adjRange.first; Itr != adjRange.second; ++Itr) {
			temp = g[*vItr].color;
			g[*vItr].color = g[*Itr].color;
			g[*Itr].color = temp;
			
			if (getConflicts(g) < numConflicts) {
				bestN = Graph(g);
				numConflicts = getConflicts(bestN);
			}
			g[*Itr].color = g[*vItr].color;
			g[*vItr].color = temp;
		}
	}
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