# Python 2/3 compatibility
from __future__ import print_function
import sys
if sys.version_info < (3, 0):
    sys.stdout.write("Sorry, requires Python 3.x, not Python 2.x\n")
    sys.exit(1)
import glob
import numpy as np
import matplotlib
matplotlib.use('Agg')             #matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import matplotlib.cm     as cm
from matplotlib.patches import Circle,CirclePolygon
from matplotlib.collections import PatchCollection
from matplotlib.colors import ListedColormap, LinearSegmentedColormap
import subprocess
import os
######
#
######

def convertCSV2npz(dirS,N,phi,seed):
    runName             = "sim-N=%06d-phi=%07.7f-seed=%05d" % (N,phi,seed)
    dataSaveName        = dirS+"/"+runName+"-data.npz"              #npz save name
    dataSaveNameCSV     = dirS+"/"+runName+".csv"              #npz save name
    A = np.loadtxt(dataSaveNameCSV,delimiter=",") #x,y,r,z,s11,s12,s21,s22
    x = A[:,0]
    y = A[:,1]
    r = A[:,2]
    Z = A[:,3]
    np.savez_compressed(dataSaveName, x=x, y=y, r=r, Z=Z)
######
#
######

def convertCSV2npzBatch(dirS,N,phi,seed,Nseeds):
    for i in range(Nseeds):
        convertCSV2npz(dirS,N,phi,seed+i)

