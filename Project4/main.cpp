//9B734EC0C043C5A836EA0EBE4BEFEA164490B2C7
#include <vector>
#include <limits>
#include <string>
#include <iostream>
#include <utility>
#include <getopt.h>
#include <algorithm>
#include <cmath>
#include <climits>
#include <iomanip>
#include <cfloat>
#include <iostream>
#include "AmongUs.h"

using namespace std;
double currPathLength = 0;

void amongUs::getOpts(int argc, char* argv[])
{
	//getopt housekeeping
	int choice = 0;
	int option_index = 0;
	string type;

	option long_options[] =
	{
	   {"mode", required_argument,   nullptr, 'm'},
	   {"help", no_argument,         nullptr, 'h'},
	   { nullptr, 0,                 nullptr, '\0' }
	};

	while ((choice = getopt_long(argc, argv, "m:h", long_options, &option_index)) != -1)
	{
		switch (choice)
		{
		case 'm':
			type = *optarg;
			break;

		case 'h':
			printHelp();
			break;

		default:
			cerr << "Error: invalid option" << endl;
			exit(0);
		} // switch
	} // while

	switch (type[0])
	{
		case 'M':
		{
			readInMST();
			MSTPathandPrint();
			break;
		}
		case 'O':
		{
			readInOPTTSP();
			nearestNeighbor(partCTSP);
			twoOpt(partCTSP);
			twoOpt(partCTSP);
			graphWeight += sqrt(EuclideanDistance(partCTSP[0], partCTSP.back(), false));
			genPerms(partCTSP, 1);
			printTSP(partCTSP);
			break;
		}
		case 'F':
		{
			readinFASTTSP();
			nearestNeighbor(partBTSP);
			twoOpt(partBTSP);
			twoOpt(partBTSP);
			graphWeight += sqrt(EuclideanDistance(partBTSP[0], partBTSP.back(), false));
			printTSP(partBTSP);
			break;
		}
		default:
		{
			cerr << "Must selected a valid mode " << endl;
		}
	}
}

void amongUs::printHelp()
{
	cout << "Dude this has been going on all semester. Its getting ridculous at this point. Get your shit together." << endl;
	exit(0);
}

void amongUs::readInMST()
{
	cin >> numRooms;

	//Create a temp node and x/y variables to store each room
	Node tempNode;
	int tempNodeX = 0, tempNodeY = 0;
	int tempNodeWeight = 0;
	bool isDecontamination = false, isLab = false, isOuter = false;

	//Continue to read in coordinates while the file is not empty
	while (cin >> tempNodeX >> tempNodeY)
	{
		tempNode.xCord = tempNodeX;
		tempNode.yCord = tempNodeY;
		tempNode.currVertex = tempNodeWeight;
		tempNode.isInGraph = false;
		tempNode.minWeight = DBL_MAX;
		tempNode.parentVertex = INT_MAX;

		//If the node is in quadrant 3, mark it in the lab
		if ((tempNodeX < 0 && tempNodeY < 0))
		{
			tempNode.location = 'l';
			isOuter = true;
		}
		//Check if node is on the decontamination line
		else if ((tempNodeX <= 0 && tempNodeY == 0) || (tempNodeX == 0 && tempNodeY <= 0))
		{
			tempNode.location = 'd';
			isDecontamination = true;
		}
		//Node must be in the outer region
		else
		{
			tempNode.location = 'o';
			isLab = true;
		}
		//Push the temp node to the vector to caclulate path later
		imposterMST.push_back(tempNode);
		tempNodeWeight++;
	}

	//If a node is in outer region and the lab, but not in decontamination, there cant be an MST
	if (isLab == true && isOuter == true && isDecontamination == false)
	{
		cerr << "Cannot construct MST" << endl;
		exit(1);
	}

	//MSTPathandPrint();
}

