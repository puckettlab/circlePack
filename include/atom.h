#ifndef ATOM_H_
#define ATOM_H_
#include <vector>
#include "type.h" 					// type (double/float) and minor includes



class ATOM {
	public:
		T m; 							// mass
		T r;                            // radius
		int Z; 													// z number of contacts
		std::vector<T> x;   						// x,y position
		std::vector<T> v;   						// velocity
		std::vector<T> f;   						// force
		std::vector<T> fold;   					// force, last timesetp, md only
		//
		void force_clear();
		void velocity_clear();
		//
		T 	getForceMag();
		T   getVelocityMag();
		//
		ATOM() ;   						// constructor
		~ATOM();
};


#endif
