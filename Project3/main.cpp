// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "silly.h"
#include <iostream>
#include<sstream>
#include <vector>
#include <sstream>
#include <getopt.h>
#include <climits>
#include <algorithm>

using namespace std;

void SQL::getOpts(int argc, char* argv[])
{
	//getopt housekeeping
	int choice = 0;
	int option_index = 0;

	option long_options[] =
	{
	   {"quiet", no_argument,        nullptr, 'q'},
	   {"help", no_argument,         nullptr, 'h'},
	   { nullptr, 0,                 nullptr, '\0' }
	};

	while ((choice = getopt_long(argc, argv, "qh", long_options, &option_index)) != -1) 
	{
		switch (choice)
		{
		case 'q':
			isQuiet = true;
			break;

		case 'h':
			printHelp();
			break;

		default:
			cerr << "Error: invalid option" << endl;
			exit(0);
		} // switch
	} // while
}

void SQL::printHelp()
{
	cout << "Bro really? This the third time, be better.\n";
	exit(1);
}

void SQL::start()
{
	string userInput;

	//Loop until the quit command is given
	while (keepRunning == true)
	{
		cout << "%" << " ";
		cin >> userInput;

		switch (userInput[0])
		{
		case 'C':
			create();
			break;
		case('I'):
			insert();
			break;
		case('D'):
			deleteTable();
			break;
		case('G'):
			generate();
			break;
		case('P'):
			print();
			break;
		case('J'):
			join();
			break;
		case('R'):
			remove();
			break;
		case('Q'):
			quit();
			break;
		case('#'):
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			break;
		default:
			 cout << "Error: unrecognized command\n";
			 getline(cin, userInput);
		}
	}
	cout << "Thanks for being silly!\n";
}

void SQL::create()
{
	string name, colType;
	int col;
	//Read in table name and index
	cin >> name >> col;

	//Error checking, make sure the table being created doesn't already exist
	if (tables.find(name) != tables.end())
	{
		cout << "Error: Cannot create already exisiting table " << name << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}
	else
	{
		//Create a new table
		tables[name] = new Table;
		Table* newTable = tables[name];
		//Create a temp vector to store the column types
		vector<char> storeType;
		string type;
		//Loop through the columns and read in column types
		for (int i = 0; i < col; i++)
		{
			cin >> type;
			//MAY HAVE TO CHANGE TO SWITCH CASE
			storeType.push_back(type[0]);
		}

		cout << "New table " << name << " with column(s)" << " ";

		string colName;
		//Loop through the columns again and set the new index of main table with the values of the read in table
		for (int i = 0; i < col; i++)
		{
			//Read in and print column names
			cin >> colName;
			cout << colName << " ";
			//Set indicies of newTable
			newTable->index[colName] = i;
			newTable->tpye[colName] = storeType[i];
			newTable->cols.push_back(colName);
		}

		cout << "created" << endl;
	}
}

