//19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F
#include <iostream>
#include<sstream>
#include <vector>
#include <sstream>
#include <getopt.h>
#include <iomanip>
#include <climits>
#include "MineEscape.h"
#include "P2random.h"
using namespace std;

void MineEscape::getOpts(int argc, char* argv[])
{
	//getopt housekeeping
	int choice = 0;
	int option_index = 0;

	option long_options[] =
	{
	   {"help", no_argument,         nullptr, 'h'},
	   {"stats", required_argument,  nullptr, 's'},
	   {"median", no_argument,       nullptr, 'm'},
	   {"verbose", no_argument,      nullptr, 'v'},
	   { nullptr, 0,                 nullptr, '\0'}
	};

	while ((choice = getopt_long(argc, argv, "hs:mv", long_options, &option_index)) != -1)
	{
		switch (choice)
		{
		case 'h':
			cout << "Figure it out";
			exit(0);
		case 's':
			isSummarization = true;
			statsVal = stoi(optarg);
			break;
		case 'm':
			isMedian = true;
			break;
		case 'v':
			isVerbose = true;
			break;
		default:
			cerr << "Error: invalid option" << endl;
			exit(0);

		} // switch
	} // while
}

void MineEscape::readMap()
{
	char mapType, whitespace;
	cin >> mapType >> ws;

	//Error checking to make sure map is right type
	if (mapType != 'R' && mapType != 'M')
	{
		cerr << "Invalid entry. Map type must be R or M." << endl;
		exit(1);
	}

	//Ignore the "size" string
	cin >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace;
	//Get the map size
	cin >> mapSize >> ws;

	//Ignore the "start " string and read in the row and column
	cin >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace;
	cin >> currRow >> ws >> currCol >> ws;

	//More error checking
	if (currCol >= mapSize || currRow >= mapSize || currCol < 0 || currRow < 0)
	{
		cerr << "Invalid row or column index." << endl;
		exit(1);
	}

	//Random mine handling
	stringstream readInput;
	if (mapType == 'R' || mapType == 'r')
	{
		generateRandom(readInput);
	}
	istream& inputStream = (mapType == 'M') ? cin : readInput;

	//Reserve space for map
	mine.reserve(mapSize);
	//Create a new vector of tiles and then push it back into the 2D array
	int row = 0, col = 0;

	for (int i = 0; i < mapSize; i++)
	{
		vector<Tile> currRow;
		currRow.reserve(mapSize);
		mine.push_back(currRow);
	}

	//Add tiles to median tracker
	if (isMedian == true)
	{
		addToMedianPQ();
	}

	int tileVal = 0;
	while (inputStream >> tileVal)
	{
		bool seen = true;
		Tile currTile;
		if (row == currRow && col == currCol)
		{
			updateTile(currTile, seen);
		}
		else
		{
			seen = false;
			updateTile(currTile, seen);
		}
		//Intialize each tiles starter varaibles
		readInStartVals(currTile, tileVal, row, col);

		//Insert the row into the vector and read in the next index
		mine[row].push_back(currTile);

		//If you are the end of the column, start looking at the next row
		if (col == mapSize - 1)
		{
			col = 0;
			row++;
		}
		//else look at the next index in the row
		else
		{
			col++;
		}
	}
}

void MineEscape::generateRandom(stringstream& readInput)
{
	char whitespace;
	int seedVal = 0, maxRubble = 0, tnt = 0;
	cin >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace;
	cin >> seedVal >> ws;

	cin >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace >> whitespace;
	cin >> maxRubble >> ws;

	cin >> whitespace >> whitespace >> whitespace >> whitespace;
	cin >> tnt >> ws;

	P2random::PR_init(readInput, mapSize, seedVal, maxRubble, tnt);
}

void MineEscape::addToMedianPQ()
{
	//Push two dummy tiles into the greater and less pq to keep track and calculate median
	Tile dummyMax, dummyMin;
	dummyMax.tntValue = INT_MAX;
	dummyMin.tntValue = INT_MIN;
	pqGreater.push(dummyMax);
	pqLess.push(dummyMin);
}

void MineEscape::readInStartVals(Tile& currTile, int tileVal, int row, int col)
{
	currTile.tntValue = tileVal;
	currTile.origVal = tileVal;
	currTile.row = row;
	currTile.col = col;
}

void MineEscape::updateTile(Tile& currTile, bool& seen)
{
	//If the tile has been scene, mark it as visited
	if (seen == true)
	{
		currTile.isVisited = true;
		currTile.isScene = true;
	}
	//If not, mark it as unseen
	else
	{
		currTile.isVisited = false;
		currTile.isScene = false;
	}
}

