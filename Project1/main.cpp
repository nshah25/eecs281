//50EB44D3F029ED934858FFFCEAC3547C68768FC9
#include <iostream>
#include <fstream>
#include<sstream>
#include <string>
#include <vector>
#include <getopt.h>
#include <stack>
#include <queue>
#include <deque>

using namespace std;

class test
{
public:
	//Variables to track position and indicies for Letterman objects
	int currPosition;
	char currLetter;
	string currWord;
	bool isUsed;
	int prevPosition;
	char changeMode;

	//Constructors
	test() : currPosition(-1), currLetter(' '), currWord(""), isUsed(false), prevPosition(-1), changeMode(' ') {}
	test(string word_in) : currPosition(-1), currLetter(' '), currWord(word_in), isUsed(false), prevPosition(-1), changeMode(' ') {}
};
class Letterman
{
public:
	//Function to get command line options
	void getOpts(int argc, char* argv[]);
	//Getter and setter functions
	 void updateStartWord();
	 void updateEndWord();
	 void updateCanSwap();
	 void updateCanInsert();
	 void updateCanDelete();
	 void updateCanChange();
	 void updateOutputMode();
	 void updateRouteType();
	 void getDictTypeandNumWords();
	 void incNumStackorQueue();
	 void incNumModesEntered();
	//Print help message
	void printHelp();
	//Function to read in the dictionary that takes a letterman object, the dictionary type, and dictrionary size
	void readDictionary(vector<test>& dictionary, char& dictType, int& dictSize);
	//Function to check if line has a comment
	bool isComment(const string& word);
	//Functions to check if word exists in dictionary
	bool isStartingWord(const string& word);
	bool isEndingWord(const string& word);
	//Backtracking function
	void backtrack(deque<int>& a, vector<test>& dictionary);
	void writeOutput(vector<test>& dict);
	//Function to swap chars inside a string
	string swapChars(string &a, string &b, int& i);
	int checkIfValidOp(test& dict, const string& currWord, char& outputMode, bool& canSwap, bool& canChange, bool& flag);

	ostringstream os;
	//Tracks if dictionary is complex or simple
	char dictType = '\0';
	//Track the output mode from the command line
	char outputMode = '\0';
	//Tracks number of words in the ditionary
	int numWords = 0;
	//Tracks how many options are in the command line argument
	int numStackorQueue = 0;
	//Track number of swap, change, insert, delete entries in the command line
	int numModesEntered = 0;
	//Track wheter a word is in the dictrionary and if the index has been visited
	int startIndex = -1, endIndex = -1;
	//Tracks the number of words read in
	int numCheckedWords = 0;
	//Bools to track depending on what type of arguments are supplied
	bool startWordGiven = false;
	bool endWordGiven = false;
	bool canSwap = false;
	bool canChange = false;
	bool canInsert = false;
	bool canDelete = false;
	bool outputModeGiven = false;
	//Bool to track routing type, true for stack, false for queue, defaults to stack
	bool routeType = true;
	//Strings to hold the starting and ending word
	string startWord;
	string endWord;

};
 void Letterman::updateStartWord()
{
	startWordGiven = true;
}

 void Letterman::updateEndWord()
{
	endWordGiven = true;
}

 void Letterman::updateCanSwap()
{
	canSwap = true;
}

 void Letterman::updateCanInsert()
{
	canInsert = true;
}

 void Letterman::updateCanDelete()
{
	canDelete = true;
}

 void Letterman::updateCanChange()
{
	canChange = true;
}

 void Letterman::updateOutputMode()
{
	outputModeGiven = true;
}

 void Letterman::updateRouteType()
{
	routeType = false;
}

 void Letterman::getDictTypeandNumWords()
{
	cin >> dictType;
	cin >> numWords;
}

 void Letterman::incNumStackorQueue()
{
	numStackorQueue++;
}

 void Letterman::incNumModesEntered()
{
	numModesEntered++;
}

void Letterman::printHelp()
{
	cout << "How do you even have access to this and not know how it works?\n";
	exit(0);
}

