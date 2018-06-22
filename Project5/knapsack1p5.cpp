// Project 5 Knapsack 1: Solving knapsack using local search
// Created by Patrick Hanbury and Cassandra Smith
// Submitted 6/25/18
//

#include <iostream>
#include <stdlib.h>
#include <limits.h>
#include <list>
#include <fstream>
#include <queue>
#include <vector>
#include <time.h>

using namespace std;

#include "d_except.h"
#include "d_matrix.h"
#include "knapsack.h"


void randomKnapsack(knapsack &k);
void steepDescentKnapsack(knapsack &k);
void bestNeighbor(knapsack &k);

int main()
{
	char x;
	ifstream fin;
	string fileName;

	// Read the name of the graph from the keyboard or
	// hard code it here for testing.

	fileName = "knapsack/knapsack8.input";


	//cout << "Enter filename" << endl;
	//cin >> fileName;

	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}

	try
	{
		cout << "Reading knapsack instance" << endl;
		knapsack k(fin);

		randomKnapsack(k);
		steepDescentKnapsack(k);

		//string output = "knapsack8.output";

		cout << endl << "Best solution" << endl;
		k.printSolution();
	}

	catch (indexRangeError &ex)
	{
		cout << ex.what() << endl; exit(1);
	}
	catch (rangeError &ex)
	{
		cout << ex.what() << endl; exit(1);
	}

	system("pause");
}

void steepDescentKnapsack(knapsack &k) {
	knapsack bestSol = knapsack(k);
	bool done = false;

	while (!done) {
		bestNeighbor(k);
		
		if (bestSol == k) {
			done = true;
		}
		else {
			bestSol = k;
		}
	}
}

void bestNeighbor(knapsack &k) {
	knapsack bestN = knapsack(k);

	for (int i = 0; i < k.getNumObjects(); i++) {
		if (k.isSelected(i)) {
			k.unSelect(i);
			for (int j = (i + 1); j < k.getNumObjects(); j++) {
				if (!k.isSelected(j) & ((k.getCost() + k.getCost(j)) <= k.getCostLimit())) {
					k.select(j);
				}
			}
			if (k.getValue() > bestN.getValue()) {
				bestN = knapsack(k);
			}
		}
	}
	k = bestN;
}

void randomKnapsack(knapsack &k) {
	srand(time(0));
	for (int i = 0; i < k.getNumObjects(); i++) {
		if ((rand() % 2) & ((k.getCost() + k.getCost(i)) <= k.getCostLimit())) {
			k.select(i);
		}
	}
}