void MineEscape::backtrack()
{
	//Bool to track if miner is at the edge of the mine
	bool isEdge = false;
	//Priority queue to track visible tiles
	priority_queue<Tile, vector<Tile>, tileGreater> pq;
	Tile* possibleTNT = &mine[currRow][currCol];

	//Tile is a tnt
	if (mine[currRow][currCol].tntValue == -1)
	{
		calculateTNT(mine[currRow][currCol], pq);
	}
	//Not tnt
	else if (mine[currRow][currCol].tntValue > 0)
	{
		//Print out the current tile and tnt if verbose command flag is given
		if (isVerbose == true)
		{
			cout << "Cleared: " << mine[currRow][currCol].tntValue << " at [" << mine[currRow][currCol].row << "," << mine[currRow][currCol].col << "]\n";
		}
		//Push tile to median pq if verbose command flag is given
		if (isMedian == true)
		{
			caclulateMedian(&mine[currRow][currCol]);
			printSummary(3);
		}

		//Push back the cleared tile, update the tnt valuem and mark the tile as seen
		updateBackrack();
	}

	isEdge = checkIfEdge(isEdge);
	//While the miner is not at the edge of the mine, keep looking
	while (isEdge == false)
	{
		checkDirections(pq);
		bool isValid = true;
		//Continue to loop wihile miner can make a valid move
		while (isValid == true)
		{
			//Initalize the temp tile to the top of the pq
			possibleTNT = &mine[pq.top().row][pq.top().col];
			//the temp tile and pq have the same tnt value
			if ((*possibleTNT).tntValue == pq.top().tntValue)
			{
				//Reset the top elements
				int topRow = (*possibleTNT).row;
				currRow = topRow;
				int topCol = (*possibleTNT).col;
				currCol = topCol;

				//Calculate tnt explosion when the tile is tnt
				if ((*possibleTNT).tntValue == -1)
				{
					calculateTNT(pq.top(), pq);
				}
				//Top is not TNT
				else if ((*possibleTNT).tntValue > 0)
				{
					//If verbose flag is given, print the cleared TNT
					if (isVerbose == true)
					{
						cout << "Cleared: " << mine[currRow][currCol].origVal << " at [" << mine[currRow][currCol].row << "," << mine[currRow][currCol].col << "]\n";
					}
					//If median flag is given, print the median
					if (isMedian == true)
					{
						caclulateMedian(&mine[(*possibleTNT).row][(*possibleTNT).col]);
						printSummary(3);
					}
					//After expolding, set tnt to 0 and push to main pq
					(*possibleTNT).tntValue = 0;
					rubblePQ.push_back(mine[currRow][currCol]);
				}
				//Pop tile and set current location as visited
				pq.pop();
				mine[currRow][currCol].isVisited = true;

				isEdge = checkIfEdge(isEdge);
				isValid = false;
			}
			else
				pq.pop();
		}
	}
}

void MineEscape::updateBackrack()
{
	//Push back the cleared tile, update the tnt valuem and mark the tile as isScene
	rubblePQ.push_back(mine[currRow][currCol]);
	mine[currRow][currCol].tntValue = 0;
	mine[currRow][currCol].isVisited = true;
}

bool MineEscape::checkIfEdge(bool isEdge)
{
	//Check if the miner is at any of the 4 edges
	if (currCol == mapSize - 1 || currRow == mapSize - 1 || currCol == 0 || currRow == 0)
	{
		isEdge = true;
	}
	return isEdge;
}

void MineEscape::checkDirections(priority_queue<Tile, vector<Tile>, tileGreater>& pq)
{
	//Call check functions on all 4 directions
	checkNorth(pq);
	checkEast(pq);
	checkSouth(pq);
	checkWest(pq);
}

void MineEscape::checkNorth(priority_queue<Tile, vector<Tile>, tileGreater>& pq)
{
	//Check if tile above has been discovered, if not, mark it and push it to the pq
	if (mine[currRow + 1][currCol].isScene == false)
	{
		mine[currRow + 1][currCol].isScene = true;
		pq.push(mine[currRow + 1][currCol]);
	}
}

void MineEscape::checkEast(priority_queue<Tile, vector<Tile>, tileGreater>& pq)
{
	//Check if tile right has been discovered, if not, mark it and push it to the pq
	if (mine[currRow][currCol + 1].isScene == false)
	{
		mine[currRow][currCol + 1].isScene = true;
		pq.push(mine[currRow][currCol + 1]);
	}
}

void MineEscape::checkSouth(priority_queue<Tile, vector<Tile>, tileGreater>& pq)
{
	//Check if tile below has been discovered, if not, mark it and push it to the pq
	if (mine[currRow - 1][currCol].isScene == false)
	{
		mine[currRow - 1][currCol].isScene = true;
		pq.push(mine[currRow - 1][currCol]);
	}
}