void::amongUs::MSTPathandPrint() 
{
	vector<int> path;
	double totalDistance = 0;
	int numNodes = 0, currIndex = 0, counter = 0;

	//Follow the algorithm from lecture slides
	//Set the first index to 0 to calculate the weight
	imposterMST[0].minWeight = 0;

	//Finding the path
	//while(numNodes < int(imposterMST.size()))
	//Iterate until the path has the same number of nodes as read in
	for (; numNodes < (int)imposterMST.size();)
	{
		//Reset the distance and counter variables each iteration
		double currDistance = DBL_MAX;
		counter = 0;
		//Iterate through imposterMST to find the smallest distance
		for (auto& it : imposterMST)
		{
			//If the node is not in the graph and less than the current distance, reset the smallest weight
			if (it.isInGraph == false)
			{
				if (currDistance > it.minWeight)
				{
					currDistance = it.minWeight;
					currIndex = counter;
				}
			}
			counter++;
		}

		//Mark the index as visited and then push back the index 
		imposterMST[currIndex].isInGraph = true;
		path.push_back(currIndex);
		//Update the total graph weight and the number of nodes
		graphWeight += sqrt(currDistance);
		numNodes++;

		//Iterate again through the entire vector and update the distances
		for (int i = 0; i < (int)imposterMST.size(); i++)
		{
			//If the index of the smallest weight is not equal to the current index and is not in the graph, update the distances
			if ((i != currIndex) && (imposterMST[i].isInGraph == false))
			{
				if ((imposterMST[currIndex].location != 'l' || imposterMST[i].location != 'o') && (imposterMST[currIndex].location != 'o' || imposterMST[i].location != 'l'))
				{
					//Get the distance
					totalDistance = EuclideanDistance(currIndex, i, true);

					if (imposterMST[i].minWeight > totalDistance)
					{
						//If the minimum weight of each node is greater than the total distance, update the distance and change the parent vertex
						imposterMST[i].minWeight = totalDistance;
						imposterMST[i].parentVertex = currIndex;
					}
				}
			}
		}
	}

	//Printing

	//Print out the graph weight followed by each node's coordiante
	cout << graphWeight << endl;

	for (int i = 1; i < (int)path.size(); i++)
	{
		//Check to print in the right order
		if (path[i] > imposterMST[path[i]].parentVertex)
		{
			cout << imposterMST[path[i]].parentVertex << " " << path[i] << endl;
		}
		else
		{
			cout << path[i] << " " << imposterMST[path[i]].parentVertex << endl;
		}
	}

}

double amongUs::EuclideanDistance(int node1, int node2, bool isPartA)
{
	if (isPartA == true)
	{
		//Create 2 nodes indexed at the passed in indicies
		Node& a = imposterMST[node1];
		Node& b = imposterMST[node2];

		//return the Euclidean distance formula
		return double((pow((double)b.xCord - (double)a.xCord, 2) + pow((double)b.yCord - (double)a.yCord, 2)));
	}
	else
	{
		TSPNode& a = ghostMST[node1];
		TSPNode& b = ghostMST[node2];

		return double((pow(b.xCord - a.xCord, 2) + pow(b.yCord - a.yCord, 2)));
	}
}

void amongUs::readinFASTTSP()
{
	//Copy and paste code from MST read in
	cin >> numRooms;

	//Create a temp node and x/y variables to store each room
	TSPNode tempNode;
	int tempNodeX = 0, tempNodeY = 0;

	while (cin >> tempNodeX >> tempNodeY)
	{
		tempNode.xCord = tempNodeX;
		tempNode.yCord = tempNodeY;
		tempNode.isInGraph = false;
		tempNode.minWeight = DBL_MAX;

		ghostMST.push_back(tempNode);
	}
}

//nearest neighbor algorithm
void amongUs:: nearestNeighbor(vector<int>& partBorCTSP)
{
	int pathLength = 0, numNodesVisited = 0, counter = 0, nextNodeIndex = 0;
	double totalDistance = 0, minDistance = 0;

	//Loop until the length of the tsp path is less than the graph size
	//while (pathLength < (int)ghostMST.size())
	for(pathLength = 0; pathLength < int(ghostMST.size()); pathLength++)
	{
		//Reset the distance and counter variables each iteration
		minDistance = DBL_MAX;
		counter = 0;
		//Loop through nodes and calculate the distance only if the current node is not in the path
		for (auto& it : ghostMST)
		{
			if (it.isInGraph == false)
			{
				//Calculate the current distance
				totalDistance = EuclideanDistance(numNodesVisited, counter, false);

				//reset the smallest distance if necessary
				if (minDistance > totalDistance)
				{
					minDistance = totalDistance;
					nextNodeIndex = counter;
				}
			}
			counter++;
		}

		//Mark the shorest distance node as in the graph
		ghostMST[nextNodeIndex].isInGraph = true;
		//Add the weight of the edge to the total graph weight
		graphWeight += sqrt(minDistance);
		//Push back the node for printing
		partBorCTSP.push_back(nextNodeIndex);
		//update the current node to get the next distance
		numNodesVisited = nextNodeIndex;
	}

}

void amongUs::twoOpt(vector<int>& optimalTSP)
{
	//Initalize 4 doubles and ints to store the distance and nodes
	double dist1 = 0, dist2 = 0, dist3 = 0, dist4 = 0;
	int node1X = 0, node1Y = 0, node2X = 0, node2Y = 0;

	for (int i = 1; i < int(optimalTSP.size() - 1); i++)
	{
		for (int j = i + 1; j < int(optimalTSP.size()); j++)
		{
			//set the four int to be respective indicies
			node1X = optimalTSP[i - 1];
			node1Y = optimalTSP[i];
			node2X = optimalTSP[j - 1];
			node2Y = optimalTSP[j];

			//calculate the distances between node1x and node1y
			dist1 = EuclideanDistance(node1X, node1Y, false);
			//caculate the distance between node1x and node2x
			dist2 = EuclideanDistance(node1X, node2X, false);
			//calculate the distance between node2x and node2y
			dist3 = EuclideanDistance(node2X, node2Y, false);
			//calculate the distance between node1y and node2y
			dist4 = EuclideanDistance(node1Y, node2Y, false);

			if ((dist2 + dist4) < (dist1 + dist3))
			{
				//change to reverse and see if it works
				reverse(optimalTSP.begin() + i, optimalTSP.begin() + j);

				//update graph weight
				graphWeight -= (sqrt(dist1) + sqrt(dist3));
				graphWeight += (sqrt(dist2) + sqrt(dist4));
			}
		}
	}
}

