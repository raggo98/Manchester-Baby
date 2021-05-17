// including c++ libaries
#include <iostream>
#include <string>
#include <stdio.h>
#include<fstream>
#include <sstream>

// Using the standard name space
using namespace std;

// global varaibles for setting the size of memory
const int ACCUM_SIZE = 32;
const int CONTROL_SIZE = 32;
const int CI = 32;
const int PI = 32;
const int STORE_SIZE = 32;

class ManchesterBaby
{
	public:

		string MADM_status;						// Control Information
		int Accumulator[ACCUM_SIZE];			/* arithmetic register - holds results of arithmetic operations */
		int Control[CONTROL_SIZE];				/* program counter - current instruction and present instruction*/
		int Store[STORE_SIZE][STORE_SIZE];		/* main memory */
		int ControlInstruction[CI];				// Ci similar to program counter - contains address (line number) of an instruction in the store
		int PresentInstruction[PI];				// during execution holds present instruction being executed
		string flip_Flop;						// When accumulator is less than zero, skip next step.

		// constructor and deconstructor
		ManchesterBaby();
		~ManchesterBaby();

		// methods to initilising,running and testing the Manchester Baby
		void initilise();
		bool CPU(int clockSignal);
		int convertToDecimal(string binary);
		string convertToBinary(int decimalNumber);

		// loading program into memory
		bool loadBinaryToStore();

		// Fetch execute cycle functions
		void getInstructions();
		int fetch(int address);
		int decode(int address);
		int execute(int address, int decoded);

		// display functions
		void displayStore();
		void displayAccumulator();
		void displayControl();

		// instruction functions
		void JMP(int Line);
		void JRP(int Line);
		void LDN(int Line);
		void STO(int Line);
		void SUB(int line);
		void CMP();
		void STP();
};

ManchesterBaby::ManchesterBaby()
{
	cout << " Manchester Baby Created." << endl;
	MADM_status = "STOPPED";
	flip_Flop="RESET";
}

ManchesterBaby::~ManchesterBaby()
{
	cout << "\n\n Manchester Baby has stopped running." << endl;
}

void ManchesterBaby::initilise()
{
	for(int i =0; i < STORE_SIZE; i++)
	{
		Accumulator[i]=0;
		ControlInstruction[i]=0;
		for(int j =0; j < STORE_SIZE; j++)
		{
			Store[i][j] = 0;
		}
	}
}

void ManchesterBaby::displayStore()
{
	cout << "\n\n" << " Store" << endl;
	cout << "      Operand      " << "   Memory Bits 1-8        " << " Opcode " << "  Memory Bits 9-32 " << endl;
	cout << "    <------------->" << " <--------------------->" << " <------>" << " <--------------------------------------------->" <<  endl;
	cout << "                    1  2  4  8 16 32 64 128          1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16  " << endl;
	cout << "     1  2  4  8 16                          1  2  4  " << endl;

	for(int i = 0; i < 32; ++i)
	{
		if((i)<10)
			cout << " " << (i) << "  ";
		else
			cout << " " << (i) << " ";

		for(int j = 0; j < 32; ++j)
		{
			if(Store[i][j] == 0)
				cout << " . ";
			else if(Store[i][j] == 1)
				cout << " | ";
		}
		cout << endl;
	}
	cout << endl;
}

void ManchesterBaby::displayAccumulator()
{
	cout << "\n\n" << " ACCUMULATOR" << endl;
	cout << "      Operand      " << "   Memory Bits 1-8        " << " Opcode " << "  Memory Bits 9-32 " << endl;
	cout << "    <------------->" << " <--------------------->" << " <------>" << " <--------------------------------------------->" <<  endl;
	cout << "                    1  2  4  8 16 32 64 128          1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16  " << endl;
	cout << "     1  2  4  8 16                          1  2  4  " << endl;
	cout << "    ";

	for(int i = 0; i < 32; ++i)
	{
			if(Accumulator[i] == 0)
				cout << " . ";
			else if(Accumulator[i] == 1)
				cout << " | ";
	}
	cout << endl;
}