void MineEscape::checkWest(priority_queue<Tile, vector<Tile>, tileGreater>& pq)
{
	//Check if tile left has been discovered, if not, mark it and push it to the pq
	if (mine[currRow][currCol - 1].isScene == false)
	{
		mine[currRow][currCol - 1].isScene = true;
		pq.push(mine[currRow][currCol - 1]);
	}
}

void MineEscape::printSummary(int x)
{
	//Regular tile print
	if (x == 1)
	{
		int numTilesCleared = 0;
		int totalRubble = 0;
		//Loop through the pq and update the totals for non tnt tiles
		for (int i = 0; i < int(rubblePQ.size()); i++)
		{
			if (rubblePQ[i].origVal != -1)
			{
				totalRubble += rubblePQ[i].origVal;
				numTilesCleared++;
			}
		}
		//Print
		cout << "Cleared " << numTilesCleared << " tiles " << "containing " << totalRubble << " rubble and escaped. \n";
	}
	//Print summary
	if (x == 2)
	{
		int counter = 0, index = 0;
		cout << "First tiles cleared:\n";
		//While index is less than size and counter is less than statsVal, continue to print out tiles
		while (index < (int)rubblePQ.size() && counter < statsVal)
		{
			//Print tnt tile
			if (rubblePQ[index].origVal == -1)
			{
				cout << "TNT";
			}
			//Print regular tile
			else
			{
				cout << rubblePQ[index].origVal;
			}
			cout << " at [" << rubblePQ[index].row << "," << rubblePQ[index].col << "]\n";

			index++;
			counter++;
		}

		//reset counter and index
		counter = 0;
		index = (int)rubblePQ.size() - 1;

		cout << "Last tiles cleared:\n";
		//While index is greater than 0 and counter is less than statsVal, continue to print out tiles
		while (index >= 0 && counter < statsVal)
		{
			//Print tnt tile
			if (rubblePQ[index].origVal == -1)
			{
				cout << "TNT";
			}
			//Print regular tile
			else
			{
				cout << rubblePQ[index].origVal;
			}
			cout << " at [" << rubblePQ[index].row << "," << rubblePQ[index].col << "]\n";

			index--;
			counter++;
		}

		sort(rubblePQ.begin(), rubblePQ.end(), compare);

		counter = 0, index = 0;
		cout << "Easiest tiles cleared:\n";

		//While index is less than size and counter is less than statsVal, continue to print out tiles
		while (index < (int)rubblePQ.size() && counter < statsVal)
		{
			//Print tnt tile
			if (rubblePQ[index].origVal == -1)
			{
				cout << "TNT";
			}
			//Print regular tile
			else
			{
				cout << rubblePQ[index].origVal;
			}
			cout << " at [" << rubblePQ[index].row << "," << rubblePQ[index].col << "]\n";

			index++;
			counter++;
		}

		//reset counter and index
		counter = 0;
		index = (int)rubblePQ.size() - 1;

		cout << "Hardest tiles cleared:\n";
		//While index is greater than 0 and counter is less than statsVal, continue to print out tiles
		while (index >= 0 && counter < statsVal)
		{
			//Print tnt tile
			if (rubblePQ[index].origVal == -1)
			{
				cout << "TNT";
			}
			//Print regular tile
			else
			{
				cout << rubblePQ[index].origVal;
			}
			cout << " at [" << rubblePQ[index].row << "," << rubblePQ[index].col << "]\n";

			index--;
			counter++;
		}
	}
	//Print median
	if (x == 3)
	{
		cout << "Median difficulty of clearing rubble is: ";

		//Set variables equal to the value of the top elements of the median queues
		int pqGreaterTop = pqGreater.top().origVal;
		int pqLessTop = pqLess.top().origVal;

		//If sizes are equal divide by 2
		if (pqGreater.size() == pqLess.size())
		{
			cout << (pqGreaterTop + pqLessTop) / 2.0 << "\n";
		}
		//If pqLess is bigger than pqGreater, print the top of pqLess
		else if (pqGreater.size() < pqLess.size())
		{
			cout << pqLessTop << ".00\n";
		}
		//If pqLess is less than pqGreater, print the top of pqGreater
		else
		{
			cout << pqGreaterTop << ".00\n";
		}
	}
}

