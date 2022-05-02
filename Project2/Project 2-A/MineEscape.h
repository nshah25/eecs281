//19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F
#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <getopt.h>
#include <queue>
#include <algorithm>

using namespace std;
class Tile
{
public:

	bool isScene;
	bool isVisited;
	int row;
	int col;
	int origVal;
	int tntValue;

	//overloaded < operator to compare Tile objects
	bool operator<(const Tile& rightVal) const 
	{
		if ((tntValue == rightVal.tntValue))
		{
			if (col == rightVal.col)
			{
				if (row > rightVal.row)
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
				if (col > rightVal.col)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
			if (tntValue > rightVal.tntValue)
			{
				return true;
			}
			else
			{
				return false;
			}
	}
};
struct tileGreater
{
public:
	//Overloaded () Greater than operator
	bool operator() (Tile& lhs, Tile& rhs)
	{
		if ((lhs.tntValue == rhs.tntValue))
		{
			if (lhs.col == rhs.col)
			{
				if (lhs.row > rhs.row)
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
				if (lhs.col > rhs.col)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			if (lhs.tntValue > rhs.tntValue)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
};
//Overloaded less than operator for tiles
struct tileLess
{
public:
	bool operator() (Tile& lhs, Tile& rhs)
	{
		if (lhs.tntValue == rhs.tntValue)
		{
			if (lhs.col == rhs.col)
			{
				if (lhs.row < rhs.row)
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
				if (lhs.col < rhs.col)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			if (lhs.tntValue < rhs.tntValue)
			{
				return true;
			}
			else
			{
				return false;

			}
		}
	}
};

class MineEscape
{
public:
	//Variables to read in command line, track position, and keep track of rubble
	bool isVerbose = false;
	bool isSummarization = false;
	bool isMedian = false;
	int currCol = 0;
	int currRow = 0;
	int mapSize = 0;
	int statsVal = 0;

	//2D vector of tiles to represent the map
	vector<vector<Tile>> mine;
	//Vector of tile pointers to track rubble
	vector<Tile> rubblePQ;
	//2 priority queues to track upper and lower tiles
	priority_queue<Tile, vector<Tile>, tileGreater> pqGreater;
	priority_queue<Tile, vector<Tile>, tileLess> pqLess;

	//Member functions
	//get options handling
	void getOpts(int, char* []);
	//Read in the map
	void readMap();
	//Backtracking to find the optimal path
	void backtrack();
	//Will print tiles cleared, summarization if flag is given, and median difficulty
	//int = 1 ==> print tiles cleared, int = 2 --> print summarization, int = 3 --> print median diff
	void printSummary(int);
	//Compares tiles to find the best path
	static bool compare(Tile&, Tile&);
	//Handles all TNT explosions
	void calculateTNT(const Tile&,priority_queue<Tile, vector<Tile>, tileGreater>&);
	//Function to do edge checks on tnt explosions
	void TNTPq(Tile*, priority_queue<Tile, vector<Tile>, tileGreater>&);
	//Fucntion to calculate median difficulty
	void caclulateMedian(Tile*);
	//Generate the map if its random
	void generateRandom(stringstream&);
	//Updates member varaibles after reading in the map
	void updateTile(Tile&, bool&);
	//Check if miner is at the edge
	bool checkIfEdge(bool);
	//Checks north, south, east, and west for tnt
	void checkDirections(priority_queue<Tile, vector<Tile>, tileGreater>&);
	//Push the tile for median calculation
	void addToMedianPQ();
	//Update varaibles inside the backtrack function
	void updateBackrack();
	//Checks all 4 directions for tnt
	void checkNorth(priority_queue<Tile, vector<Tile>, tileGreater>&);
	void checkSouth(priority_queue<Tile, vector<Tile>, tileGreater>&);
	void checkEast(priority_queue<Tile, vector<Tile>, tileGreater>&);
	void checkWest(priority_queue<Tile, vector<Tile>, tileGreater>&);
	//Initalizes the tnt priority queue with the first tile of the queue
	void initalizeTNTPQ(const Tile&, priority_queue<Tile, vector<Tile>, tileGreater>& pq);
	//Functions to update tnt pq based on if they have tnt or not
	void updateTNTTracker(Tile*, priority_queue<Tile, vector<Tile>, tileGreater>&, priority_queue<Tile, vector<Tile>, tileGreater>&);
	void updateNonTNTTracker(Tile*, priority_queue<Tile, vector<Tile>, tileGreater>&);
	void readInStartVals(Tile&, int, int, int);
};