void SQL::insert()
{
	string name, currVal, junk;
	int rows = 0;
	//ignore the junk string and then read in the column name
	cin >> name >> name;

	//Error checking, make sure the table exits
	if (tables.find(name) == tables.end())
	{
		cout << "Error: " << name << " does not name a table in the database" << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	//Read in the number of rows and then the junk string
	cin >> rows >> junk;

	//Create a table pointer a vector to store the new row
	Table* currRow = tables[name];
	vector<TableEntry> newRow;

	//Loop through n rows and then loop through each column and handle each TableEntry case
	for (int i = 0; i < rows; ++i) 
	{
		for (vector<string>::iterator it = currRow->cols.begin(); it != currRow->cols.end(); it++)
		{
			cin >> currVal;

			if (currRow->tpye[*it] == 's')
			{
				newRow.emplace_back(currVal);
			}

			else if (currRow->tpye[*it] == 'd')
			{
				newRow.emplace_back(stod(currVal));
			}

			else if (currRow->tpye[*it] == 'b')
			{
				if (currVal == "true")
				{
					newRow.emplace_back(true);
				}
				else
				{
					newRow.emplace_back(false);
				}
			}
			else if (currRow->tpye[*it] == 'i')
			{
				newRow.emplace_back(stoi(currVal));
			}
		}
		currRow->rows.emplace_back(newRow);
		newRow.clear();
	}

	if (currRow->isCurrIndex == true) 
	{
		currRow->update(currRow->colToBeUpdated);
	}

	cout << "Added " << rows << " rows to " << name << " from position " << currRow->rows.size() - rows << " to " << currRow->rows.size() - 1 << endl;

}

void SQL::deleteTable()
{
	string rowName, colName, deleteFrom;
	char ws;
	int index = 0;

	//Read in table name
	cin >> rowName >> rowName;

	//Error checking, make sure table exists so you can delete it
	if (tables.find(rowName) == tables.end())
	{
		cout << "Error: " << rowName << " does not name a table in the database" << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	//Read in junk string then the column name
	cin >> ws >> ws >> ws >> ws >> ws;
	cin >> colName >> ws >> deleteFrom;

	//Error checking, make sure the given column name exists in tables
	if (tables[rowName]->index.find(colName) == tables[rowName]->index.end())
	{
		cout << "Error: " << colName << " does not name a column in " << rowName << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	//Create a new table pointer track deleted values
	Table* currRow = tables[rowName];
	index = currRow->index[colName];
	vector<vector<TableEntry>>::iterator it;

	//Handle all three cases for delete and call the correct custom struct
	if (ws == '<')
	{
		lessThan less(currRow, index, deleteFrom, colName);
		it = remove_if(currRow->rows.begin(), currRow->rows.end(), less);
	}

	else if (ws == '>')
	{
		greaterThan greater(currRow, index, deleteFrom, colName);
		it = remove_if(currRow->rows.begin(), currRow->rows.end(), greater);
	}

	else if (ws == '=')
	{
		equalTo equal(currRow, index, deleteFrom, colName);
		it = remove_if(currRow->rows.begin(), currRow->rows.end(), equal);
	}

	//Get size before deleting, then delete, then get post size
	size_t preSize = currRow->rows.size();
	currRow->rows.erase(it, currRow->rows.end());
	size_t postSize = currRow->rows.size();

	//Printing
	cout << "Deleted " << (preSize - postSize) << " rows from " << rowName << endl;

	if (currRow->isCurrIndex == true)
	{
		currRow->update(currRow->colToBeUpdated);
	}
}

void SQL::generate()
{

	string rowName, idType, colName;
	cin >> rowName >> rowName >> idType >> colName >> colName >> colName;

	//Error checking, make sure rowName exists
	if (tables.find(rowName) == tables.end())
	{
		cout << "Error: " << rowName << " does not name a table in the database" << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	//Error checking, make sure colName exists
	if (tables[rowName]->index.find(colName) == tables[rowName]->index.end())
	{
		cout << "Error: " << colName << " does not name a column in " << rowName << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	Table* currRow = tables[rowName];
	currRow->colToBeUpdated = colName;

	//Clear hash and bst
	if (currRow->hash.empty() == false)
	{
		currRow->hash.clear();
	}

	if (currRow->binarySearch.empty() == false)
	{
		currRow->binarySearch.clear();
	}

	//Switch on idType, either update bst or the hash
	switch (idType[0])
	{
		case 'b':
		{
			//Loop through data and push back each index of i to update and generate
			for (int i = 0; i < (int)currRow->rows.size(); ++i) 
			{
				currRow->binarySearch[currRow->rows[i][currRow->index[colName]]].push_back(i);
			}
			break;
		}
		case 'h':
		{
			//Loop through data and push back each index of i to update and generate
			for (int i = 0; i < (int)currRow->rows.size(); ++i)
			{
				currRow->hash[currRow->rows[i][currRow->index[colName]]].push_back(i);
			}
			break;
		}
	}

	currRow->isCurrIndex = true;
	cout << "Created " << idType << " index for table " << rowName << " on column " << colName << endl;
}

void SQL::print()
{
	vector<string> toBePrinted;
	string rowName, colName, deleted, colCompare, compareTo;
	int numPrint = 0, rows = 0;
	char onSwitch;

	cin >> rowName >> rowName >> numPrint;

	//Error checking, name sure given row name exists in the tbale
	if (tables.find(rowName) == tables.end())
	{
		cout << "Error: " << rowName << " does not name a table in the database" << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	//Iterate through the table and check that each column exists before deleting
	for (int i = 0; i < numPrint; i++)
	{
		cin >> colName;

		//Error checking
		if (tables[rowName]->index.find(colName) == tables[rowName]->index.end())
		{
			cout << "Error: " << colName << " does not name a column in " << rowName << endl;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			return;
		}
		//If the column exists, push back the name into the vector
		toBePrinted.push_back(colName);
	}

	cin >> deleted;
	Table* currRow = tables[rowName];

	switch (deleted[0])
	{
		//Case when deleting with all
		case 'A':
		{
			//If the quiet flag is not given, print out each column name
			if (isQuiet == false)
			{
				for (int i = 0; i < int(toBePrinted.size()); i++)
				{
					cout << toBePrinted[i] << " ";
				}
				cout << endl;
			}

			//Loop through the current table looking for index to print
			for (int i = 0; i < int(currRow->rows.size()); i++)
			{
				//If isQuiet is false,  print the column names
				if (isQuiet == false)
				{
					//Loop through all columns and print the names
					for (int j = 0; j < numPrint; j++)
					{
						if (currRow->tpye[toBePrinted[j]] == 'b')
						{
							if (currRow->rows[i][currRow->index[toBePrinted[j]]] == true)
							{
								cout << true;
							}
							else
							{
								cout << false;
							}
						}
						//Printing column names
						else
						{
							cout << currRow->rows[i][currRow->index[toBePrinted[j]]];
						}
						cout << " ";
					}
					cout << endl;
				}
				rows++;
			}
			break;
		}

		//Case where printing with where
		case 'W':
		{
			cin >> colCompare;

			//Error checking, make sure column exists in the table
			if (currRow->index.find(colCompare) == currRow->index.end())
			{
				cout << "Error: " << colCompare << " does not name a column in " << rowName << endl;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				return;
			}

			//If the quiet flag is not given, print out each column name
			if (isQuiet == false)
			{
				for (int i = 0; i < int(toBePrinted.size()); i++)
				{
					cout << toBePrinted[i] << " ";
				}
				cout << endl;
			}

			//Read in char holding compare value and what to compare to
			cin >> onSwitch >> compareTo;

			//If the table has the current index, the column that is being updated matches the compared column, and the bst is not empty, print using indices
			if (onSwitch == '<')
			{
				if (currRow->isCurrIndex && (currRow->colToBeUpdated == colCompare) && currRow->binarySearch.empty() == false)
				{
					printLessWithIndex(toBePrinted, colCompare, compareTo, rowName, numPrint, rows);
				}
				//If any of those condtions fail, print without
				else
				{
					printLessWithoutIndex(toBePrinted, colCompare, compareTo, rowName, numPrint, rows);
				}
			}

			//If the table has the current index, the column that is being updated matches the compared column, and the bst is not empty, print using indices
			else if (onSwitch == '>')
			{
				if (currRow->isCurrIndex && (currRow->colToBeUpdated == colCompare) && currRow->binarySearch.empty() == false)
				{
					printGreaterWithIndex(toBePrinted, colCompare, compareTo, rowName, numPrint, rows);
				}
				//If any of those condtions fail, print without
				else
				{
					printGreaterWithoutIndex(toBePrinted, colCompare, compareTo, rowName, numPrint, rows);
				}
			}

			//If the table has the current index, the column that is being updated matches the compared column, and the bst is not empty, print using indices
			else if (onSwitch == '=')
			{
				if (currRow->isCurrIndex && (currRow->colToBeUpdated == colCompare))
				{
					printEqualWithIndex(toBePrinted, colCompare, compareTo, rowName, numPrint, rows);
				}

				//If any of those condtions fail, print without
				else
				{
					printEqualWithoutIndex(toBePrinted, colCompare, compareTo, rowName, numPrint, rows);
				}
			}
			break;
		}
	}

	//Printing
	cout << "Printed " << rows << " matching rows from " << rowName << endl;
}

//Essentially the same 6 functions, just calling a different functor
void SQL::printLessWithoutIndex(vector<string>& toBePrinted, string colCompare, string compareTo, string rowName, int numPrint, int& rows)
{
	Table* currRow = tables[rowName];

	lessThan less(currRow, currRow->index[colCompare], compareTo, colCompare);

	for (auto& it : currRow->rows)
	{
		if (less(it) == true)
		{
			if (isQuiet == false)
			{
				for (int i = 0; i < numPrint; i++)
				{
					if (currRow->tpye[toBePrinted[i]] == 'b')
					{
						if (it[currRow->index[toBePrinted[i]]] == true)
						{
							cout << "true ";
						}
						else
						{
							cout << "false ";
						}
					}
					else
					{
						cout << it[currRow->index[toBePrinted[i]]] << " ";
					}
				}
				cout << endl;
			}
			rows++;
		}
	}
}

void SQL::printLessWithIndex(vector<string>& toBePrinted, string colCompare, string compareTo, string rowName, int numPrint, int& rows)
{
	Table* currRow = tables[rowName];
	//Create an iterator for a map of tableEntries and vectors to use a index for printing
	map<TableEntry, vector<int>>::iterator it;

	//Set the iterator based on the tableEntry
	//int
	if (currRow->tpye[colCompare] == 'i')
	{
		//Convert the string value into an int and set the lower bound of the bst equal to the iterator
		TableEntry convertToInt(stoi(compareTo));
		it = currRow->binarySearch.lower_bound(convertToInt);
	}
	//double
	else if (currRow->tpye[colCompare] == 'd')
	{
		//Convert the string value into a double and set the lower bound of the bst equal to the iterator
		TableEntry convertToDouble(stod(compareTo));
		it = currRow->binarySearch.lower_bound(convertToDouble);
	}
	//boolean
	else if (currRow->tpye[colCompare] == 'b')
	{
		if (compareTo == "true")
		{
			//Initalize a tableEntry object with true and set the lower bound of the bst equal to the iterator
			TableEntry convertToBool(true);
			it = currRow->binarySearch.lower_bound(convertToBool);
		}
		else
		{
			TableEntry convertToBool(false);
			it = currRow->binarySearch.lower_bound(convertToBool);
		}
	}
	//String
	else if (currRow->tpye[colCompare] == 's')
	{
		//Already a string, no need to convert, just call constructor and set iterator
		TableEntry keepAsString(compareTo);
		it = currRow->binarySearch.lower_bound(keepAsString);
	}

	//Loop from the beginning of the bst till it, and print 
	for (auto i = currRow->binarySearch.begin(); i != it; i++)
	{
		for (int j = 0; j < int(i->second.size()); j++)
		{
			//If isQuiet is false, print true/false and the row
			if (isQuiet == false)
			{
				for (int k = 0; k < numPrint; k++)
				{
					//If the map indexed at the vector at k is equal to b print out the indicies
					if (currRow->tpye[toBePrinted[k]] == 'b')
					{
						
						if (currRow->rows[i->second[j]][currRow->index[toBePrinted[k]]] == false)
						{
							cout << "false ";
						}
						else
						{
							cout << "true ";
						}
					}
					else
					{
						cout << currRow->rows[i->second[j]][currRow->index[toBePrinted[k]]] << " ";
					}
				}
				cout << endl;
			}
			rows++;
		}
	}
}

void SQL::printGreaterWithoutIndex(vector<string>& toBePrinted, string colCompare, string compareTo, string rowName, int numPrint, int& rows)
{
	Table* currRow = tables[rowName];

	greaterThan greater(currRow, currRow->index[colCompare], compareTo, colCompare);

	for (auto& it : currRow->rows)
	{
		if (greater(it) == true)
		{
			if (isQuiet == false)
			{
				for (int i = 0; i < numPrint; i++)
				{
					if (currRow->tpye[toBePrinted[i]] == 'b')
					{
						if (it[currRow->index[toBePrinted[i]]] == true)
						{
							cout << "true ";
						}
						else
						{
							cout << "false ";
						}
					}
					else
					{
						cout << it[currRow->index[toBePrinted[i]]] << " ";
					}
				}
				cout << endl;
			}
			rows++;
		}
	}
}

void SQL::printGreaterWithIndex(vector<string>& toBePrinted, string colCompare, string compareTo, string rowName, int numPrint, int& rows)
{
	Table* currRow = tables[rowName];
	//Create an iterator for a map of tableEntries and vectors to use a index for printing
	map<TableEntry, vector<int>>::iterator it;

	//Set the iterator based on the tableEntry
	//int
	if (currRow->tpye[colCompare] == 'i')
	{
		//Convert the string value into an int and set the lower bound of the bst equal to the iterator
		TableEntry convertToInt(stoi(compareTo));
		it = currRow->binarySearch.upper_bound(convertToInt);
	}
	//double
	else if (currRow->tpye[colCompare] == 'd')
	{
		//Convert the string value into a double and set the lower bound of the bst equal to the iterator
		TableEntry convertToDouble(stod(compareTo));
		it = currRow->binarySearch.upper_bound(convertToDouble);
	}
	//boolean
	else if (currRow->tpye[colCompare] == 'b')
	{
		if (compareTo == "true")
		{
			//Initalize a tableEntry object with true and set the lower bound of the bst equal to the iterator
			TableEntry convertToBool(true);
			it = currRow->binarySearch.upper_bound(convertToBool);
		}
		else
		{
			TableEntry convertToBool(false);
			it = currRow->binarySearch.upper_bound(convertToBool);
		}
	}
	//String
	else if (currRow->tpye[colCompare] == 's')
	{
		//Already a string, no need to convert, just call constructor and set iterator
		TableEntry keepAsString(compareTo);
		it = currRow->binarySearch.upper_bound(keepAsString);
	}

	//Loop from the beginning of the bst till it, and print 
	for (auto i = it; i != currRow->binarySearch.end(); i++)
	{
		for (int j = 0; j < int(i->second.size()); j++)
		{
			//If isQuiet is false, print true/false and the row
			if (isQuiet == false)
			{
				for (int k = 0; k < numPrint; k++)
				{
					//If the map indexed at the vector at k is equal to b print out the indicies
					if (currRow->tpye[toBePrinted[k]] == 'b')
					{

						if (currRow->rows[i->second[j]][currRow->index[toBePrinted[k]]] == false)
						{
							cout << "false ";
						}
						else
						{
							cout << "true ";
						}
					}
					else
					{
						cout << currRow->rows[i->second[j]][currRow->index[toBePrinted[k]]] << " ";
					}
				}
				cout << endl;
			}
			rows++;
		}
	}
}

void SQL::printEqualWithoutIndex(vector<string>& toBePrinted, string colCompare, string compareTo, string rowName, int numPrint, int& rows)
{
	Table* currRow = tables[rowName];

	equalTo equal(currRow, currRow->index[colCompare], compareTo, colCompare);

	for (auto& it : currRow->rows)
	{
		if (equal(it) == true)
		{
			if (isQuiet == false)
			{
				for (int i = 0; i < numPrint; i++)
				{
					if (currRow->tpye[toBePrinted[i]] == 'b')
					{
						if (it[currRow->index[toBePrinted[i]]] == true)
						{
							cout << "true ";
						}
						else
						{
							cout << "false ";
						}
					}
					else
					{
						cout << it[currRow->index[toBePrinted[i]]] << " ";
					}
				}
				cout << endl;
			}
			rows++;
		}
	}
}

void SQL::printEqualWithIndex(vector<string>& toBePrinted, string colCompare, string compareTo, string rowName, int numPrint, int& rows)
{
	Table* currRow = tables[rowName];

	vector<int> dummy;
	vector<int>* tempVec = &dummy;
	
	//Set the iterator based on the tableEntry
	//int
	if (currRow->tpye[colCompare] == 'i')
	{
		//Convert the string value into an int and set the lower bound of the bst equal to the iterator
		TableEntry convertToInt(stoi(compareTo));

		if (currRow->binarySearch.empty() == false)
		{
			tempVec = &currRow->binarySearch[convertToInt];
		}
		else
		{
			tempVec = &currRow->hash[convertToInt];
		}
	}
	//double
	else if (currRow->tpye[colCompare] == 'd')
	{
		//Convert the string value into a double and set the lower bound of the bst equal to the iterator
		TableEntry convertToDouble(stod(compareTo));

		if (currRow->binarySearch.empty() == false)
		{
			tempVec = &currRow->binarySearch[convertToDouble];
		}
		else
		{
			tempVec = &currRow->hash[convertToDouble];
		}
	}
	//boolean
	else if (currRow->tpye[colCompare] == 'b')
	{
		if (compareTo == "true")
		{
			//Initalize a tableEntry object with true and set the lower bound of the bst equal to the iterator
			TableEntry convertToBool(true);
			if (currRow->binarySearch.empty() == false)
			{
				tempVec = &currRow->binarySearch[convertToBool];
			}
			else
			{
				tempVec = &currRow->hash[convertToBool];
			}
		}
		else
		{
			TableEntry convertToBool(false);
			if (currRow->binarySearch.empty() == false)
			{
				tempVec = &currRow->binarySearch[convertToBool];
			}
			else
			{
				tempVec = &currRow->hash[convertToBool];
			}
		}
	}
	//String
	else if (currRow->tpye[colCompare] == 's')
	{
		//Already a string, no need to convert, just call constructor and set iterator
		TableEntry keepAsString(compareTo);
		if (currRow->binarySearch.empty() == false)
		{
			tempVec = &currRow->binarySearch[keepAsString];
		}
		else
		{
			tempVec = &currRow->hash[keepAsString];
		}
	}

	//Loop from the beginning of the bst till it, and print 
	for (auto i = 0; i < int(tempVec->size()); i++)
	{
		if (isQuiet == false)
		{
			for (int j = 0; j < numPrint; j++)
			{
				if (currRow->tpye[toBePrinted[j]] == 'b')
				{
					if (currRow->rows[(*tempVec)[i]][currRow->index[toBePrinted[j]]] == false)
					{
						cout << "false ";
					}
					else
					{
						cout << "true ";
					}
				}
				else
				{
					cout << currRow->rows[(*tempVec)[i]][currRow->index[toBePrinted[j]]] << " ";
				}
			}
			cout << endl;
		}
		rows++;
	}
}

void SQL::join()
{
	string rowName1, rowName2, colName1, colName2, colIndex, junk;
	int nInput = 0, joinIndexLocation = 0, colIndex1 = 0, colIndex2 = 0, numRowsPrinted = 0,  numRows = 0;
	vector<int> joinIndex;
	vector<string> toBePrinted;

	cin >> rowName1 >> rowName2 >> rowName2;

	//Error checking, make sure the table names read in exist
	if (tables.find(rowName1) == tables.end())
	{
		cout << "Error: " << rowName1 << " does not name a table in the database" << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}
	if (tables.find(rowName2) == tables.end())
	{
		cout << "Error: " << rowName2 << " does not name a table in the database" << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	cin >> colName1 >> colName1 >> junk >> colName2;

	//Error checking, make sure the column names exist inside the table
	if (tables[rowName1]->index.find(colName1) == tables[rowName1]->index.end())
	{
		cout << "Error: " << colName1 << " does not name a column in " << rowName1 << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}
	if (tables[rowName2]->index.find(colName2) == tables[rowName2]->index.end())
	{
		cout << "Error: " << colName2 << " does not name a column in " << rowName2 << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	//Read in the number of columns to join
	cin >> junk >> junk;
	cin >> nInput;

	for (int i = 0; i < nInput; i++)
	{
		cin >> colIndex >> joinIndexLocation;

		switch (joinIndexLocation)
		{
			case(1):
			{
				//Error checking, make sure column exists in table
				if (tables[rowName1]->index.find(colIndex) == tables[rowName1]->index.end())
				{
					cout << "Error: " << colIndex << " does not name a column in " << rowName1 << endl;
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					return;
				}
				break;
			}
			case(2):
			{
				if (tables[rowName2]->index.find(colIndex) == tables[rowName2]->index.end())
				{
					cout << "Error: " << colIndex << " does not name a column in " << rowName2 << endl;
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					return;
				}
				break;
			}

		}
		//If the respective tables exist, push back the column and index in order to print them out
		toBePrinted.push_back(colIndex);
		joinIndex.push_back(joinIndexLocation);
	}

	//When the quiet flag is not provided, print out the column names
	if (isQuiet == false)
	{
		for (int i = 0; i < int(toBePrinted.size()); i++)
		{
			cout << toBePrinted[i] << " ";
		}
		cout << endl;
	}

	//Initalize table pointer for the passed in tables and set the indicies
	Table* currRow1 = tables[rowName1];
	Table* currRow2 = tables[rowName2];
	colIndex1 = currRow1->index[colName1];
	colIndex2 = currRow2->index[colName2];

	//Create a new hash table for the second table
	unordered_map<TableEntry, vector<int>> hashing;
	//Loop through all entries in second table and update the hash
	for (auto& it : currRow2->rows)
	{
		hashing[it[colIndex2]].push_back(numRows);
		numRows++;
	}

	//Iterate through first table and second table hash, joining elements and printing as necessary
	for (auto& it : currRow1->rows)
	{
		for (auto& it2 : hashing[it[colIndex1]])
		{
			//Print out actual values when quiet flag is not provided
			if (isQuiet == false)
			{
				for (int i = 0; i < nInput; i++)
				{
					if (joinIndex[i] == 1)
					{
						cout << it[currRow1->index[toBePrinted[i]]];
					}
					else if (joinIndex[i] == 2)
					{
						cout << currRow2->rows[it2][currRow2->index[toBePrinted[i]]];
					}
					cout << " ";
				}
				cout << endl;
			}
			numRowsPrinted++;
		}
	}
	cout << "Printed " << numRowsPrinted << " rows from joining " << rowName1 << " to " << rowName2 << endl;
}

void SQL::remove()
{
	string rowName;

	cin >> rowName;

	//Error checking, make sure the item being removed exists in the table
	if (tables.find(rowName) == tables.end())
	{
		cout << "Error: " << rowName << " does not name a table in the database" << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	//Call delete on the provided tables
	delete tables[rowName];
	tables.erase(rowName);

	cout << "Table " << rowName << " deleted" << endl;
}

void SQL::quit()
{
	//Exit the while loop
	keepRunning = false;

	//Deallocate all the table pointers and clear the table
	for (auto& i : tables)
	{
		delete i.second;
	}
	tables.clear();
}

int main(int argc, char* argv[])
{
	ios_base::sync_with_stdio(false);
	cin >> boolalpha;
	cout << boolalpha;

	SQL run;
	run.getOpts(argc, argv);
	run.start();

	return 0;
}