// FileIO.cpp : Defines the entry point for the console application.

//*******    Yuan: The way of running jian0655_pa0.cpp
//*******
//Yuan (Mac): input one command line "g++ jian0655_pa0.cpp -o jian0655_pa0 && ./jian0655_pa0 sample_NLDM_lib"
//*******
//Yuan (Mac): When I ran the jian0655_pa0.cpp, it gives two warning: range-based for loop is a C++11 extension [-Wc++11-extensions] (98:19 & 125:19)
//*******


#include <iostream>  // basic C++ input/output (e.g., cin, cout)
#include <fstream>	// needed to open files in C++
#include <sstream>   // needed if you are using sstream (C++)
#include <stdio.h>	// needed to open files in C
#include <string.h>

using namespace std; 

// returns the next character in file without actually reading it (i.e., going past it)
int fpeek(FILE *stream);
int parseFileCppFormat(char *fName);

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "I need one parameter, which is the file name." << endl;
		return -1;
	}
	parseFileCppFormat(argv[1]);

	return 0;
}

// Kia got this function from stackoverflow https://stackoverflow.com/questions/2082743/c-equivalent-to-fstreams-peek
int fpeek(FILE *stream)
{
	int c;					// Yuan: This function serves to get the next character, but does not move the file location pointer forward.

	c = fgetc(stream);
	ungetc(c, stream);

	return c;
}


int parseFileCppFormat(char *fName)
{
	int count = 0;             		// Yuan: Calculate the number of cell types of gates for outputs

	struct GateEntry {   			// Yuan: For each cell type which includes name and delayTable
		string gateName;      
		string delayTable[7][7]; 	// Yuan: Create 7x7 2-dimensional array
	};

	GateEntry gateEntries[100];  	//Yuan: Create size of 100 array

	
	char lineBuf[1024];
	cout << "Parsing input file " << fName << " using C++ syntax." << endl; 	//Yuan: print statement 'Paring...using...C++'
	ifstream ifs(fName);
	if (ifs.is_open() == 0){ 													// or we could say if (!ifs)
		cout << "Error opening file " << fName << endl;  						//Yuan: if cannot find a file, then return -1 and print 'Error...'
		return -1;
	}

	bool is_delayValue = true;   												// Yuan: which set up to differentiate "values" in between "cell_delay" and "output_slew"

	while (ifs.good()) {                                 						//Yuan: loop for reading line by line
		ifs.getline(lineBuf, 1023);												//Yuan: read one line and save in lineBuf at most 1023 char
		string lineStr(lineBuf); // convert to C++ string
		if (lineStr.empty())	// is empty line?
			continue;            // Yuan: reading the next line

		istringstream iss(lineStr);   											// Yuan: iss for reading the first word

		string firstWord;
		iss >> firstWord; 

		if (firstWord.find("cell") != string::npos) { // found the word "cell"
			char c;
			iss >> c; // read '('
			if (c != '(') {
				iss >> c;
				continue; // this is not an error. Happens for example when we have cell_delay(
			}

			iss >> gateEntries[count].gateName; 								// Yuan: save gateName
																				//Yuan: We don't update count value here untill after reading related cell_delay table
		}

		if (firstWord.find("values") != string::npos){   						// Yuan: found the world "values"
			if (is_delayValue){													// Yuan: If value is cell_delay(Timing_7_7)

				int sp = 0;   													// Yuan: define start point = 0 of lineString
				int ep = 0;   												// Yuan: define end point = 0 of lineString
				int numRecdValue = 0; 										// Yuan: To know whether finish get 7 values in line
					for (char p : lineStr) {
						if (isdigit(p) || p == '.') {						// Yuan: if char p is digital number or ".", only ep += 1
							ep += 1;
						}else if (p == ',' || (p == '"' && numRecdValue == 6))		// Yuan: Two possibilities to make loop stop: 1. find "," 2. find '"' at the end of seven value in a line
						{
							gateEntries[count].delayTable[0][numRecdValue] = lineStr.substr(sp, ep-sp);  	// Yuan: updates gateEntries value
							ep += 1; 																		// Yuan: updates sp and ep
							sp = ep;
							numRecdValue += 1;
							if (numRecdValue == 7){															// Yuan: updates numRecdValue and if we've already found 7 value, break the loop
								break;
							}

						}else{
							sp += 1;
							ep += 1;										// Yuan: If found other char type, update sp and ep to continue
						}
					}
				
				for (int i = 1; i < 7; i++) {    								//Yuan: next six lines read after find "value"

					ifs.getline(lineBuf, 1023);	// read one line   
					string lineStr(lineBuf); // convert to C++ string

					int sp = 0;   												// Yuan: the following code part is the same as before
					int ep = 0;   												// Yuan: the difference is that this part specifies for next six lines read after find "value"
					int numRecdValue = 0; 										
						for (char p : lineStr) {
							if (isdigit(p) || p == '.') {
								ep += 1;
							}else if (p == ',' || (p == '"' && numRecdValue == 6))
							{
								gateEntries[count].delayTable[i][numRecdValue] = lineStr.substr(sp, ep-sp);
								ep += 1;
								sp = ep;
								numRecdValue += 1;
								if (numRecdValue == 7){
									break;
								}

							}else{
								sp += 1;
								ep += 1;
							}
						}
					
				}
				count += 1;   														// Yuan: after store 7X7 delay value, update number of cell type of gate
			}												
			is_delayValue = !is_delayValue;  										// Yuan: Updates is_delayValue
																					// Yuan: If value is output_slew(Timing_7_7), the make is_delayValue == true in order to find next delay value
																					// Yuan: If value is cell_delay(Timing_7_7), the make is_delayValue == false to avoid next output_slew value
		}
	
	}

							// ************* Outputs a txt file *******************
	ofstream ofs("jian0655.txt");

	if (ofs.is_open()) {
		ofs << "It totally has number of " << count << " cell gate types." << endl;
		ofs << endl;
		for (int i = 0; i < count; i++) {

																		// Yuan: a simple way to remove ")"
			string gateName = gateEntries[i].gateName;
			if (!gateName.empty() && gateName.back() == ')') {
				gateName.pop_back();									
			}
			ofs << gateName << endl; 									// Yuan: print gateName

																		// Yuan: print gateDelay table value
			ofs << endl;
			for (int j = 0; j < 7; j++) {
				for (int k = 0; k < 7; k++) {
					if (k==6)
					{
						ofs << gateEntries[i].delayTable[j][k] << ";" << endl;
					}else{
						ofs << gateEntries[i].delayTable[j][k] << ", ";
					}
				}
				ofs << endl;
			}
			ofs << endl;
		}
		ofs.close();
	} else {
		cout << "Error opening output file." << endl;
	}
	return 0;
}