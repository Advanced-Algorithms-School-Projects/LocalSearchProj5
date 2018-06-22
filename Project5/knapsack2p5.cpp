// Project 5 Knapsack 2: Solving knapsack using local search
// and greedy algorithm
// Created by Patrick Hanbury and Cassandra Smith
// Submitted 6/25/18
//

#include <iostream>
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


void greedyKnapsack(knapsack &k);
void orderKnapsack(knapsack &k, vector<int> &items);

int main()
{
	char x;
	ifstream fin;
	string fileName;

	// Read the name of the graph from the keyboard or
	// hard code it here for testing.

	fileName = "knapsack/knapsack1024.input";

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

		greedyKnapsack(k);

		//string output = "knapsack1024.output";
		
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


void greedyKnapsack(knapsack &k) {
	vector<int> items;
	items.resize(k.getNumObjects());
	for (int i = 0; i < k.getNumObjects(); i++) {
		items[i] = i;
	}

	//Order knapsack items by decreasing value 
	orderKnapsack(k, items);

	//Loop through items
	//Choose first/next item that does not have a cost more than the cost limit
	for (int i = 0; i < k.getNumObjects(); i++) {
		if ((k.getCost() + k.getCost(items[i])) <= k.getCostLimit()) {
			k.select(items[i]);
		}
	}
}

void orderKnapsack(knapsack &k, vector<int> &items) {
	int temp, j;

	for (int i = 1; i < k.getNumObjects(); i++) {
		temp = items[i];
		j = i - 1;

		while (j >= 0 && k.getRatio(items[j]) < k.getRatio(temp)) {
			items[j + 1] = items[j];
			j = j - 1;
		}
		items[j + 1] = temp;
	}
}