void amongUs::printTSP(vector<int>& path)
{
	cout << graphWeight << endl;

	//loop through and print the nodes in the order they were visited
	for (int i = 0; i < (int)path.size(); i++) 
	{
		cout << path[i];

		if ((i != (int)path.size() - 1))
		{
			cout << " ";
		}
	}
	cout << endl;
}

void amongUs::readInOPTTSP()
{
	//Copy and paste code from MST read in
	cin >> numRooms;

	//Create a temp node and x/y variables to store each room
	TSPNode tempNode;
	int tempNodeX = 0, tempNodeY = 0;

	while (cin >> tempNodeX >> tempNodeY)
	{
		tempNode.xCord = tempNodeX;
		tempNode.yCord = tempNodeY;
		tempNode.isInGraph = false;
		tempNode.minWeight = DBL_MAX;

		ghostMST.push_back(tempNode);
	}
}

void amongUs::genPerms(vector<int>& path, size_t permLength)
{
	double totalDistance = 0;
	vector<int> optimalPath;

	//From the gemPerms given
	if (permLength == path.size())
	{
		//Calculate the distance from the start and end node
		totalDistance = EuclideanDistance(0, path.back(), false);
		currPathLength += sqrt(totalDistance);

		//Find the smallest path length seen yet
		if (currPathLength < graphWeight)
		{
			graphWeight = currPathLength;
			optimalPath = path;
		}

		currPathLength -= sqrt(totalDistance);
		return;
	}
	//No point in checking if its not promising
	if (isPromising(path, permLength, currPathLength) == false)
	{
		return;
	}

	for (size_t i = permLength; i < path.size(); i++)
	{
		//From lecture slides
		swap(path[permLength], path[i]);
		currPathLength += sqrt(EuclideanDistance(path[permLength], path[permLength - 1], false));
		genPerms(path, permLength + 1);
		currPathLength -= sqrt(EuclideanDistance(path[permLength], path[permLength - 1], false));
		swap(path[permLength], path[i]);
	}
}

bool amongUs::isPromising(vector<int>& TSP, size_t permLength, double currPathLength)
{
	double minDist = DBL_MAX, totalGraphWeight = 0;
	int numImposters = int(permLength), currNode = 0;
	double start = DBL_MAX;
	double end = DBL_MAX;

	//Check to speed up promising function
	if ((int)TSP.size() - permLength >= 4)
	{
		if (currPathLength < graphWeight)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		//Vector to store optimal tsp
		vector<int> optimalPath;
		//Auxillary vector to make adding elements easier
		vector<int> leftover(TSP.begin() + permLength, TSP.end());

		//set initial weight
		ghostMST[leftover[0]].minWeight = 0;
		 
		//While the perm length is smaller than the total number of nodes
		while (numImposters < (int)TSP.size())
		{
			int tempImposter = 0;
			for (int i = 0; i < int(leftover.size()); i++)
			{
				tempImposter = leftover[i];

				if (ghostMST[tempImposter].minWeight < minDist)
				{
					currNode = i;
					minDist = ghostMST[tempImposter].minWeight;
				}
			}
			//push back the node with the smallest weight
			optimalPath.push_back(leftover[currNode]);

			//erase the node so it doesnt get added again on next iteration
			leftover.erase(leftover.begin() + currNode);

			//update the path length
			totalGraphWeight += sqrt(ghostMST[optimalPath.back()].minWeight);
			numImposters++;
			ghostMST[optimalPath.back()].minWeight = DBL_MAX;
		
			for (int i = 0; i < int(leftover.size()); i++)
			{
				double distance = EuclideanDistance(optimalPath.back(), leftover[i], false);

				if (distance < ghostMST[leftover[i]].minWeight)
				{
					ghostMST[leftover[i]].minWeight = distance;
				}
			}
		}

		for (int i = 0; i < int(optimalPath.size()); i++)
		{
			double x = EuclideanDistance(optimalPath[i], TSP[permLength - 1], false);
			double y = EuclideanDistance(optimalPath[i], TSP[0], false);

			if (x < start)
			{
				start = x;
			}

			if (y < end)
			{
				end = y;
			}
		}

		totalGraphWeight += sqrt(start);
		totalGraphWeight += sqrt(end);
		totalGraphWeight += currPathLength;

		if (totalGraphWeight <= graphWeight)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

int main(int argc, char* argv[])
{
	cout << setprecision(2);
	cout << fixed;

	std::ios_base::sync_with_stdio(false);

	amongUs run;
	run.getOpts(argc, argv);
}