void Letterman::readDictionary(vector<test>& dictionary, char& dictType, int& dictSize)
{
	string word;
	//Simple dictionary handling
	if (dictType == 's' || dictType == 'S')
	{
		// Dictionary is simple so you can reserve exactly the amount of space as the dictionary size and read it in
		dictionary.reserve(dictSize);
		getline(cin, word);
		//When program is able to insert and delete, enter
		if (canInsert == true && canDelete == true)
		{
			//While there are still words in the input, keep pushing words in
			while (getline(cin, word))
			{
				//Discard current line if it is a comment
				if (isComment(word) == true)
				{
					continue;
				}
				//else push back the current word
				dictionary.push_back(test(word));

				//Get starting word index
				if (isStartingWord(word) == true)
				{
					startIndex = int(dictionary.size()) - 1;
				}
				//Get ending word index
				if (isEndingWord(word) == true)
				{
					endIndex = int(dictionary.size()) - 1;
				}

			}
		}
		else if (canInsert == false && canDelete == false)
		{
			//While there are still words in the input, keep pushing words in
			while (getline(cin, word))
			{
				//Discard current line if it is a comment
				if (isComment(word) == true)
				{
					continue;
				}
				//Only push words that are the same size as the start word because inserting and deleting is not enabled
				if (word.size() == startWord.size())
				{
					dictionary.push_back(test(word));
				}
				//Get starting word index
				if (isStartingWord(word) == true)
				{
					startIndex = int(dictionary.size()) - 1;
				}
				//Get ending word index
				if (isEndingWord(word) == true)
				{
					endIndex = int(dictionary.size()) - 1;
				}
			}
		}
	}
	else
	{
		//Tracks if a word has a special character
		//Reserve 2 times dictionary size(?), call resize after to save memory
		dictionary.reserve(dictSize);
		getline(cin, word);
		while (getline(cin, word))
		{
			bool isFoundFlag = false;
			//Discard current line if it is a comment
			if (isComment(word) == true)
			{
				continue;
			}
			//Loop to find special character
			for (int i = 0; i < int(word.size()); i++)
			{
				//reverse case
				if (word[i] == '&')
				{
					isFoundFlag = true;
					//Delete ampersand and push into dictionary
					word.erase(i, 1);
					dictionary.push_back(test(word));

					//Get starting word index
					if (isStartingWord(word) == true)
					{
						startIndex = int(dictionary.size()) - 1;
					}
					//Get ending word index
					if (isEndingWord(word) == true)
					{
						endIndex = int(dictionary.size()) - 1;
					}

					//reverse the word
					reverse(word.begin(), word.end());
					dictionary.push_back(test(word));

					//Get starting word index
					if (isStartingWord(word) == true)
					{
						startIndex = int(dictionary.size()) - 1;
					}
					//Get ending word index
					if (isEndingWord(word) == true)
					{
						endIndex = int(dictionary.size()) - 1;
					}
					break;
				}
				//Insert each case
				else if (word[i] == '[')
				{
					isFoundFlag = true;
					string letters;
					int secondBracket = 0;
					//Loop until second bracket is found
					while (word[secondBracket] != ']')
					{
						secondBracket++;
					}
					//Set letters string equal to the string inside the brackets
					letters = word.substr(i + 1, secondBracket - i - 1);
					word.erase(i, secondBracket - i + 1);
					//Loop through word and add the character in
					for (int j = 0; j < int(letters.size()); j++)
					{
						word.insert(i, 1, letters[j]);
						dictionary.push_back(test(word));

						//Get starting word index
						if (isStartingWord(word) == true)
						{
							startIndex = int(dictionary.size()) - 1;
						}
						//Get ending word index
						if (isEndingWord(word) == true)
						{
							endIndex = int(dictionary.size()) - 1;
						}
						word.erase(i, 1);
					}
					break;
				}
				//Swap case
				else if (word[i] == '!')
				{
					isFoundFlag = true;
					string swapChar;
					word.erase(i, 1);
					string swapped = swapChars(swapChar, word, i);

					//Push back original and swapped word
					dictionary.push_back(test(word));
					dictionary.push_back(test(swapped));

					//Get starting word index
					if (isStartingWord(word) == true)
					{
						startIndex = int(dictionary.size()) - 1;
					}
					//Get ending word index
					if (isEndingWord(word) == true)
					{
						endIndex = int(dictionary.size()) - 1;
					}
					//Get starting word index
					if (isStartingWord(swapped) == true)
					{
						startIndex = int(dictionary.size()) - 1;
					}
					//Get ending word index
					if (isEndingWord(swapped) == true)
					{
						endIndex = int(dictionary.size()) - 1;
					}
					break;
				}
				//Duplicate letter case
				else if (word[i] == '?')
				{
					isFoundFlag = true;
					//erase ? character
					word.erase(i, 1);
					//push word
					dictionary.push_back(test(word));

					//Get starting word index
					if (isStartingWord(word) == true)
					{
						startIndex = int(dictionary.size()) - 1;
					}
					//Get ending word index
					if (isEndingWord(word) == true)
					{
						endIndex = int(dictionary.size()) - 1;
					}
					//Insert duplicate character
					//Might need change to word[i -1]
					word.insert(i, 1, word[i - 1]);
					dictionary.push_back(test(word));

					//Get starting word index
					if (isStartingWord(word) == true)
					{
						startIndex = int(dictionary.size()) - 1;
					}
					//Get ending word index
					if (isEndingWord(word) == true)
					{
						endIndex = int(dictionary.size()) - 1;
					}
					break;
				}
			}
			if (isFoundFlag == false)
			{
				//Regular word, just push it
				dictionary.push_back(test(word));

				//Get starting word index
				if (isStartingWord(word) == true)
				{
					startIndex = int(dictionary.size()) - 1;
				}
				//Get ending word index
				if (isEndingWord(word) == true)
				{
					endIndex = int(dictionary.size()) - 1;
				}
			}
			dictionary.resize(dictionary.size());
		}
	}

	//startWord & endWord error handling
	if (startIndex == -1 || endIndex == -1)
	{
		cerr << "Invalid entry. Start or end word does not exist in dictionary." << " \n";
		exit(1);
	}
	if (canInsert == false && canDelete == false && startWord.size() != endWord.size())
	{
		cerr << "Invalid entry. Start word and end word lengths must match." << "\n";
		exit(1);
	}
}

