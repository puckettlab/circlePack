#include <fstream> 		//file handle
#include <sstream> 		//tokenize
#include <string>
#include <stdio.h>      /* printf */
#include "params.h"

//=======================================================================================
// sim parameter constructor
//=======================================================================================
SimParameters::SimParameters()
{
	//initialize fire parameters
	N 						= 100;            // number of particles
	rratio 				= 1.4;      			// radius ratio, the large particle is 1.4x larger than small particle.
	phi0 		  		= 0.01; 					// initial packing fraction
	phi						= 0.80;	    			// packing fraction
	mass 					= 1.0;            // mass of each particle
	kN 						= 10000; 					// normal spring coefficient
	fileSaveName 	= ""; 						// fileSaveName
	dirS 				  = "data"; 						// saveFileDir
	seed 					= 0;        			// seed for random number generator
	Nseeds 				= 1;
}
//=======================================================================================
// print
//=======================================================================================
void SimParameters::print()
{
	printf("===SIM  PARAMETERS=======================\n");
	printf("N          = %d\n",N);
	printf("mass       = %4.4g\n",mass);
	printf("rratio     = %4.4g\n",rratio);
	printf("phi        = %4.4g\n",phi);
	printf("phi0       = %4.4g\n",phi0);
	printf("kN         = %4.4g\n",kN);
	printf("seed       = %d\n",seed);
	printf("Nseeds     = %d\n",Nseeds);
	printf("dirS            = %s\n",dirS.c_str());
}

//=======================================================================================
// fire constructor
//=======================================================================================
FireParameters::FireParameters()
{
	//initialize fire parameters
	Nmin 			   = 5;
	finc 			   = 1.1;
	fdec 			   = 0.5;
	f_alpha 		 = 0.99;
	alpha_start  = 0.10;
	dt_max 			 = 0.1;              // max allowed time step
	dt_init 		 = 0.01;             // initial time step
	fire_mass 	 = 4.0; 				//inertia
	force_tol 	 = 1e-6;
	max_relaxation_steps  = (int)(200); 		// stopping criteria
}
//=======================================================================================
// print
//=======================================================================================
void FireParameters::print()
{
	printf("===FIRE PARAMETERS=======================\n");
	printf("Nmin       = %d\n",Nmin);
	printf("finc       = %4.4g\n",finc);
	printf("fdec       = %4.4g\n",fdec);
	printf("f_alpha    = %4.4g\n",f_alpha);
	printf("alpha_start= %4.4g\n",alpha_start);
	printf("dt_max     = %4.4g\n",dt_max);
	printf("dt_init    = %4.4g\n",dt_init);
	printf("fire_mass  = %4.4g\n",fire_mass);
	printf("force_tol  = %4.4g\n",force_tol);
	printf("max_relaxation_steps = %d\n",max_relaxation_steps);
}
//=======================================================================================
// methods
//=======================================================================================
bool iequals(const std::string& a, const std::string& b)   // method that passes a and b by reference and const
{
	unsigned int sz = a.size();
	if (b.size() != sz)                                    // a and b must be the same size for this method
		return false;
	for (unsigned int i = 0; i < sz; ++i)
		if (std::tolower(a[i]) != std::tolower(b[i]))        // std::tolower converts characters to lowercase
			return false;                                    // if the lower case versions of a and b are not the same, return
	return true;                                              // false for the method
}

//=======================================================================================
// get parameters from file;  all the work
// file has following format
// N 			= 100
// rratio 		= 1.4
// the = is a delimeter (whitespace is ignored)
// variable name must match
// see input.txt
//=======================================================================================
void getParamsFromFile(std::string filename,SimParameters &simP,FireParameters &fireP)
{
	std::ifstream myfile (filename);
	if (!myfile)                            //exit incase uploading doesnt work.
	{
		perror ("File error: ");
		exit (EXIT_FAILURE);
	}

	std::string str;

	std::string word,tmp,arg;
	while (std::getline(myfile, str))                  // getline gets characters from input stream and converts to str
	{
		if (str.length()<2) 	// not an empty line
			continue;
		if ( (str[0] == '#') | (str[0] == ' ') | (str[0] == '\t') | (str[0] == '\r') | (str[0] == '\n'))	// ignore comments
			continue; 			// ignore these as well
		//
		std::istringstream iss(str);
		iss >> word; 			// get parameter
		iss >> tmp; 			// not used, this is = sign
		iss >> arg;
		if(iss.fail()) {
			iss.clear();
			continue;
		}

		if (arg.length()<1) 		// if no argument // more error checking?
			continue;
		//===========================================================================
		// Simulation Parameters
		//===========================================================================
		// std::atof extracts floats from inside strings and returns them as doubles
		//===========================================================================
		if (iequals(word,"N"))		//
		{
			simP.N =  (int)(std::atof(arg.c_str()));
		}
		if (iequals(word,"seed"))		//
		{
			simP.seed =  (int)(std::atof(arg.c_str()));
		}
		if (iequals(word,"Nseeds"))		//
		{
			simP.Nseeds =  (int)(std::atof(arg.c_str()));
		}
		if (iequals(word,"mass"))	//
		{
			simP.mass = std::atof(arg.c_str());
		}
		if (iequals(word,"rratio"))		//
		{
			simP.rratio = std::atof(arg.c_str());
		}
		if (iequals(word,"phi"))		//
		{
			simP.phi = std::atof(arg.c_str());
		}
		if (iequals(word,"phi0"))		//
		{
			simP.phi0 = std::atof(arg.c_str());
		}
		if (iequals(word,"kN"))//
		{
			simP.kN = std::atof(arg.c_str());
		}
		if (iequals(word,"fileSaveName"))//
		{
			simP.fileSaveName =  arg.c_str();
		}
		if (iequals(word,"dirS"))//
		{
			simP.dirS =  arg.c_str();
		}
		//====================================================
		// Fire Parameters
		//====================================================
		if (iequals(word,"Nmin"))//
		{
			fireP.Nmin = std::atoi(arg.c_str());         // std::atoi interprets byte string as integer to be returned
		}
		if (iequals(word,"max_relaxation_steps"))//
		{
			fireP.max_relaxation_steps = (int)(std::atof(arg.c_str()));
		}
		if (iequals(word,"finc"))//
		{
			fireP.finc = std::atof(arg.c_str());
		}
		if (iequals(word,"fdec"))//
		{
			fireP.fdec = std::atof(arg.c_str());
		}
		if (iequals(word,"f_alpha"))//
		{
			fireP.f_alpha = std::atof(arg.c_str());
		}
		if (iequals(word,"alpha_start"))//
		{
			fireP.alpha_start  = std::atof(arg.c_str());
		}
		if (iequals(word,"dt_max"))//
		{
			fireP.dt_max  = std::atof(arg.c_str());
		}
		if (iequals(word,"dt_init"))//
		{
			fireP.dt_init  = std::atof(arg.c_str());
		}
		if (iequals(word,"fire_mass"))//
		{
			fireP.fire_mass  = std::atof(arg.c_str());
		}
		if (iequals(word,"force_tol"))//
		{
			fireP.force_tol  = std::atof(arg.c_str());
		}
		// end
	}
}
