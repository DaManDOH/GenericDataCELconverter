/*
Program to parse Command Console Generic Data .CEL files

Dan C. Wlodarski

Info: http://www.affymetrix.com/support/developer/powertools/changelog/gcos-agcc/generic.html
*/

#include <fstream>
#include <iostream>
#include <string>

#include "CELUtils.h"

using namespace std;

int main( int argc, char * argv[] ) {
	int retval = 0;

	if (argc < 2) {
		cerr << "Need an intensity file.\n" << endl;
		retval = -1;
	} else {

		int numberOfDataGroups;
		unsigned int dataGroupStartPos;
		string filepath = argv[1]; // A little buffer overflow protection.

		ifstream ccgdCelFile(filepath.c_str(), ios::binary);

		if (ccgdCelFile.fail()) {
			cerr << "Error opening file \"" << filepath << "\".\n" << endl;
			retval = -2;
		} else {

			if (!extractFileHeader(ccgdCelFile, numberOfDataGroups, dataGroupStartPos)) {
				cerr << "Bad file header.\n" << endl;
				retval = -3;
			} else {

				vector<DataHeaderParameter> headerParamVector;
				if (!extractGenericDataHeader(ccgdCelFile, headerParamVector)) {
					cerr << "Bad data header.\n" << endl;
					retval = -4;
				} else {

					cout << "Number of data groups: " << numberOfDataGroups << "\n";
					// Extract data groups
					vector<DataGroup> allGroups;
					allGroups.reserve(numberOfDataGroups);
					for (int i = 0; i < numberOfDataGroups; i++) {
						DataGroup oneGroup;

						oneGroup.setNextDataGroupPos(extractUintFromFile(ccgdCelFile));
						oneGroup.setStartPos(extractUintFromFile(ccgdCelFile));
						int numberOfDataSets = extractIntFromStream(ccgdCelFile);
						extractWstringFromFile(ccgdCelFile, oneGroup.setDataGroupName());

						cout << "Data group #" << dec << i+1 << ": ";
						wcout << oneGroup.getDataGroupName();
						cout << "\n";

						cout << "\tNumber of data sets in group #";
						cout << i+1 << ": " << numberOfDataSets << "\n";

						// Extract data sets in the current group
						oneGroup.setDataSets().reserve(numberOfDataSets);
						for (int j = 0; j < numberOfDataSets; j++) {
							DataSet oneSet;

							oneSet.setFirstDataElementPos(extractUintFromFile(ccgdCelFile));
							oneSet.setNextDataSetPos(extractUintFromFile(ccgdCelFile));
							extractWstringFromFile(ccgdCelFile, oneSet.setDataSetName());

							cout << "\tData set #" << dec << j+1 << " in group #" << i+1 << ": ";
							wcout << oneSet.getDataSetName();
							cout << "\n";

							extractNameTypeValueTrips(ccgdCelFile, oneSet.setHeaderParams());

							extractDataSet(ccgdCelFile, oneSet);
							oneGroup.setDataSets().push_back(oneSet);

						} // end numberOfDataSets for loop

						allGroups.push_back(oneGroup);

					} // end numberOfDataGroups for loop

					cout << "\n\n**Outputting text file version." << endl;
					string targetFilepath = filepath + ".csv";
					ofstream textCelFile(targetFilepath);
					vector<DataGroup>::const_iterator oneGroup = allGroups.cbegin();
					vector<DataGroup>::const_iterator allGroupsEnd = allGroups.cend();
					for (; oneGroup < allGroupsEnd; oneGroup++) {
						textCelFile << *oneGroup;
					}
					textCelFile << flush;
					textCelFile.close();
					cout << "**Done outputting text file version." << endl;
				}
			}

			ccgdCelFile.close();

		}

	}

#ifdef _DEBUG
	cout << "\nAny key to continue." << endl;
	cin.get();
#endif

	return retval;
}
