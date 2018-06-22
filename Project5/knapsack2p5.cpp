// Project 5 Knapsack 2: Solving knapsack using simulated
// annealing and greedy algorithm
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
void simAnnealing(knapsack &k);
knapsack getRandNeighbor(knapsack &k);
bool chance(int th);

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

void simAnnealing(knapsack &k) {
	knapsack bestSol = knapsack(k);
	//threshold value
	float th = 1;
	float alpha = 0.9;
	
	//number of times to increment
	int totalIncrements = 30;
	//increment num
	int thIncrement = 0;

	while (thIncrement <= totalIncrements) {
		for (int i = 0; i < k.getNumObjects(); i++) {
			knapsack temp = getRandNeighbor(k);
			if (temp.getValue() > bestSol.getValue()) {
				bestSol = temp;
				k = temp;
			}
			else if (temp.getValue() > k.getValue()) {
				k = temp;
			}
			else if (chance(th)) {
				k = temp;
			}
		}
		th *= alpha;
		thIncrement += 1;
	}
	k = bestSol;
}

knapsack getRandNeighbor(knapsack &k) {
	knapsack temp = knapsack(k);
	vector<int> selected;
	for (int i = 0; i < temp.getNumObjects(); i++) {
		if (temp.isSelected(i)) {
			selected.push_back(i);
		}
	}
	srand(time(0));
	int start = rand() % selected.size();

	temp.unSelect(start);
	for (int j = 0; j < temp.getNumObjects(); j++) {
		if ((!temp.isSelected(j)) & ((temp.getCost() + temp.getCost(j)) <= temp.getCostLimit())) {
			temp.select(j);
		}
	}
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