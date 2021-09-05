#include <fstream> 				//writing to file
#include <string>
#include <vector>
#include <stdio.h>      	//printf
#include <stdlib.h>       // system, NULL, EXIT_FAILURE
#include <sys/stat.h> 		//mkdir
#include <math.h>       	//sqrt , round, floor, ceil, trunc
#include <random> 				//random # generator
#include <filesystem> 		//create directory
#include "dem.h"
#include "helperMath.h" 	//zeroing vectors
//=======================================================================================
// main methods
//=======================================================================================
/**
* constructor for the DEM class. intializes the DEM class, setup the simulation.
* creates N particles, set their radii, then runs fire relaxes the system quasi-statically
* @param inFilename parameters input filename.
* @param verbose_ 	boolean how much to write to the terminal when running
* @see DEM()
* @see fire()
* @return 					none
*/
DEM::DEM(std::string inFilename, bool verbose_)
{
	getParamsFromFile(inFilename,simP,fireP);						//get parameters, takes from SimParameters and FireParameters in params.cpp
	verbose 	= verbose_;
	N 				= (size_t)simP.N;
	phi 			= simP.phi;
	rratio 		= simP.rratio;
	seed 			= simP.seed;
	Nseeds    = simP.Nseeds;
	strain    = 0;
	strain_value = 0;
  atom.assign(N,ATOM()); 															//initialize list of atoms/grains
	if (verbose)                            						//verbose was the input parameter, if set to true, we print all the parameters.
	{
		fireP.print();
		simP.print();
	}
}

//=======================================================================================
// methods: work with atoms
//=======================================================================================
void DEM::atoms_setR(float phi, float rratio)         // creates object of class DEM.
{
	T r0 = (T)(sqrt(2*phi/(N*pi*(1+rratio*rratio)))); 	//cast as double/T
	T r1 = (T)(rratio*r0);
	for (std::size_t i = 0; i<N; i++)               		// runs for all 100 particles.
	{                                                  	// calls in atom class from atom.cpp
		if (i%2)                                					//initializes half the particles as radius r0 and half as r1.
			atom[i].r=r0;
		else
			atom[i].r=r1;
	}
}

void DEM::atoms_zero_velocities()         // creates object of class DEM.
{
	for (std::size_t i = 0; i<N; i++)
	{
    atom[i].velocity_clear();
	}
}

void DEM::atoms_zero_forces()
{
	for (std::size_t i = 0; i<N; i++)
	{
    atom[i].force_clear();
	}
}

void DEM::atoms_zero_Z()
{
	for (std::size_t i = 0; i<N; i++)
	{
    atom[i].Z=0;
	}
}

void DEM::atoms_find_massAll()
{
	for (std::size_t i = 0; i<N; i++)               		// runs for all particles.
	{                                                  	//
		//atom[i].m = simP.mass * atom[i].m*atom[i].r * atom[i].r;           //constant density;
		if (i%2)                                					//
			atom[i].m=simP.mass;
		else
			atom[i].m=simP.mass*(simP.rratio*simP.rratio); 	// large particles are more massive
	}
}

//=======================================================================================
// quasi-static methods
//=======================================================================================
void DEM::fire()                                // fire object
{
	atoms_zero_velocities(); 						// clears atom velocities.
	atoms_zero_forces(); 								// clears atom velocities.
	//initialize fire variables
	T dt 				= fireP.dt_init;
	T alpha 		= fireP.alpha_start; 	//
	T Pfire 		= 0.0;
	T max_force 	= 1e8; 					//
	int i_relax 	= 0;
	int Nneg 	 		= 0;
	T fmag,vmag,cx,cy;
	while ( (max_force > fireP.force_tol) & (i_relax < fireP.max_relaxation_steps) )
	{
		get_forces_PBCfire();
		//
		Pfire 			= 0.0;
		max_force 	= 0.0;
		T fxhat  			= 0.0;
		T fyhat 			=	0.0;
		for (std::size_t i = 0; i<N; i++)
		{
			fmag = atom[i].getForceMag(); 		// stores the magnitude of the forces on particle [i]
			if (fmag>max_force)
				max_force=fmag;
			//
			if (fmag>eps)
			{
				fxhat   = atom[i].f[0]/fmag;
				fyhat   = atom[i].f[1]/fmag;
				Pfire   = Pfire + (atom[i].v[0]*atom[i].f[0] + atom[i].v[1]*atom[i].f[1]);  //add rotational??
			}
		}
		if ((Pfire>0) & (Nneg>fireP.Nmin))
		{
			dt 			= std::min( (float)(dt*fireP.finc), (float)(fireP.dt_max));
			alpha 	= alpha*fireP.f_alpha; 			//why never called?
			for (std::size_t i=0; i<N; i++)
			{
				vmag 	= atom[i].getVelocityMag();
				if (vmag>0)
				{
					atom[i].v[0]	= (1.0-alpha)*atom[i].v[0]  + alpha* fxhat  * vmag;
					atom[i].v[1]	= (1.0-alpha)*atom[i].v[1]  + alpha* fyhat  * vmag;
				}
			}
		}
		if (Pfire<=0 )
		{
			dt 			= fireP.fdec 		* dt;
			alpha 	= fireP.alpha_start;
			atoms_zero_velocities();
			Nneg 	= 0;
		}
		Nneg++;
		//fire dynamics // velocity verlet
		//#pragma omp parallel for default(shared) private(cx,cy)
		for (std::size_t i = 0; i<N; i++)
		{
			atom[i].v[0] += atom[i].f[0]*dt/fireP.fire_mass;
			atom[i].v[1] += atom[i].f[1]*dt/fireP.fire_mass;

			atom[i].x[0] += atom[i].v[0]*dt;
			atom[i].x[1] += atom[i].v[1]*dt;
			// lees-edwards PBC
			cy 		= round(atom[i].x[1]);
			cx 		= atom[i].x[0] - cy*strain;
			atom[i].x[0] = cx      - round(atom[i].x[0]); // round function rounds the argument to the nearest integer
			atom[i].x[1] = atom[i].x[1] -  cy;
		}
		i_relax++;
	}
}

