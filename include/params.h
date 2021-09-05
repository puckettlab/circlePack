#ifndef PARAMS_H_
#define PARAMS_H_
#include <string>
// define two classes FireParameters and SimParameters; and how to read from a file
//=======================================================================================
// fire parameter class
//=======================================================================================

class FireParameters {
	public:
		int Nmin;
		int max_relaxation_steps; 		// stopping criteria
		float finc;
		float fdec;
		float f_alpha;
		float alpha_start;
		float dt_max;
		float dt_init;
		float fire_mass;
		float force_tol;
		FireParameters();//set to default
		void print();
} ;
//=======================================================================================
// simulation parameter class
//=======================================================================================
class SimParameters {
	public:
		int N;
		float rratio;
		float phi; 		//packing fraction
		float phi0; 	//initial packing fraction; make larger than phi to make system initially unjammed
		int seed;	//seed for our random number generator
		int Nseeds;
		float dt;
		float mass;
		float kN;
		int saveFileInterval;
		std::string fileSaveName;
		std::string dirS;
		SimParameters();
		void print();
} ;
//=======================================================================================
// misc methods for dealing with these classes
//=======================================================================================
bool iequals(const std::string& a, const std::string& b);
void getParamsFromFile(std::string filename,SimParameters &simP,FireParameters &fireP);
#endif