void ManchesterBaby::displayControl()
{
	cout << "\n\n" << " CONTROL" << endl;
	cout << "      Operand      " << "   Memory Bits 1-8        " << " Opcode " << "  Memory Bits 9-32 " << endl;
	cout << "    <------------->" << " <--------------------->" << " <------>" << " <--------------------------------------------->" <<  endl;
	cout << "                    1  2  4  8 16 32 64 128          1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16  " << endl;
	cout << "     1  2  4  8 16                          1  2  4  " << endl;

	// current instruction
	cout << " CI ";
	for(int i = 0; i < 32; ++i)
	{
			if(ControlInstruction[i] == 0)
			{
				cout << " . ";
			}
			else if(ControlInstruction[i] == 1)
			{
				cout << " | ";
			}
	}
	cout << endl;

	// Present instruction only displayed when the machine is running
	if(MADM_status=="RUNNING")
	{
		cout << " PI ";
		for(int i = 0; i < 32; ++i)
			{
					if(PresentInstruction[i] == 0)
					{
						cout << " . ";
					}
					else if(PresentInstruction[i] == 1)
					{
						cout << " | ";
					}
			}
	}
}

int main()
{
	cout << "<---------------------------------------------->" << endl;
	cout << ""<<" 	Manchester Baby Simulator" << endl;
	cout << "<---------------------------------------------->" << endl;
	cout << ""<<"  Author   : Kyle Harrison" << endl;
	cout << ""<<"  ID#      : 110009870" << endl;
	cout << ""<<"  Module   : AC21009 - Computer Systems 2a: " << endl;
	cout << ""<<"             Architecture Fundamentals  And Unix" << endl;
	cout << ""<<"  Project  : Assignment 5 - Manchester Baby" << endl;
	cout << ""<<"  Date     : 21/11/2011" << endl;
	cout << "<---------------------------------------------->\n " << endl;

	// Used to display results such as if binary was loaded to memory, which iteration the fetch execute cycle is on and if cycle has been executed
	bool loadedSuccessful;
	int  clockSignal=(0-1);
	bool cycleExecuted;

	// Creates a new isntance of the manchester baby
	ManchesterBaby ManchesterBaby;

	// initilises memory to default
	ManchesterBaby.initilise();

	// loads program into store and displays store
	loadedSuccessful = ManchesterBaby.loadBinaryToStore();
	ManchesterBaby.displayStore();

	// If sucessfull starts the fetch execute cycle, otherwise displays error message
	if(loadedSuccessful==true)
	{
		// sets the machine as running if binary loaded sucessfully
		ManchesterBaby.MADM_status="RUNNING";

		// CPU
		do
		{
			if(ManchesterBaby.flip_Flop=="RESET")
			{
				clockSignal++;
			}
			else if ( ManchesterBaby.flip_Flop=="SET")
			{
				clockSignal++;
				clockSignal++;
				ManchesterBaby.flip_Flop="RESET";
			}

			cout << "\nCPU CYCLE = " << clockSignal << endl;
			cycleExecuted = ManchesterBaby.CPU(clockSignal);

			if(cycleExecuted==0)
			{
				cout << "\n Error in CPU" << endl;
			}

		}while(ManchesterBaby.MADM_status=="RUNNING");

		// Display
		ManchesterBaby.displayStore();
		ManchesterBaby.displayAccumulator();
		ManchesterBaby.displayControl();
	}
	else
	{
		cout << "Failed to load" << endl;
	}
	return 0;
}

bool ManchesterBaby::CPU(int clockSignal)
{
	int address = clockSignal;

	// fetch - Transfering address to memory
	fetch(address);
	// decode
	int decoded = decode(address);
	// execute
	execute(address,decoded);

	// Display at end of cycle
	displayAccumulator();
	displayControl();
	displayStore();

	for(int index=0; index <32; index++)
	{
		ControlInstruction[index] = Store[address][index];
	}
	cin.get();
	return true;
}

int ManchesterBaby::fetch(int address)
{
	cout << "FETCH"<< endl;
	cout << " Fetching Memory Address = " << address << endl;

	// Accessing memory and returning values
	cout << " Present Instruction     = ";
	for(int index=0; index <32; index++)
	{
		PresentInstruction[index]     = Store[address][index];
		cout << PresentInstruction[index];
	}

	return 0;
}