void MineEscape::calculateTNT(const Tile& temp, priority_queue<Tile, vector<Tile>, tileGreater>& pq)
{
	priority_queue<Tile, vector<Tile>, tileGreater> tntPQ;
	//Set the tile to the top of the tnt pq
	initalizeTNTPQ(temp, tntPQ);

	while (tntPQ.size() > 0)
	{
		Tile* tempTile;
		//Initalize temp tile
		tempTile = &mine[tntPQ.top().row][tntPQ.top().col];
		//When the tile is -1, print out the tnt values
		if (tempTile->tntValue > -2 && tempTile->tntValue < 0)
		{
			if (isVerbose)
				cout << "TNT explosion at [" << tempTile->row << ',' << tempTile->col << "]!\n";

			//"Blow up" the tile and update the tnt variables to mark it as seen
			updateTNTTracker(tempTile, pq, tntPQ);
			TNTPq(tempTile, tntPQ);
		}
		else
		{
			//If the tlie is not tnt, print out the tiles cleared
			if (tempTile->tntValue > 0)
			{
				//Print verbose flag stuff
				if (isVerbose == true)
				{
					cout << "Cleared by TNT: " << tntPQ.top().origVal
						<< " at [" << tntPQ.top().row << ',' << tntPQ.top().col << "]\n";
				}
				//Print median flag stuff
				if (isMedian == true)
				{
					caclulateMedian(&mine[tempTile->row][tempTile->col]);
					printSummary(3);
				}
				//Mark the tile as seen and update the other tracker variables for non tnt tiles
				updateNonTNTTracker(tempTile, pq);
			}
			tntPQ.pop();
		}
	}
}

void MineEscape::initalizeTNTPQ(const Tile& initalTile, priority_queue<Tile, vector<Tile>, tileGreater>& tntPQ)
{
	tntPQ.push(mine[initalTile.row][initalTile.col]);

}

void MineEscape::updateTNTTracker(Tile* tile, priority_queue<Tile, vector<Tile>, tileGreater>& pq, priority_queue<Tile, vector<Tile>, tileGreater>& tntPQ)
{
	//Reset the tnt to 0 after its been exploded and push it to the main pq
	(*tile).tntValue = 0;
	rubblePQ.push_back(mine[(*tile).row][(*tile).col]);

	pq.push(mine[(*tile).row][(*tile).col]);
	(*tile).isScene = true;

	tntPQ.pop();
}

void MineEscape::updateNonTNTTracker(Tile* tile, priority_queue<Tile, vector<Tile>, tileGreater>& pq)
{
	//Reset the tile tntValue, mark it as isScene, and then push it into the cleared pq and median pq
	(*tile).tntValue = 0;
	(*tile).isScene = true;
	rubblePQ.push_back(mine[(*tile).row][(*tile).col]);
	pq.push(mine[(*tile).row][(*tile).col]);
}

void MineEscape::TNTPq(Tile* temp, priority_queue<Tile, vector<Tile>, tileGreater>& TNTExplosionPQ)
{
	//Check boundary cases before adding tile to TNT priority queue
	if ((*temp).row != 0)
	{
		if (!mine[(*temp).row - 1][(*temp).col].tntValue == 0)
		{
			TNTExplosionPQ.push(mine[(*temp).row - 1][(*temp).col]);
		}
	}

	//If the tile is not the edge and the tile right is not tnt, push it to the tntExplosionPQ
	if ((*temp).col != (mapSize - 1))
	{
		if (!mine[(*temp).row][(*temp).col + 1].tntValue == 0)
		{
			TNTExplosionPQ.push(mine[(*temp).row][(*temp).col + 1]);
		}
	}

	//If the tile is not the edge and the tile left is not tnt, push it to the tntExplosionPQ
	if ((*temp).col != 0)
	{
		if (!mine[(*temp).row][(*temp).col - 1].tntValue == 0)
		{
			TNTExplosionPQ.push(mine[(*temp).row][(*temp).col - 1]);
		}
	}

	//If the tile is not the edge and the tile above is not tnt, push it to the tntExplosionPQ
	if ((*temp).row != (mapSize - 1))
	{
		if (!mine[(*temp).row + 1][(*temp).col].tntValue == 0)
		{
			TNTExplosionPQ.push(mine[(*temp).row + 1][(*temp).col]);
		}
	}
}

bool MineEscape::compare(Tile& lhs, Tile& rhs)
 {
	//Basic compare function to break ties
	if (lhs.origVal == rhs.origVal)
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
		if (lhs.origVal < rhs.origVal)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


void MineEscape::caclulateMedian(Tile* val)
{
	if ((*val).origVal >= pqGreater.top().origVal)
	{
		pqGreater.push(*val);
	}
	else
	{
		pqLess.push(*val);
	}

	if (pqGreater.size() - pqLess.size() == 2)
	{
		pqLess.push(pqGreater.top());
		pqGreater.pop();
	}
	else if (pqLess.size() - pqGreater.size() == 2)
	{
		pqGreater.push(pqLess.top());
		pqLess.pop();
	}
}

int main(int argc, char* argv[])
{
	ios_base::sync_with_stdio(false);
	cout << fixed << setprecision(2);

	MineEscape start;

	start.getOpts(argc, argv);
	start.readMap();
	start.backtrack();
	start.printSummary(1);

	if (start.isSummarization == true)
	{
		start.printSummary(2);
	}
}