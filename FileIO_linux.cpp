// FileIO.cpp : Defines the entry point for the console application.
//

#include <iostream>  // basic C++ input/output (e.g., cin, cout)
#include <fstream>	// needed to open files in C++
#include <sstream>   // needed if you are using sstream (C++)
#include <stdio.h>	// needed to open files in C
#include <string.h>

using namespace std;

// returns the next character in file without actually reading it (i.e., going past it)
int fpeek(FILE *stream);
int parseFileCFormat(char *fName);
int parseFileCppFormat(char *fName);

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "I need one parameter, which is the file name." << endl;
		return -1;
	}
	parseFileCFormat(argv[1]);
	parseFileCppFormat(argv[1]);
	return 0;
}

// Kia got this function from stackoverflow https://stackoverflow.com/questions/2082743/c-equivalent-to-fstreams-peek
int fpeek(FILE *stream)
{
	int c;

	c = fgetc(stream);
	ungetc(c, stream);

	return c;
}

int parseFileCFormat(char *fName)
{
	FILE *fp;
	char lineBuf[1024];
	char *cellName = NULL;
	char *nextToken = NULL;
	cout << "Parsing input file " << fName << " using C syntax." << endl;
	
	// open the file
	fp = fopen(fName, "r");
	if (fp == NULL) {
		cout << "Error opening file " << fName << endl;
		return -1;
	}

	// look for the keyword "cell"
	while (!feof(fp)) {
		fgets(lineBuf, 1023, fp); // get one whole line
		char *bufPtr = strstr(lineBuf, "cell"); // can you find "cell" in lineBuf? 
		//   bufPtr will be a char pointer to a char in lineBuf at the start of cell

		if (bufPtr == NULL)
			continue;		// the first word was not "cell". Go on to reading the next line
		// we have found cell.
		cout << "found cell ";

		// continue scanning lineBuf to get to '('
		while (*bufPtr != '(' && *bufPtr != NULL)
			bufPtr++;

		if (*bufPtr == NULL) {
			cout << " ERROR! Found cell, but didn't find '('" << endl;
			return -1;
		}

		// we found '('. Now we are at the start of the cell name
		cellName = strtok(bufPtr, "() ");
		// if we wanted to read another token, we would have used:
		//     cellName2 = strtok(NULL, "() ");
		cout << cellName << endl;

		// if one were to extract numbers, then they could use atoi, or sscanf(cellName, "%f", &fNumber), etc.
	}

	fclose(fp);
	return 0;
}

int parseFileCppFormat(char *fName)
{
	char lineBuf[1024];
	cout << "Parsing input file " << fName << " using C++ syntax." << endl;
	ifstream ifs(fName);
	if (ifs.is_open() == 0){ // or we could say if (!ifs)
		cout << "Error opening file " << fName << endl;
		return -1;
	}

	while (ifs.good()) {
		ifs.getline(lineBuf, 1023);	// read one line
		string lineStr(lineBuf); // convert to C++ string
		if (lineStr.empty())	// is empty line?
			continue;

		istringstream iss(lineStr); 

		string firstWord;
		iss >> firstWord;
		if (firstWord.find("cell") != string::npos) { // found the word cell
			cout << "Found cell ";
			char c;
			iss >> c; // read '('
			if (c != '(') {
				continue; // this is not an error. Happens for example when we have cell_delay(
			}

			string cellName;
			iss >> cellName;
			cout << cellName << endl;

			// want to get rid of the ')' at the end of the cell name?
			// You can get rid of the last character, but a cooler and probably much more useful technique is to
			// define your delimiter to be space and ')'.
			// This would be useful when later we want to get rid of " and , as well to get the 7x7 delay table.
			// See this post and the solution https://stackoverflow.com/questions/7302996/changing-the-delimiter-for-cin-c
		}
	}


	return 0;
}

