#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "baseFile.h"

using namespace std;


int main(int argc, char **argv) {

	string ucfFileName;
	string hFileName = "config.h";
	
	
	if(argc >= 2)
		ucfFileName = argv[1];
	else 
	{
		cout << "No .ucf file name was given, please enter the .ucf file name" << endl;
		cin >> ucfFileName;
	}


	ifstream inputFile(ucfFileName);
	ofstream outputFile(hFileName);

	string text;

	//intro
	outputFile << _fileHead;
	//first part of .h
	outputFile << _startOfFile;

	//ucf lines
	while (getline(inputFile, text)) {
		if(text.substr(0,2) == "Ac")
		{
			char address[3];
			char data[3];
			char toWrite[100];

			sscanf(text.c_str(), "Ac %s %s", address, data);
			sprintf(toWrite, _ucfLineFormat.c_str(), address, data);

			// Output the text from the file
			outputFile << toWrite;
		}
	}

	//end of .h
	outputFile << _endOfFile;


	// Close the files
	inputFile.close();
	outputFile.close();

	cout << endl << endl << "Code has ended!" << endl;
}