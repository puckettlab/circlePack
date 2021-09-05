import numpy as np
from scipy.sparse.csgraph import connected_components
from scipy import stats

def getXYRZfmatrix(filenameFull):
    data        = np.load(filenameFull)
    x           = data['x']
    y           = data['y']
    r           = data['r']
    Z           = data['Z']       #
    stress      = data['stress']             #[Nsteps, Nparticles, 2x2]
    fmatrix     = data['fmatrix']           #[Nsteps, NparticlesxNparticles]
    strain      = data['strain']           #[Nsteps, NparticlesxNparticles]
    return x,y,r,Z,stress,fmatrix,strain

#### each frame? or for all steps
def find_Z(fmatrix):
    threshold = 0
    N = dstar.shape[0]                      # stores size of a row of dstar.
    Z = np.zeros(N)                         # creates storage array Z and initializes with N zeros.
    Z   = np.sum( fmatrix>threshold, axis=1)# fmatrix is symmetric [ ];
    for i in range(N):                      # loops through each row in dstar matrix.
        isTouchingParticleI = fmatrix[i] > 0.          # creates boolean array of where all dstar[i] values > 0 are true.
        Z[i] = np.sum( isTouchingParticleI )   #True values = 1, false = 0. The sum is the # of true values.
    return Z

#### each frame?
def calc_Pressures(stress):
    Nsteps  = stress.shape[0]             #(256, 2, 2)
    N       = stress.shape[1]
    press_store = np.zeros((Nsteps,N))    # empty array to store the pressures. Will get returned later.
    shearStress = np.zeros((Nsteps,N))
    for i in range(Nsteps):              # loops through all rows of dstar (1 row = 1 particle)
        for j in range(N):      #for each particle
            w,v     = np.linalg.eig(stress[i,j])    #w eigenvalues; v eigenvectors
            s1      = np.max(np.abs(w))              #get the largest abs value
            s2      = np.min(np.abs(w))              #get the largest abs value
            w       = [s1,s2]                       #sorted eigenvalues
            press_store[i,j]    = (s1+s2)/2    #https://www.nature.com/articles/nature10667.pdf
            shearStress[i,j]    = (s1-s2)/2       ##sort by magnitude, not max/min
    return press_store,shearStress

def findConnTimeFraction(fmatrix,threshold=0):            #in the library
    N = fmatrix.shape[1]
    conn = []
    for f in fmatrix:
        indA,indB = findConnComponents(f,threshold=0)
        conn.append( indA.shape[0] )
    conn = np.array(conn)
    conn = conn.astype(np.float32)/N
    return conn



def findConnComponents(fmatrix,threshold=0):            #in the library
    AdjacencyMatrix = fmatrix > threshold;              #
    n_components, labels = connected_components(csgraph=AdjacencyMatrix, directed=False, return_labels=True)
    m           = stats.mode(labels, axis=None)
    modeA       = m.mode[0]
    connList    = labels==modeA
    indexsA     = np.array(np.where(labels==modeA))[0]
    indexsB     = np.array(np.where(labels!=modeA))[0]
    return indexsA,indexsB