int ManchesterBaby::decode(int address)
{
	cout << "\nDECODE" << endl;
	int decode = 0;
	int decodeValue[3] = {1,2,4};

	for(int index = 0; index <3; index++)
	{
		decode = decode + PresentInstruction[index+13]*decodeValue[index];
		//Control[index]=PresentInstruction[index+13];
	}

	// converts address opcode into function number
	cout << " Decode Instruction Code =  " << decode << endl;

	switch(decode)
	{
		case 0 :
		{
			cout << " Instruction   	         =  JMP" << endl;
			break;
		}
		case 1 :
		{
			cout << " Instruction   	         =  JRP" << endl;
			break;
		}
		case 2 :
		{
			cout << " Instruction   	         =  LDN" << endl;
			break;
		}
		case 3 :
		{
			cout << " Instruction   	         =  STO" << endl;
			break;
		}
		case 4 :
		{
			cout << " Instruction   	         =  SUB" << endl;
			break;
		}
		case 5 :
		{
			cout << " Instruction   	         =  SUB" << endl;
			break;
		}
		case 6 :
		{
			cout << " Instruction   	         =  CMP" << endl;
			break;
		}
		case 7 :
		{
			cout << " Instruction   	         =  STP" << endl;
			break;
		}
	}

	return decode;
}

int ManchesterBaby::execute(int address, int decoded)
{
	cout << "EXECUTE" << endl;
	int decodeValue[3] = {1,2,4};
	address=0;
	for(int index = 0; index <3; index++)
	{
		address = address + PresentInstruction[index]*decodeValue[index];
	}

	// getting address from present instruction
	address=0;
	int array[5]={1,2,4,8,16};
	for(int index=0; index<5; index++)
	{
		address=address+PresentInstruction[index]*array[index];
	}

	switch(decoded)
	{
		case 0 :
		{
			cout << " Executing JMP" << endl;
			JMP(address);
			break;
		}
		case 1 :
		{
			cout << " Executing JRP" << endl;
			JRP(address);
			break;
		}
		case 2 :
		{
			cout << " Executing LDN" << endl;

			address=0;
			int array[5]={1,2,4,8,16};
			for(int index=0; index<5; index++)
			{
				address=address+PresentInstruction[index]*array[index];
			}
			LDN(address);
			break;
		}
		case 3 :
		{
			cout << " Executing STO" << endl;
			STO(address);
			break;
		}
		case 4 :
		{
			address=0;
			int array[5]={1,2,4,8,16};
			for(int index=0; index<5; index++)
			{
				address=address+PresentInstruction[index]*array[index];
			}
			cout << " Executing SUB" << endl;
			SUB(address);
			break;
		}
		case 5 :
		{
			address=0;
			int array[5]={1,2,4,8,16};
			for(int index=0; index<5; index++)
			{
							address=address+PresentInstruction[index]*array[index];
			}
			cout << " Executing SUB" << endl;
			SUB(address);
			break;
		}
		case 6 :
		{
			cout << " Executing CMP" << endl;
			CMP();
			break;
		}
		case 7 :
		{
			cout << " Executing STP" << endl;
			STP();
			break;
		}
	}
	cout << endl;

	return 0;
}

