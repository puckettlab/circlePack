# circlePack

2D circle pack generator

# What is circlePack?

circlePack generates 2D packing of bidisperse granular material at given packing fraction.  The user can set numerous parameters (number of particles, packing fraction, bidisperse particle ratio)

Requirements

# Software

Everything was coded in c++ and Python 3.

Python Packages:
- numpy
- matplotlib

# Using the code

Python is used to generate input file and run the c++ code.  Numerous parameters can be set including:
- simParameters : simulation Parameters.
- fireParameters : fire Parameters.  These set how the c++ code simulation will run to generate the circle packing.

# Parameters - simParameters


- phi         = 0.830  
- N           = 20    
- phii        = 0.8
