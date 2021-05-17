/**
 * Manchester Baby Assembler
 * Reads in a text file containing source code written in baby assembly
 * makes a first pass at the code to add any tokens to the symbol table
 * updates labels in the symbol table with values if they exist
 * stores the 1st pass in a 2 dimensional array
 * during the second pass it links operands to the line numbers for each variable
 * adds the line number onto the start of the binarycode and add all other formatting
 * Writes the binary program to a file that is saved as a .txt file using any name the user desires
 * size is adjustable.
 * @ "Kyle Harrison"
 * @ (28/11/2011)
 */

// Importing libaries
#include <iostream>
#include <string>
#include <stdio.h>
#include<fstream>
#include <sstream>
#include "Assembler.h"
#include <vector>

using namespace std;

// Global variables for accessing file lines, progress bar and default file
int currentLineNumber=0;
int totalLines = 0;
string fileName = "default.txt";
bool displayProgressBar=true;


/**
 * symbolTable
 * stores any symbol from the assembly code as 2 variables
 */
class symbolTable
{
	public:
		string label;
		int lineNumber;

		symbolTable();
		~symbolTable();
};

symbolTable::symbolTable()
{
	//cout << " Symbol Table Created" << endl;
}
symbolTable::~symbolTable()
{
	//cout << " Symbol Table Destroyed." << endl;
}

/**
 * Assembler
 * Creates the assembler and all necessary classes - stores the temporaryMemory before it is written.
 */
class Assembler
{
	public:

		string tempStore[38][32];
		string insertFileName();

		Assembler();
		void getInstructions();
		void writeProgram();
		void importfile(string selection);
		void interpretateLine(string sourceCodeLine);
		void addSymbol(string symbolLabel, int symbolLineNumber);
		void insertInstruction(string label);

		string convertToBinary(int decimalNumber);
		string convertOpcode(string opcode);
		~Assembler();
};

Assembler::Assembler()
{
	//cout << " Assembler Created" << endl;
}

Assembler::~Assembler()
{
	//cout << "\n Assembler Destroyed." << endl;
}

// Symbol vector that is used to create a list of all symbols from the source code.
vector <symbolTable> Symbols;



/**
 * main
 * creates a new instance of assembler
 * imports header from assembler.h
 * calls import file to make the first pass at the source code
 * calls import file again ignoring specific blocks of code to make a 2nd pass
 * displays progress bar whilst running through code.
 * writes to program
 */
int main()
{
	Assembler Assembler;

	// Displaying the header at the top of the program
	header();

	// Making first pass at source code
	Assembler.importfile("1st");

	// Making second pass at source code
	Assembler.importfile("2nd");

	// Writing the program to a text file
	Assembler.writeProgram();
	return 0;
}


/**
 * insertFileName
 * allows the user to enter a file name of any text file to either load into the system or store too.
 */
string Assembler::insertFileName()
{
	string fileName;
	bool fileExtensionValid=false;

	// Getting valid file name with .txt extension
	do
	{
		cout << "\n Insert file name   : ";
		cin >> fileName;

		int fileSize = fileName.size();

		if( ((fileName.at(fileSize-4))=='.') && ((fileName.at(fileSize-3))=='t') && ((fileName.at(fileSize-2))=='x') && ((fileName.at(fileSize-1))=='t'))
		{
			fileExtensionValid=true;
		}
		else
		{
			cout << "\n Error in file name : " << fileName << " \n Extension '.txt' either missing or incorrect." << endl;
		}

	}while(fileExtensionValid!=true);

	return fileName;
}

/**
 * importfile
 * used to read the source code for both first and second passes by using string selection to change blocks of code.
 */
