# circlePack

2D circle pack generator

# What is circlePack?

circlePack generates 2D packing of bidisperse granular material at given packing fraction.  The user can set numerous parameters (number of particles, packing fraction, bidisperse particle ratio)

Requirements

# Software

Everything was coded in c++ and Python 3.  The following packages are needed only to convert output from c++ code csv files into npz files and generate images of the packing.

Python Packages:
- numpy
- matplotlib

# Using the code

> git clone https://github.com/puckettlab/circlePack.git

> cd circlePack

> cmake CMakeLists.txt

> make

Edit runCirclePack.py to your desired parameters. Run with:
> python runCirclePack.py

# Editing parameters in runCirclePack.py
- a


# Note on Parameters

Python is used to generate input file and run the c++ code.  Numerous parameters can be set including:
- simParameters : simulation Parameters. These setting control features of the packing.  See below for more details.
- fireParameters : fire Parameters.  These set how the c++ code simulation will run to generate the circle packing. These do not in general need to be altered.

# Parameters - simParameters

- N   : number of particles in simulation   
- phi : packing fraction
- phi0: when generating a configuraiton, it may be desirable to require the configuration is initially unjammed.  Set phi0>phi for an initially unjammed pack.  
- rratio: bidispersity ratio (rL/rS)
- kN  : particle stiffness
- seed : seed for generating the packing
- Nseeds: if Nseeds>1, the code will start with 'seed' and iterate through 'Nseeds' configurations