######
#
######
def plotCircle(x,y,r,strain,ax,ccolors,L=1):		#A = (x,y,r)
    Laxis	    = (1.0)*L
    Lsim	    = (0.5)*L
    for i in range(x.shape[0]):
        cy  = y[i]
        cx  = x[i]
        circle = Circle((cx,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
        #middle

    #======
    plt.xlim([-Lsim,Lsim])
    plt.ylim([-Lsim,Lsim])
    ax.set_aspect(1.0)
    plt.tight_layout()
    plt.axis('off')

######
#
######
def plotCirclePBC(x,y,r,strain,ax,ccolors,L=1):		#A = (x,y,r)
    Laxis	    = (1.0)*L
    Lsim	    = (0.5)*L
    for i in range(x.shape[0]):
        cy  = y[i]
        cx  = x[i]
        circle = Circle((cx,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
        #middle
        cx  = x[i]+L
        if cx>L:
            cx = cx-2*L;
        circle = Circle((cx,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
        #======
        cy  = y[i]+L
        cx0  = (x[i]+strain)%(L)
        circle = Circle((cx0,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
        cx  = cx0 + (L)
        circle = Circle((cx,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
        cx  = cx0 - (L)
        circle = Circle((cx,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
        #======
        cy  = y[i]-L
        cx0  = (x[i]-strain)%(L)
        circle = Circle((cx0,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
        cx  = cx0 + (L)
        circle = Circle((cx,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
        cx  = cx0 - (L)
        circle = Circle((cx,cy),r[i], color=ccolors[i], fill=True,ec='none');ax.add_artist(circle)
    #======
    plt.vlines(-Lsim, -Laxis,Laxis, colors='k', linestyles='solid',linewidth=0.5)
    plt.vlines(+Lsim, -Laxis,Laxis, colors='k', linestyles='solid',linewidth=0.5)
    plt.hlines(-Lsim, -Laxis,Laxis, colors='k', linestyles='solid',linewidth=0.5)
    plt.hlines(+Lsim, -Laxis,Laxis, colors='k', linestyles='solid',linewidth=0.5)
    plt.xlim([-Laxis,Laxis])
    plt.ylim([-Laxis,Laxis])
    ax.set_aspect(1.0)
    plt.tight_layout()
    plt.axis('off')

######
#makeVideoNPZ
######
def makeImageNPZ(loadName):
    print(loadName)
    A           = np.load(loadName)
    x           = A['x']
    y           = A['y']
    r           = A['r']
    my_dpi      = 100
    px		    = 800
    N           = x.shape[0]
    cmap        = cm.get_cmap('viridis',N)   #fix N
    ccolors     = cmap(np.linspace(0, 1, N))
    plt.switch_backend('Agg')
    fig         = plt.figure(figsize=(px/my_dpi, px/my_dpi), dpi=my_dpi) #necessary to save to multiple of 4;  mp4
    ax          = plt.gca()
    strain      = 0
    plotCircle(x,y,r,strain,ax,ccolors)    #only center pack
    #plotCirclePBC(x,y,r,strain,ax,ccolors) #show PBC
    plt.draw()
    imgSaveName = loadName[:-4]+"-sim.png"
    plt.savefig(imgSaveName, bbox_inches='tight', dpi=my_dpi)
    ax.clear()

def makeImageNPZBatch(dirS,N,phi,seed,Nseeds):
    for i in range(Nseeds):
        runName             = "sim-N=%06d-phi=%07.7f-seed=%05d" % (N,phi,seed+i)
        dataName            = dirS+"/"+runName+"-data.npz"              #npz save name
        makeImageNPZ(dataName)


class fireParameters:
    Nmin 			= 5;
    finc 			= 1.1;      #increase fire dt
    fdec 			= 0.5;      #decrease fire dt
    f_alpha 		= 0.99;
    alpha_start 	= 0.10;
    dt_max 			= 0.1;
    dt_init 		= 0.01;
    fire_mass 		= 4.0; 		# inertia
    force_tol 		= 1e-8;
    max_relaxation_steps = 2000; # stopping criteria


class simParameters:
    N 				= 64;       # number of particles;N = 512, 1024, 2048, 4096, 8192
    rratio 			= 1.4;      # radius ratio, the large particle is 1.4x larger than small particle.
    phi0			= 0.01;	    # packing fraction
    phi 		    = 0.80; 	# initial packing fraction
    mass 			= 0.0003;   # mass of each particle; 1g/cm^3 = 3.14*(0.5*0.5/4)*(0.25) = 0.049 g; daniels2020
    kN 				= 7000;     # vishay PSM-4; E = 4 MPa # k = E*A/L => kN =  6490 kg/s
    saveFileDir 	= "./data/";
    fileSaveName    = ""
    seed 			= 0;
    Nseeds          = 1;

def writeInputFile(fireP,simP,filename):
    file        = open(filename,"w")
    file.write("#=================================================\n#input parameters\n#=================================================\n")
    file.write("N                     = "+str(simP.N)+'\n')
    file.write("rratio                = "+('%8.8f' % simP.rratio)+'\n')
    file.write("phi0                  = "+('%8.8f' % simP.phi0)+'\n')
    file.write("phi                   = "+('%8.8f' % simP.phi)+'\n')
    file.write("mass                  = "+('%8.8f' % simP.mass)+'\n')
    file.write("kN                    = "+('%d' % simP.kN)+'\n')
    file.write("seed                  = "+('%d' % simP.seed)+'\n')
    file.write("Nseeds                = "+('%d' % simP.Nseeds)+'\n')
    file.write("#=================================================\n#output options\n#=================================================\n")
    file.write("fileSaveName          = "+('%s' % simP.fileSaveName)+'\n')
    file.write("#=================================================\n#fire parameters\n#=================================================\n")
    file.write("Nmin                  = "+('%d' % fireP.Nmin)+'\n')
    file.write("finc                  = "+('%5.5f' % fireP.finc)+'\n')
    file.write("f_alpha               = "+('%5.5f' % fireP.f_alpha)+'\n')
    file.write("alpha_start           = "+('%5.5f' % fireP.alpha_start)+'\n')
    file.write("dt_max                = "+('%5.5f' % fireP.dt_max)+'\n')
    file.write("dt_init               = "+('%5.5f' % fireP.dt_init)+'\n')
    file.write("fire_mass             = "+('%5.5f' % fireP.fire_mass)+'\n')
    file.write("force_tol             = "+('%8.8f' % fireP.force_tol )+'\n')
    file.write("max_relaxation_steps  = "+('%d' % fireP.max_relaxation_steps)+'\n')
    file.write("#=================================================\n#end of file\n#=================================================\n")
    file.close()
