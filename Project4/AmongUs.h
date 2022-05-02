//9B734EC0C043C5A836EA0EBE4BEFEA164490B2C7
#pragma once

class Node
{
public:
	double minWeight = 0;
	int xCord = 0, yCord = 0;
	int currVertex = 0, parentVertex = 0;
	bool isInGraph;
	char location;
};

class TSPNode
{
public:
	double minWeight = 0;
	size_t currIndex;
	int xCord = 0, yCord = 0;
	bool isInGraph;
};

//Functor to compare node objects
struct nodeCompare
{
	bool operator()(Node* lhs, Node* rhs)
	{
		if (lhs->minWeight > rhs->minWeight)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class amongUs
{
	//Vectors to store positions and types for MST, FASTTSP, and OPTTSP
	std::vector<Node> imposterMST; //Part A
	std::vector<TSPNode> ghostMST; //Part B & C
	std::vector<int> partBTSP; //Part B
	std::vector<int> partCTSP;//Part C
	double graphWeight = 0;
	int numRooms = 0;

public:
	//Member functions
	void getOpts(int, char*[]);
	void printHelp();
	void readInMST();
	void MSTPathandPrint();
	double EuclideanDistance(int, int, bool);
	void readinFASTTSP();
	void nearestNeighbor(std::vector<int>&);
	void twoOpt(std::vector<int>&);
	void printTSP(std::vector<int>&);
	void readInOPTTSP();
	void genPerms(std::vector<int>&, size_t);
	bool isPromising(std::vector<int>&, size_t, double);




};