void Assembler::importfile(string selection)
{
	int linesLoaded = 0;
	string readLine;
	string previousLine;
	ifstream programFile;
	bool fileExists=false;
	int counter =0;

	do
	{ifstream ifile(fileName.c_str());
		if(selection=="1st")
		{
			// Getting a new file name
			cout << "\n Enter the name of the file to convert to binary" << endl;
			fileName = insertFileName();
			programFile.open (fileName.c_str());
		}
		else{
			programFile.open (fileName.c_str());
		}

		do{
			counter++;
			if(previousLine==readLine)	{
				readLine="";
			}
			getline(programFile,readLine);
			previousLine = readLine;

			if(selection=="1st"){
				cout << readLine << endl;
			}
		}while(readLine!="");

		programFile.close();
		totalLines = counter-1;

		// If file exits then
		if(ifile)
		{
			fileExists=true;
			programFile.open (fileName.c_str());

			if(selection=="1st"){
				cout << "\n Opening " << fileName << endl;
				cout << "<---------------------------------------------->" << endl;
				cout << "\n Converting " << totalLines << " lines to binary.\n" << endl;
			}

			for(int index=0; index< totalLines; index++){
				// Get line
				getline(programFile,readLine);

				// Increment linesloaded counter to move to next line
				linesLoaded++;

				if(selection=="1st")
				{
					cout <<" Converting line : " << readLine << endl;
					// Call method to deal with line
					interpretateLine(readLine);
				}
				else if (selection=="2nd")
				{
					// get token from line
					string label;
					for(int index =11; index <16; index++)
						{
							if(readLine.at(index)==32 || readLine.at(index)==58 )
							{
								index=15;
							}
							else
							{
								label = label + readLine.at(index);
							}
						}

				for(int i = 0; i <Symbols.size(); i++)
				{
						if(Symbols.at(i).label==label)
						{
							//cout << Symbols.at(i).lineNumber << endl;
							string binarylineno = convertToBinary(Symbols.at(i).lineNumber);
							//	cout << binarylineno << endl;

							for(int j=0; j<5;j++)
							{
								tempStore[j+5][linesLoaded]=binarylineno.at(j);
							}
							i=Symbols.size();
						}
					}
				}
			}

			// checking file has completed reading all lines of code
			if(linesLoaded==totalLines)
			{
				if(selection=="2nd"){
					progress=100;
					if(displayProgressBar==true){
										displayProgress();
					}
					 cout << " <---------------------------------------------->" << endl;
					 cout << "\n Program loaded sucessfully" << endl;
					 cout << "\n Closing " << fileName << "\n" << endl;
				}
			}
			// When file errors occur
			else{
				 cout << "\n Error occured loading file - Line "<< totalLines << endl;
			}
			cout << " <----------------------------------------------> \n" << endl;
			programFile.close();
		}
		// file does not exist
		else{
			cout << "\n " << fileName << " does not exist.\n" << endl;
			cout << "<----------------------------------------------> " << endl;
			fileExists=false;
		}
	}while(fileExists!=true);
}

/**
 * insertInstruction
 * used for inserting stop and start binary lines into the program file.
 */
void Assembler::insertInstruction(string label)
{
	// Inserting Start
	if(label=="START" && currentLineNumber==0)
	{
		string line0 =  "0000:00000000000000000000000000000000";
		for(int i =0; i < 37; i++)
		{
					tempStore[i][currentLineNumber]=line0.at(i);
		}

		currentLineNumber++;
	}

	if(label=="STOP")
	{
		string lineNo;
		stringstream out;
		out << currentLineNumber;
		lineNo = out.str();

		if(currentLineNumber<10)
		{
			string linex =  "000" + lineNo + ":00000000000001110000000000000000";
			for(int i =0; i < 37; i++)
			{
				tempStore[i][currentLineNumber]=linex.at(i);
			}
		}
		else
		{
			string linex =  "00" + lineNo + ":00000000000001110000000000000000";
			for(int i =0; i < 37; i++)
			{
				tempStore[i][currentLineNumber]=linex.at(i);
			}
		}
	}
	if(displayProgressBar==true){
		displayProgress();
	}
}

/**
 * writeProgram
 * writes the entire converted program to a text file of the user choics.
 */
void Assembler::writeProgram()
{
	 ofstream outdata; // outdata is like cin

	 cout << " Enter a filename for the binary output file with the '.txt' extension :" << endl;
	 string newFile = insertFileName();

	 outdata.open(newFile.c_str()); // opens the file

	   if( !outdata ) { // file couldn't be opened
	      cerr << "Error: file could not be opened" << endl;
	   }
	   else
	   {
		   cout <<  totalLines << endl;
		   outdata << totalLines << endl;

		   for(int j = 1; j<=totalLines; j++)
		   {
			  for (int i=0; i<37; i++)
			  {
				  outdata << tempStore[i][j];
				  cout << tempStore[i][j];
			  }
			  outdata<< endl;
			  cout << endl;
		   }
	   }
	   outdata.close();
}

/**
 * interpretateLine
 * reads in a single line to check which portions of the line contain different values
 * e.g. labels, operands, opcodes, variables, values
 */
