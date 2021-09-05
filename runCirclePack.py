import subprocess
import numpy as np
from subprocess import call
from pythonInclude import simParameters, fireParameters
from pythonInclude import writeInputFile, convertCSV2npzBatch, makeImageNPZBatch
#=======================
# input parameters
#=======================
phi         = 0.830                     #phi
N           = 20                      #
phii        = 0.8
seed        = 0
Nseeds      = 2
dirS        = "./data/"                 #save directory
fireP       = fireParameters()          # see pythonInclude for full details
simP        = simParameters()           #
simP.N      = N                         # number of particles
simP.phi0   = 0.85                      # initial packing, phi0>phi;  if this is too small, packing can have gaps
simP.phi    = phii      #phiArray[i]                    # packing fraction for simulation
simP.seed   = seed
simP.Nseeds = Nseeds
#=======================
def singleRun():
    #=========
    runName             = "sim-N=%06d-phi=%07.7f-seed=%05d" % (simP.N,simP.phi,simP.seed)
    inputFile           = dirS+"/"+runName+"-input.txt"             #input file save name
    writeInputFile(fireP,simP,inputFile)             #
    paramSaveName       = dirS+"/"+runName+"-params.npz"            #npz save name
    np.savez(paramSaveName,simP=simP,fireP=fireP,runName=runName)
    simP.fileSaveName   = runName                                   #
    #=========
    simCMD              = "./circlePack "+ inputFile                #cmd to start simulation, check path
    p                   = subprocess.Popen(simCMD, shell=True)
    p.wait()
    #=========
    print("Converting csv to npz")
    convertCSV2npzBatch(dirS,simP.N,simP.phi,simP.seed,simP.Nseeds) #convert csv output to npz
    print("Converting npz to png")
    makeImageNPZBatch(dirS,simP.N,simP.phi,simP.seed,simP.Nseeds)                          #make video
#=======================
# main
#=======================
if __name__ == '__main__':
    singleRun()
