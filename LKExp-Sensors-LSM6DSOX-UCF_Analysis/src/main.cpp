/**
 * @file main.cpp
 * @author Maxime Blanc & Samuel Hadjes
 * @brief Small executable to automatically comment .UCF files
 * @date 2020-07-03
 * 
 */
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>

using namespace std;

bool basic_regs = true;

int main(int argc, char **argv) {

	string ucfFileName = "configuration.ucf";
	string outFileName = "commented.txt";
	
	map<string, string> registers;
	ifstream regFile("registers.csv");
	string line;
	int comma;
	string address;
	string name;
	while (getline(regFile,line))
	{
		comma = line.find(',');
		name = line.substr(0, comma);
		address = line.substr((comma+1));
		registers[address] = name;
	}
	regFile.close();

	map<string, string> registersEmbedded;
	ifstream regEmbeddedFile("registers_embedded.csv");
	while (getline(regEmbeddedFile,line))
	{
		comma = line.find(',');
		name = line.substr(0, comma);
		address = line.substr((comma+1));
		registersEmbedded[address] = name;
	}
	regEmbeddedFile.close();

	
	// if(argc >= 2)
	// 	ucfFileName = argv[1];
	// else 
	// {
	// 	cout << "No .ucf file name was given, please enter the .ucf file name" << endl;
	// 	cin >> ucfFileName;
	// }


	ifstream inputFile(ucfFileName);
	ofstream outputFile(outFileName);

	string text;
	string value;
	string comments;
	//ucf lines
	while (getline(inputFile, text)) {
		if(text.substr(0,2) == "Ac")
		{
			address = text.substr(3, 2);
			value = text.substr(6,2);

			if(basic_regs || address == "01")
			{
				if(registers.find(address) != registers.end()){
					comments = "\tWriting to register : ";
					comments += registers[address];
					comments += " the value: ";
					comments += value;
				}
				else
				{
					comments = "\tRegister not known in basic registers";
				}
			}
			else
			{
				if(registersEmbedded.find(address) != registers.end()){
					comments = "\tWriting to register : ";
					comments += registersEmbedded[address];
					comments += " the value: ";
					comments += value;
				}
				else
				{
					comments = "\tRegister not known in embeded registers";
				}
			}
			
			if(address == "01")
			{
				uint8_t val = stoi(value, 0, 16);
				if(val >> 7 == 0x01) basic_regs = false;
				else basic_regs = true; 
			}
			
			outputFile << text << comments << endl;
		}
	}

	// Close the files
	inputFile.close();
	outputFile.close();

	cout << endl << endl << "Code has ended!" << endl;

	getchar();
}