#include <stdio.h>      /* printf */
#include <string>
#include "dem.h"
//-------------------------------------------
// print help
//-------------------------------------------
void printHelp(std::string s)
{
	std::printf("Usage:   %s inputFileName.txt\n", s.c_str());
	std::printf("\n");
}
//-------------------------------------------
// main
//-------------------------------------------
int main(int argc, char **argv) // takes two parameters, an integer argc, and a character argv via pointer to pointer.
{
	std::string filename = "input.txt";	 			//which input file to read from
	bool verbose = true;
	if (argc==1)
	{
		printHelp(argv[0]);
		return 0;
	}
	else
	{
		filename = argv[1];
	}
	//-------------------------------------------
	//start simulation
	//-------------------------------------------
	DEM dem(filename, verbose);    //creates DEM objects dem with parameters filename and verbose.
	dem.run();                      // runs the simulation.
	return 0; // Return successfull exit status
}