void ManchesterBaby::getInstructions()
{
	  string Line;
		ifstream infile;
		infile.open ("default.txt");

		getline(infile,Line); // Saves the line in STRING.
		int lineNumber;
		istringstream ( Line ) >> lineNumber;
		cout << "\n Transfering " << lineNumber << " lines to memory." << endl;

		 // loop throgh the lines of the file until you find the target
		int linesLoaded =0;
		 do
		 {
			  string optcode;
			  string operand;
		      getline (infile, Line);
		      cout << Line << endl; // Prints our STRING.

		      optcode = Line.at(3);
		      optcode = optcode + Line.at(4);
		      optcode = optcode + Line.at(5);

		      //   cout << " OPT " << optcode << endl;

		      string binaryConversion;
		      int decimalNumber = 0;

		      if(optcode=="NUM" || optcode=="STP")
		      {
		    	  binaryConversion="00000000";
			  }
		      else
		      {
		    	  operand = Line.at(7);
		    	  operand = operand + Line.at(8);
				  istringstream ( operand ) >> decimalNumber;
				  binaryConversion = convertToBinary(decimalNumber);
		      }

		      if(binaryConversion.at(7)=='1')
		      {
		    	  Store[linesLoaded][0]=1;
		      }
		      else
		      {
		    	  Store[linesLoaded][0]=0;
		      }
		      if(binaryConversion.at(6)=='1')
		      {
		    	  Store[linesLoaded][1]=1;
		      }
		      else
		      {
		    	  Store[linesLoaded][1]=0;
		      }
		      if(binaryConversion.at(5)=='1')
		      {
		    	  Store[linesLoaded][2]=1;
		      }
		      else
		      {
		    	  Store[linesLoaded][2]=0;
		      }
		      if(binaryConversion.at(4)=='1')
		      {
		    	  Store[linesLoaded][3]=1;
		      }
		      else
		      {
		    	  Store[linesLoaded][3]=0;
		      }
		      if(binaryConversion.at(3)=='1')
		      {
		    	  Store[linesLoaded][4]=1;
		      }
		      else
		      {
		    	  Store[linesLoaded][4]=0;
		      }

		      cout << "Lines " << linesLoaded << " loaded." << endl;

		      if(optcode=="JMP")
		      {
		    	  Store[linesLoaded][13]=0;
		    	  Store[linesLoaded][14]=0;
		          Store[linesLoaded][15]=0;
		      }
		      else if(optcode=="JRP")
		      {
		    	  Store[linesLoaded][13]=0;
		    	  Store[linesLoaded][14]=0;
		    	  Store[linesLoaded][15]=1;
		      }
		      else if(optcode=="LDN")
		      {
		    	  Store[linesLoaded][13]=0;
		    	  Store[linesLoaded][14]=1;
		    	  Store[linesLoaded][15]=0;
		      }
		      else if(optcode=="STO")
		      {
		    	  Store[linesLoaded][13]=0;
		    	  Store[linesLoaded][14]=1;
		    	  Store[linesLoaded][15]=1;
		      }
		      else if(optcode=="SUB")
		      {
		    	  Store[linesLoaded][13]=1;
		    	  Store[linesLoaded][14]=0;
		    	  Store[linesLoaded][15]=0;
		      }
		      else if(optcode=="NUM")
		      {
		    	  Store[linesLoaded][13]=1;
		    	  Store[linesLoaded][14]=0;
		    	  Store[linesLoaded][15]=1;
		      }
		      else if(optcode=="CMP")
		      {
		    	  Store[linesLoaded][13]=1;
		    	  Store[linesLoaded][14]=1;
		    	  Store[linesLoaded][15]=0;
		      }
		      else if(optcode=="STP")
		      {
		    	  Store[linesLoaded][13]=1;
		    	  Store[linesLoaded][14]=1;
		    	  Store[linesLoaded][15]=1;
		      }

		       linesLoaded++;
		  }while (!infile.eof() );

		 if(linesLoaded==8)
		 {
			 cout << "Program loaded sucessfully" << endl;
		 }
		infile.close();
}

string ManchesterBaby::convertToBinary(int decimalNumber)
{
	int powerOf2=128;
	string binaryNumber = "";
	int counter =0;

	do
	{
		if(decimalNumber<powerOf2)
		{
			binaryNumber = binaryNumber+"0";
		}
		else
		{
			binaryNumber = binaryNumber+"1";
			decimalNumber=decimalNumber - powerOf2;
		}

		powerOf2 = powerOf2/2;
		counter++;
	}while(counter!=8);

	return binaryNumber;
}

int ManchesterBaby::convertToDecimal(string binary)
{
	int powerOf2=2147483648;
	int number = 0;
	int counter = 31;
	int lineNumber;
	int binaryValue=0;

	do
	{
		lineNumber = binary.at(counter);
		if(lineNumber==48)
		{
			binaryValue= 0;
		}
		else if(lineNumber==49)
		{
			binaryValue=1;
		}

		powerOf2 = powerOf2/2;
		number = number + ( binaryValue * powerOf2);

		counter--;
	}while(counter>-1);

	return number * -1;
}

bool ManchesterBaby::loadBinaryToStore()
{
	  string Line;
		ifstream binaryFile;
		binaryFile.open ("binaryProgram.txt");

		getline(binaryFile,Line); // Saves the line in STRING.
		int lineNumber=0;
		//cout << Line << endl;
		istringstream ( Line ) >> lineNumber;

		cout << "\n Loading Program to Memory"<< endl;

		if(lineNumber==0)
		{
			cout << "\n File 'binaryProgram.txt' does either not exist or cannot be loaded!" << endl;
		}
		else
		{
			cout << " Transferring " << lineNumber << " lines to memory." << endl;
		}

		 // loop throgh the lines of the file until you find the target
		int linesLoaded;
		if(lineNumber==32)
		{
			linesLoaded = 0;
		}
		else
		{
			linesLoaded=0;
		}
		 do
		 {
			 linesLoaded++;
			  string optcode;
			  string operand;
		      getline (binaryFile, Line);

		      cout << " " << Line; // Prints our STRING.

		      for(int index=5; index <=36; index++)
		      {
				  char bit = Line.at(index);
				  if(bit=='1')
				  {
					  Store[linesLoaded][index-5]= 1;
				  }
				  else
				  {
					  Store[linesLoaded][index-5]= 0;
				  }
		      }

		      cout << endl;
		  }while (linesLoaded!=(lineNumber));

		 binaryFile.close();

		 if(linesLoaded==(lineNumber))
		 {
			 cout << "\n Program loaded sucessfully" << endl;
			 return true;
		 }
		 else
		 {
			 cout << "\n Error occured loading file - Line "<<lineNumber << endl;
			 return false;
		 }
		 return false;
}