//=======================================================================================
//  simple find granular force; harmonic, zero friction
//=======================================================================================
void DEM::get_forces_PBCfire()
{	//http://homepage.univie.ac.at/franz.vesely/simsp/dx/node40.html
	T sigma,nx,ny,cy,dx,dy,dr,fmag,cdelx;
	//zero out force
	atoms_zero_forces();
	atoms_zero_Z();
	strain_value = fmod(strain,1.0); 			//in code pbc_dist_lees_edwards
	//#pragma omp parallel for default(shared) private(sigma,nx,ny,cy,dx,dy,dr,fmag,cdelx)
	for (std::size_t i = 0; i<N; i++)
	{
		for (std::size_t j = 0; j<N; j++)
		{
			if (i!=j)                                     // makes sure for loop does not operate on only one particle
			{
				dx 			= atom[i].x[0] - atom[j].x[0]; // dx is stored and iterated through both for loops
				dy			= atom[i].x[1] - atom[j].x[1]; // same for dy in the [1] index
				//PBC
				cy 			= round(dy);
				cdelx 	= dx    - cy*strain_value;		//lees-edwards, is this correct???
				dx      = cdelx - round(cdelx);
				dy      = dy - cy;                      // why is there a cdelx and not a cdely
				dr   		= sqrt(dx*dx + dy*dy);
				sigma 	= atom[i].r+atom[j].r;
				if (dr < sigma) 						//if overlapping
				{
					nx 		= dx / dr;
					ny 		= dy / dr;
					fmag 	= simP.kN*(sigma - dr); 		// doesn't matter that this is not accurate potential
					//i
					T fxx = fmag*nx;
					T fyy = fmag*ny;
					atom[i].f[0]+=fxx;
					atom[i].f[1]+=fyy;
					atom[i].Z += 1;
				}
			}
		} 		//end of loop j
	} 			// end of for loop i
}
void DEM::setup()
{
	m_gen.seed( seed); 																	//set seed	of random number generator
	//initialize x position
	for (std::size_t i = 0; i<N; i++)               		// runs for all 100 particles.
	{                                                  	// calls in atom class from atom.cpp
		atom[i].x[0] = (uDist01()-0.5);  									// make a random number between -0.5 and 0.5, store as x
		atom[i].x[1] = (uDist01()-0.5); 									//y
	}
	// set packing to initial phi; if phi0<phi, skip this step
	// get initial state; should we make slightly larger, then shrink to get unjammed? closer to expt protocol
	if (simP.phi0>phi)
	{
		atoms_setR(simP.phi0, rratio);											//set initial phi; phi0>=phi
		fire();
	}
	atoms_setR(simP.phi, simP.rratio); 														// set to desired phi
	atoms_find_massAll(); 														  // set mass of each atom
	fire();
}
//=======================================================================================
// parent run command; called from main()
//=======================================================================================
void DEM::run()
{
	Nseeds 	  = simP.Nseeds;
	for (int i=0; i<Nseeds; i++)
	{
		seed =simP.seed+i;
		setup();
		//make savename
		std::string fnameS; 						// file name as string
		char fnameC[100]; 							// file name as char
		sprintf( fnameC,"sim-N=%06d-phi=%07.7f-seed=%05d.csv",(int)(N),phi,seed);
		fnameS = simP.dirS +"/"+ fnameC; 		//  convert the char to a string
		//save configuration
		std::cout<<fnameS<<"\n";
		saveXYR(fnameS); 					// 	save output old; saveXYR(fnameS); 							// 	save strain=0
	}
}

//=======================================================================================
// save data; x,y,r;
//=======================================================================================
void DEM::saveXYR(std::string outputFilename)
{
	std::ofstream myfile;
	myfile.open(outputFilename, std::ios::out );
	for(std::size_t i=0;i<N;i++)
	{
		myfile<<atom[i].x[0]<<","<<atom[i].x[1]<<","<<atom[i].r<<","<<atom[i].Z<<"\n";
	}
	myfile.close();
}

//=======================================================================================
//  destructor
//=======================================================================================
DEM::~DEM(void)
{
	if (verbose == 1)
		std::printf("Done!\n");
}