bool Letterman::isComment(const string& word)
{
	if (word.find("//") != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Letterman::isStartingWord(const string& word)
{
	if (word == startWord)
	{
		return true;
	}
	return false;
}

bool Letterman::isEndingWord(const string& word)
{
	if (word == endWord)
	{
		return true;
	}
	return false;
}

void Letterman::backtrack(deque<int>& track, vector<test>& dictionary)
{
	//Keep track of current Index and how many of words have been checked
	int currIndex = 0;
	//Push the first index and increment the word tracker
	track.push_front(startIndex);
	numCheckedWords++;
	dictionary[startIndex].isUsed = true;

	//While the indicies are still in the deque, keep running
	while (!track.empty())
	{
		//Reset the current index to the front of the deque and then pop it
		currIndex = track.front();
		track.pop_front();
		bool b = false;
		//Loop through the dictionary and find out the optimal path based on the command line flags
		for (int i = 0; i < int(dictionary.size()); i++)
		{
			if (dictionary[i].isUsed == false)
			{
				if (canSwap == true || canChange == true)
				{
					b = false;
					if (checkIfValidOp(dictionary[i], dictionary[currIndex].currWord, outputMode, canSwap, canChange, b) == 1)
					{
						//Update tracker variables
						dictionary[i].isUsed = true;
						dictionary[i].prevPosition = currIndex;
						//Change how the index is pushed based on the routing mode
						if (routeType == true)
						{
							track.push_front(i);
							numCheckedWords++;
						}
						else
						{
							track.push_back(i);
							numCheckedWords++;
						}
						continue;
					}
				}
				b = true;
				if (canDelete == true && canInsert == true)
				{
					if (checkIfValidOp(dictionary[i], dictionary[currIndex].currWord, outputMode, canSwap, canChange, b = true) == 1)
					{
						//Update tracker variables
						dictionary[i].isUsed = true;
						dictionary[i].prevPosition = currIndex;
						//Change how the index is pushed based on the routing mode
						if (routeType == true)
						{
							numCheckedWords++;
							track.push_front(i);
						}
						else
						{
							numCheckedWords++;
							track.push_back(i);
						}
						continue;
					}
				}
			}
			if (dictionary[endIndex].isUsed == true)
			{
				break;
			}
		}
		if (dictionary[endIndex].isUsed == true)
		{
			break;
		}
	}
}

void Letterman::writeOutput(vector<test>& dict)
{
	if (dict[endIndex].isUsed == true)
	{
		deque<int> indices;
		int dequeWords = 0, currWordIndex = endIndex;
		//Continue to push indicies into the deque until the startWord reaches the end
		while (currWordIndex != startIndex)
		{
			indices.push_front(currWordIndex);
			dequeWords++;
			currWordIndex = dict[currWordIndex].prevPosition;
		}

		if (indices.size() > 0)
		{
			string size = dict[indices.back()].currWord;
		}

		//Add the last word
		indices.push_front(currWordIndex);
		dequeWords++;
		//Print the number of words in the morph and the  word
		os << "Words in morph: " << dequeWords << "\n";
		os << dict[indices.front()].currWord << "\n";

		indices.pop_front();
		//While the deque still has elements, print out the word based on the command line flag given

		while (indices.empty() == false)
		{
			if (outputMode == 'M' || outputMode == 'm')
			{
				os << dict[indices.front()].changeMode << "," << dict[indices.front()].currPosition;
				if (dict[indices.front()].currLetter != ' ')
				{
					os << "," << dict[indices.front()].currLetter << "\n";
				}
				else
				{
					os << "\n";
				}
				indices.pop_front();
			}
			else if (outputMode == 'W' || outputMode == 'w')
			{
				os << dict[indices.front()].currWord << "\n";
				indices.pop_front();
			}
		}
	}
	//If the end index was never used, the path doesnt exist
	else
	{
		os << "No solution, " << numCheckedWords << " words discovered.\n";
	}
	cout << os.str();
}

string Letterman::swapChars(string& swapChar, string& word, int& index)
{
	//Manually swap the indicies of word and swapChars
	swapChar = word;
	swapChar[index - 1] = word[index - 2];
	swapChar[index - 2] = word[index - 1];

	return swapChar;
}

int Letterman::checkIfValidOp(test& dict, const string& currWord, char& outputMode, bool& canSwap, bool& canChange, bool& flag)
{
	if (flag == false)
	{
		int diff1 = 0, diff2 = 0, diff3 = 0, counter = 0;
		if (dict.currWord.length() == currWord.length())
		{
			while (counter != int(currWord.size()))
			{
				//Find the index of the differnt character
				if (currWord[counter] != dict.currWord[counter])
				{
					diff1++;
					if (diff1 == 1)
					{
						diff2 = counter;
					}
					else if (diff1 == 2)
					{
						diff3 = counter;
					}
					//If there's more than 2 differences, no point in returing true
					else
					{
						return 0;
					}
				}
				counter++;
			}
			//Update letterman member variables based on number of differences
			if (diff1 == 1)
			{
				if (canChange == true)
				{
					if (outputMode == 'M')
					{
						dict.currPosition = diff2;
						dict.currLetter = dict.currWord[diff2];
						dict.changeMode = 'c';
					}
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				//If swapping is allowed, make sure the other indexes are the same and theres only 1 difference
				if (canSwap == true)
				{
					if (currWord[diff2] == dict.currWord[diff3] && currWord[diff3] == dict.currWord[diff2])
					{
						if (diff3 - diff2 == 1)
						{
							if (outputMode == 'M')
							{
								dict.currPosition = diff2;
								dict.changeMode = 's';
							}
							return 1;
						}
					
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
			}
		}
		else
		{
			return 0;
		}
	}
	if (flag == true)
	{
		//If inserting and deleting is allowed, check for 1 larger and 1 smaller
		if (dict.currWord.size() == currWord.size() - 1)
		{
			int diff4 = 0, counter = 0;
			while (counter != int(currWord.length()))
			{
				//Find the index of the different character
				if (dict.currWord[counter] != currWord[counter])
				{
					diff4 = counter;
					break;
				}
				counter++;
			}
			for (int i = diff4 + 1; i<int(currWord.length()); i++)
			{
				if (dict.currWord[i - 1] != currWord[i])
				{
					return 0;
				}
			}
			if (outputMode == 'M')
			{
				dict.currPosition = diff4;
				dict.changeMode = 'd';
			}
			return 1;
		}

		//check 1 char larger and update variables
		else if (dict.currWord.size() == currWord.size() + 1)
		{
			int diff5 = 0, counter = 0;

			while (counter != int(dict.currWord.length()))
			{
				if (dict.currWord[counter] != currWord[counter])
				{
					diff5 = counter;
					break;
				}
				counter++;
			}

			for (int i = diff5 + 1; i<int(dict.currWord.length()); i++)
			{
				if (dict.currWord[i] != currWord[i - 1])
				{
					return 0;
				}
			}
			if (outputMode == 'M')
			{
				dict.currPosition = diff5;
				dict.currLetter = dict.currWord[diff5];
				dict.changeMode = 'i';
			}
			return 1;
		}
		else
			return 0;
	}
	return 0;
}


void Letterman::getOpts(int argc, char* argv[])
{
	//getopt housekeeping
	int choice = 0;
	int option_index = 0;

	option long_options[] =
	{
	   {"stack", no_argument,        nullptr, 's'},
	   {"queue", no_argument,        nullptr, 'q'},
	   {"change", no_argument,       nullptr, 'c'},
	   {"swap", no_argument,         nullptr, 'p'},
	   {"length", no_argument,       nullptr, 'l'},
	   {"output", required_argument, nullptr, 'o'},
	   {"begin", required_argument,  nullptr, 'b'},
	   {"end", required_argument,    nullptr, 'e'},
	   {"help", no_argument,         nullptr, 'h'},
	   { nullptr, 0,                 nullptr, '\0' }
	};

	while ((choice = getopt_long(argc, argv, "sqcplo:b:e:h", long_options, &option_index)) != -1) {
		switch (choice)
		{
		case 's':
			routeType = true;
			incNumStackorQueue();
			break;

		case 'q':
			updateRouteType();
			incNumStackorQueue();
			break;

		case 'c':
			updateCanChange();
			incNumModesEntered();
			break;

		case 'p':
			updateCanSwap();
			incNumModesEntered();
			break;
		case 'l':
			updateCanInsert();
			updateCanDelete();
			incNumModesEntered();
			break;
		case 'o':
			outputMode = optarg[0];
			updateOutputMode();
			break;
		case 'b':
			//TODO: add check to make sure start word exists in dictionary
			startWord = optarg;
			updateStartWord();
			break;
		case 'e':
			//TODO: add check to make sure end word exists in dictionary
			endWord = optarg;
			updateEndWord();
			break;

		case 'h':
			printHelp();
			break;
		default:
			cerr << "Error: invalid option" << endl;
			exit(0);
		} // switch
	} // while

	if (outputModeGiven == false)
	{
		outputMode = 'W';
	}

	//Check to ensure only 1 stack or queue option is entered
	if (numStackorQueue == 0 || numStackorQueue >= 2)
	{
		cerr << "Incorrect number of command line inputs. \n";
		exit(1);
	}
	//Check to ensure at least 1 change operation is entered
	if (numModesEntered == 0)
	{
		cerr << "Must enter at least 1 operation. \n";
		exit(1);
	}
	//Check to ensure output mode is either a W or M
	if (outputMode != 'W' && outputMode != 'M')
	{
		cerr << "Incorrect output mode entered \n";
		exit(1);
	}
}

int main(int argc, char* argv[])
{
	ios_base::sync_with_stdio(false);

	//Declare a vector of Letterman objects to store words and a deque to help store and track indicies
	vector<test> dict;
	deque<int> track;
	//Declare letterman object and call function to parse command line
	Letterman runProgram;
	runProgram.getOpts(argc, argv);

	//Read in type of dictionary and number of words inside it
	cin >> runProgram.dictType;
	cin >> runProgram.numWords;

	runProgram.readDictionary(dict, runProgram.dictType, runProgram.numWords);
	runProgram.backtrack(track, dict);
	runProgram.writeOutput(dict);

	return 0;
}