void Assembler::interpretateLine(string sourceCodeLine)
{
	string label = "";
	string opcode = "";
	string value ="";
	string writeToFile="";
	string lineNo;
	stringstream out;
	string labelval;
	int labelValue =0;

	for(int index =0; index <6; index++)
	{
		if(sourceCodeLine.at(index)==32 || sourceCodeLine.at(index)==58 )
		{
			index=6;
		}
		else
		{
			label = label + sourceCodeLine.at(index);
		}
	}

	// gets instruction based upon label
	insertInstruction(label);

	if(label=="")
	{
		label=" No Label";
	}

	cout << "\n Label = " << label << endl;

	if(label!="START" || label !="STOP")
	{
		for(int index = 0; index < 8; index++)
		{
			value=value + sourceCodeLine.at(index+7);
		}

		istringstream ( value ) >> labelValue;
		cout << " Label Value = " << labelValue << endl;
	}

	// Add symbol to symbol table
	if(label!=" No Label")
	{
		addSymbol(label,currentLineNumber);
	}

	if(label!="STOP")
	{
		for(int index=0; index <3; index++)
		{
			opcode = opcode + sourceCodeLine.at(index+7);
		}

		if(opcode.at(0) < 65)
		{
			opcode=" No opcode";
		}

		cout << " Opcode = " << opcode << endl;

		string convertedOpcode = convertOpcode(opcode);
		cout << " Binary Opcode = " << convertedOpcode << endl;

		if(convertedOpcode=="Null")
		{
			convertedOpcode="000";
		}

		if(displayProgressBar==true){
											displayProgress();
		}

		out << currentLineNumber;
		lineNo = out.str();

		if(labelValue>0)
		{
			labelval = convertToBinary(labelValue);
		}

		if(currentLineNumber<10)
		{
						if(labelValue>0)
						{
							writeToFile = "000" + lineNo + ":" + labelval + "";
							cout << " " << writeToFile << endl;
						}
						else
						{
							writeToFile = "000" + lineNo + ":0000000000000" + convertedOpcode.at(0) + convertedOpcode.at(1)  + convertedOpcode.at(2) + "0000000000000000";
							cout << " " << writeToFile << endl;
						}
				}
				else
				{
					if(labelValue>0)
					{
						writeToFile = "00" + lineNo + ":" + labelval + "";
						cout << " " << writeToFile << endl;
					}
					else
					{
						writeToFile = "00" + lineNo + ":0000000000000" + convertedOpcode.at(0) + convertedOpcode.at(1)  + convertedOpcode.at(2) + "0000000000000000";
						cout << " " << writeToFile << endl;
					}
				}

				for(int i =0; i < 37; i++)
				{
					tempStore[i][currentLineNumber]=writeToFile.at(i);

				}
				if(displayProgressBar==true){
					displayProgress();
				}
	}
	currentLineNumber++;
	cout << endl;
}

/**
 * convertOpcode
 * converts the opcode from charachters to binary value to insert to tempstore
 */
string Assembler::convertOpcode(string opcode)
{
	string convertedOpCode="Null";

	if(opcode=="JMP")
	{
		convertedOpCode="000";
	}else if(opcode=="JRP")
    {
		convertedOpCode="100";
    }else if(opcode=="LDN")
    {
		convertedOpCode="010";
    }else if(opcode=="STO")
    {
    	convertedOpCode="110";
    }else if(opcode=="SUB")
	{
    	convertedOpCode="001";
	}else if(opcode=="NUM")
	{
		convertedOpCode="101";
	}else if(opcode=="CMP")
	{
		convertedOpCode="011";
	}else if(opcode=="STP")
	{
		convertedOpCode="111";
	}

	if(displayProgressBar==true){
		displayProgress();
	}

	return convertedOpCode;
}

/**
 * convertToBinary
 * converts integer number to decimal
 */
string Assembler::convertToBinary(int decimalNumber)
{
	unsigned int powerOf2 = 2147483648;
	string binaryNumber = "";
	string flipbits= "00000000000000000000000000000000";
	int counter =0;

	do
	{
		if(decimalNumber >= powerOf2)
		{
			binaryNumber = binaryNumber+"1";
			decimalNumber=decimalNumber-powerOf2;
		}
		else
		{
			binaryNumber = binaryNumber+"0";
		}

		if(decimalNumber!=0)
		{
			powerOf2 = powerOf2/2;
		}

		counter++;
	}while(counter!=32);

	for(int i = 0; i < 32; i++)	{
		flipbits.at(i) = binaryNumber.at(31-i);
	}

	if(displayProgressBar==true){
			displayProgress();
	}
	return flipbits;
}

/**
 * addSymbol
 * adds a symbol to the vector of symbols to later use to update the list of variables in the program code
 */
void Assembler::addSymbol(string symbolLabel, int symbolLineNumber)
{
	symbolTable symbol;
	symbol.label= symbolLabel;
	symbol.lineNumber = symbolLineNumber;
	int size = Symbols.size();

	Symbols.resize(size+1);
	Symbols.at(size)=symbol;
	cout << " Label Value = " << Symbols.at(size).lineNumber << endl;

	if(displayProgressBar==true){
										displayProgress();
	}
}