void ManchesterBaby::JMP(int address)
{
	cout << " JMP ADDRESS = " << (address) << endl;
	// copy contents of the specifed line into the CI
	for(int index = 0; index < 32; index++)
	{
		Control[index]=Store[address-1][index];
	}

	// update the CI with the line just executed
	cout <<" CI = ";
	for(int index = 0; index < 32; index++)
	{
		ControlInstruction[index]=Store[address][index];
		cout << ControlInstruction[index];
	}
}

void ManchesterBaby::JRP(int address)
{
	cout << "\n Accumulator + 1         = ";
				//	int addone;
					int carryone=0;
					int numbers [32];
					int code =-1 ;
						for(int index = 0; index <32; index++)
						{
							// 0 + 1
							if(ControlInstruction[index]==0 && Store[address][index]==1)
							{
								ControlInstruction[index]=1;
							}
							if(ControlInstruction[index]==1 && Store[address][index]==0)
							{
								ControlInstruction[index]=1;
							}
							if(ControlInstruction[index]==0 && Store[address][index]==0)
							{
								ControlInstruction[index]=0;
							}
							if (ControlInstruction[index]==0 && Store[address][index]==0)
							{
								ControlInstruction[index]=0;
							}
							if(ControlInstruction[index]==0 && Store[address][index]==1)
							{
								ControlInstruction[index]=1;
							}
							if(ControlInstruction[index]==1 && Store[address][index]==0)
							{
								ControlInstruction[index]=1;
							}
							if (ControlInstruction[index]==1 && Store[address][index]==1 )
							{
								ControlInstruction[index]=0;
								carryone=1;
							}
							if (ControlInstruction[index]==1 && Store[address][index]==1 && carryone==1)
							{
								ControlInstruction[index]=1;
								carryone=1;
							}
							if (ControlInstruction[index]==1 && Store[address][index]==0 && carryone==1)
							{
								ControlInstruction[index]=1;
								carryone=0;
							}
							if (ControlInstruction[index]==0 && Store[address][index]==1 && carryone==1)
							{
								ControlInstruction[index]=1;
								carryone=0;
							}
							if (ControlInstruction[index]==0 && Store[address][index]==0 && carryone==1)
							{
								ControlInstruction[index]=1;
								carryone=0;
							}
							cout << ControlInstruction[index];
							numbers[index]=code;
						}
}

void ManchesterBaby::LDN(int address)
{
	// copy the content of the specified line, egated, into the accumulator.
	//address=address-1;
	cout << " Store Memory Address = " << address << endl;

	cout << " Accumulator initial    = ";
	string convert ="";
	for(int index = 0; index < 32; index++)
	{
			Accumulator[index]=Store[address][index];
			cout << Accumulator[index];
	}

	cout << "\n Accumulator Flipped    = ";

	for(int index = 0; index < 32; index++)
	{
		// Negating store line to accumulator
		if(Store[address][index]==1)
		{
			Accumulator[index]=0;
		}
		else if(Store[address][index]==0)
		{
			Accumulator[index]=1;
		}
		cout << Accumulator[index];
	}

	int addone =1;
	int carryone=0;

	cout << "\n Accumulator +1         = ";
	for(int index = 0; index <32; index++)
	{

		// 0 + 1
		if(Accumulator[index]==0 && addone==1)
		{
			Accumulator[index]=1;
			addone=0;
		}
		else if(Accumulator[index]==1 && addone==0)
		{
			Accumulator[index]=1;
		}
		else if (Accumulator[index]==0 && addone==0)
		{
			Accumulator[index]=0;
		}
		else if (Accumulator[index]==1 && addone==1)
		{
			Accumulator[index]=0;
			carryone=1;
		}
		else if (Accumulator[index]==1 && addone==1 && carryone==1)
		{
			Accumulator[index]=1;
			carryone=1;
		}
		else if (Accumulator[index]==1 && addone==0 && carryone==1)
		{
			Accumulator[index]=1;
			carryone=0;
		}
		else if (Accumulator[index]==0 && addone==1 && carryone==1)
		{
			Accumulator[index]=1;
			carryone=0;
		}
		cout << Accumulator[index];
	}
}

