#include <math.h>       /* sqrt , round, floor, ceil, trunc */
#include "atom.h"

ATOM::ATOM()
{
	int DIM=2;
	m 	   = 1.0; 				// default mass
	r      = 1.0;                 // default radius
	x.resize(DIM,0);            //
	v.resize(DIM,0);            //
	f.resize(DIM,0);            //
	fold.resize(DIM,0);         //
};



void ATOM::force_clear()
{
	std::fill(f.begin(), f.end(), 0);
}


void ATOM::velocity_clear()
{
	std::fill(v.begin(), v.end(), 0);
}

T ATOM::getForceMag()
{
	return sqrt(f[0]*f[0]+f[1]*f[1]);
}

T ATOM::getVelocityMag()
{
	return sqrt(v[0]*v[0]+v[1]*v[1]);
}



ATOM::~ATOM() //atom destructor (likely to save memory)
{
};
