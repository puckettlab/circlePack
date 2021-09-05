// Make sure the header is only included once
#ifndef DEM_H_
#define DEM_H_

#include <string>
#include <vector>
#include <random> 						// random # generator
#include "params.h" 					// includes params cpp too. parameter files
#include "type.h" 						// type (double/float) and minor includes
#include "atom.h" 						// atom
#include <fstream>

//=======================================================================================
// DEM class
//=======================================================================================
class DEM {
	private:
		std::vector<ATOM> atom; 			// atoms/grains; and their x,v,F,r,mass
		std::string outputFilename; 	// Output filename (full path)
		int verbose; 									// Output level
		size_t N; 										// Number of particles, type is unsigned integer, for loops
		float phi; 						// packing fraction
		float rratio; 					// ratio of particles
		int seed; 						// random seed
		int Nseeds;
		float strain;
		float strain_value;
		SimParameters  simP; 			// parameters to use for simulation
		FireParameters fireP; 			// parameters to control fire; no need to change these
		// need to use friction
		void  atoms_setR(float phi, float rratio);         // creates object of class DEM.
		// methods; find forces and update
		void 	get_forces_PBCfire(); 	// compute then store, simpler force for fire; good for initial condition, or frictionless particles
		void  printSimParameter();
		//methods to zero out v,f for all atoms/particles
		void 	atoms_zero_velocities();
		void 	atoms_zero_forces();
		void 	atoms_zero_Z();
		void  atoms_find_massAll();
		void  fire(); 							// fire, quasi-static
		void  setup(); 							// initialize w/ seed and random pack
		void  saveXYR(std::string outputFilename);//save data
		//
		std::mt19937 m_gen; 			// random # generator
		T 	uDist01(); 					// uniform random number between 0..1

	public:
		// Constructor, some parameters with default values
		DEM(std::string inFilename, bool verbose);
		void run(); 					// main run
		~DEM(void);        				// Destructor
};

#endif