void ManchesterBaby::STO(int address)
{
	cout << " Store Memory Address = " << address << endl;

	cout << " Memory at Address    = ";

	for(int index = 0; index < 32; index++)
	{
		Store[address][index]=Accumulator[index];
		cout << Store[address][index];
	}
}

void ManchesterBaby::SUB(int address)
{
	// Subtract the content of the specified line from the accumulator

		cout << " Memory Address    = " << address << endl;

		cout << " Accumulator Negated     = ";
		for(int index = 0; index < 32; index++)
		{
				//Accumulator[index]=Store[address][index];
				cout << Accumulator[index];
				// adding 1
		}

		cout << "\n Subtract Memory         = ";

		for(int index = 0; index < 32; index++)
		{
			//Store[StoreLine][index]=Accumulator[index];
			cout << Store[address][index];
		}


		cout << "\n Accumulator + 1         = ";
				int carryone=0;
					for(int index = 0; index <32; index++)
					{
						// 0 + 1
						if(Accumulator[index]==0 && Store[address][index]==1)
						{
							Accumulator[index]=1;
						}
						if(Accumulator[index]==1 && Store[address][index]==0)
						{
							Accumulator[index]=1;
						}
						if(Accumulator[index]==0 && Store[address][index]==0)
						{
							Accumulator[index]=0;
						}
						if (Accumulator[index]==0 && Store[address][index]==0)
						{
							Accumulator[index]=0;
						}
						if(Accumulator[index]==0 && Store[address][index]==1)
						{
							Accumulator[index]=1;
						}
						if(Accumulator[index]==1 && Store[address][index]==0)
						{
							Accumulator[index]=1;
						}
						if (Accumulator[index]==1 && Store[address][index]==1 )
						{
											Accumulator[index]=0;
						}

						if (Accumulator[index]==1 && Store[address][index]==1 && carryone==1)
						{
							Accumulator[index]=1;
						}
						if (Accumulator[index]==1 && Store[address][index]==0 && carryone==1)
						{
							Accumulator[index]=1;
						}
						if (Accumulator[index]==0 && Store[address][index]==1 && carryone==1)
						{
							Accumulator[index]=1;
						}
						if (Accumulator[index]==0 && Store[address][index]==0 && carryone==1)
						{
							Accumulator[index]=1;
						}
						cout << Accumulator[index];
					}
}

void ManchesterBaby::CMP()
{
	// if the accumulator is less than 0 increment the CI.
	int AccValue = -1;

	int AccIndex[8]={1,2,4,8,16,32,64,128};
	for(int index =5; index < 12; index++)
	{
		AccValue = AccValue + (Accumulator[index] * AccIndex[index]);
	}

	int AccIndex2[16]={256,512,1024,2048,4096,8192,16382,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608};
	for(int index =16; index < 31; index++)
	{
		AccValue = AccValue + (Accumulator[index] * AccIndex2[index]);
	}

	if(Accumulator[31]==1 )
	{
		cout << " Negative - Perform flip flop" << endl;
		flip_Flop="SET";
	}

	if(AccValue<0)
	{
		int CI =0;
		for(int index = 0; index < 8; index++)
		{
			CI = CI + (Accumulator[index] * AccIndex[index]);
		}
		// If CI is less than zero skip the next step
		if(CI <0)
		{

			// Increment the CI by i
			int array[5]={1,2,4,8,16};
			for(int index = 0; index < 5; index++)
			{
				CI=CI+(ControlInstruction[index]*array[index]);
			}
			CI++;
			string binaryConverted = convertToBinary(CI);

			for(int index = 0; index < 5; index++)
			{
				ControlInstruction[index] = binaryConverted.at(index);
			}
		}
		else
		{
			cout <<" ACC VAL = " << CI << endl;
		}
	}
}

void ManchesterBaby::STP()
{
	// halt the baby and light the 'stop lamp'
	MADM_status = "STOPPED";
}