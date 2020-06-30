/**
 * @file main.cpp
 * @author Maxime BLANC & Samuel HADJES
 * @brief Small program to convert .ucf files in .h files used to configure the LSM6DSOX MLC
 * @date 2020-06-30
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "hFileTemplate.h"

//##################################################################################################
// Defines
//##################################################################################################

//##################################################################################################
// Prototypes
//##################################################################################################

char *getCmdOption(char **begin, char **end, const std::string &option);
bool cmdOptionExists(char **begin, char **end, const std::string &option);
bool fileExists(string fileName);

//##################################################################################################
// Global Variables
//##################################################################################################

// default input file
string _defaultUcfFileName = "mlc_config.ucf";

// default output file
string _defaultHFileName = "mlc_config.h";

//message to display when help parameter is called
string _helpMsg =	"UCF to H file converter help\n"
							"This program takes a .ucf input file and parses it into the corresponding .h file\n"
							"The .h file can then be included in your project\n"
							"\n"
							"Parameters\n"
							"-i <filename>\n"
							"\tinput file\n"
							"\tIf no input file is set, the default one will be used: \"" + _defaultUcfFileName + "\"\n"
							"-o <filename>\n"
							"\toutput file\n"
							"\tIf no output file is set, the default one will be used: \"" + _defaultHFileName + "\"\n"
							"--overwrite\n"
							"\tIf the output file already exists, it will be writen over without a warning\n"
							"-h or --help\n"
							"\tThis help message\n";


//##################################################################################################
// Main
//##################################################################################################
using namespace std;

int main(int argc, char *argv[]) {

	string ucfFileName;
	string hFileName;

	//-------------------------------------------------------------------------------------
	// Parsing command options
	// help
	if (cmdOptionExists(argv, argv + argc, "-h") || cmdOptionExists(argv, argv + argc, "--help")) {
		printf("%s", _helpMsg.c_str());
		return 0;
	}

	// input file
	char *filename = getCmdOption(argv, argv + argc, "-i");
	if (filename)
		ucfFileName = filename;
	else
		ucfFileName = "mlc_config.ucf";
	//printf("Input file : \"%s\"\n", ucfFileName.c_str());


	// output file
	filename = getCmdOption(argv, argv + argc, "-o");
	if (filename)
		hFileName = filename;
	else
		hFileName = "mlc_config.h";
	//printf("Output file : \"%s\"\n", hFileName.c_str());


	//overwrite output
	bool outputFileOverwrite = false;
	if (cmdOptionExists(argv, argv + argc, "--overwrite")) outputFileOverwrite = true;


	//-------------------------------------------------------------------------------------
	// test input file existence
	ifstream inputFile;
	if(fileExists(ucfFileName)) inputFile.open(ucfFileName);
	else
	{
		cout << "Input file: \"" << ucfFileName << "\" does not exist." << endl;
		cout << "Exiting program" << endl;
		cout << "Try parameter -h or --help for more info" << endl;
		return 1;
	}

	//-------------------------------------------------------------------------------------
	// test output file existence
	if(fileExists(hFileName) && !outputFileOverwrite)
	{
		cout << "Output file: \"" << hFileName << "\" already exists." << endl;
		cout << "Call program with \"--overwrite\" parameter to ignore this security." << endl;
		cout << "Exiting program" << endl;
		cout << "Try parameter -h or --help for more info" << endl;
		return 1;
	}
	//if there is no problem or overwrite is set, open output file
	ofstream outputFile(hFileName);


	//-------------------------------------------------------------------------------------
	// File generation and parsing

	// Add the configuration file intro
	outputFile << _fileHead;

	// Add the beginning of the configuration file
	outputFile << _startOfFile;

	// Add the configuration lines (register data writing)
	string text;
	while (getline(inputFile, text)) {
		if (text.substr(0, 2) == "Ac") {
			char address[3];
			char data[3];
			char toWrite[100];

			sscanf(text.c_str(), "Ac %s %s", address, data);
			sprintf(toWrite, _ucfLineFormat.c_str(), address, data);

			// Output the text from the file
			outputFile << toWrite;
		}
	}

	// Add the end of the configuration file
	outputFile << _endOfFile;

	// Close the files
	inputFile.close();
	outputFile.close();

	// End of program
	cout << endl << "The output file \"" << hFileName << "\" has been generated." << endl;
	cout << "End of program" << endl;
}

//##################################################################################################
// Functions
//##################################################################################################

/**
 * @brief Get the option given to a command
 * 
 * @param begin start of the arg list (should be argv)
 * @param end end of the arg list, (should be argv+argc)
 * @param option string corresponding to the option to check
 * @return char* start of the option value
 */
char *getCmdOption(char **begin, char **end, const std::string &option) {
	char **itr = std::find(begin, end, option);
	if (itr != end && ++itr != end) { return *itr; }
	return 0;
}

/**
 * @brief Checks if a command option has been given
 * 
 * @param begin start of the arg list (should be argv)
 * @param end end of the arg list, (should be argv+argc)
 * @param option string corresponding to the option to check
 * @return true option is in args list
 * @return false option isn't in args list
 */
bool cmdOptionExists(char **begin, char **end, const std::string &option) {
	return std::find(begin, end, option) != end;
}

/**
 * @brief Checks if a file with the given filename exists
 * 
 * @param fileName name of the file
 * @return true the file exists
 * @return false the file doesn't exist
 */
bool fileExists(string fileName)
{
    ifstream infile(fileName);
    return infile.good();
}