#ifndef HELPER_MATH_H
#define HELPER_MATH_H

#include <iostream>
#include <string>
#include <sys/stat.h> // stat
#include <errno.h>    // errno, ENOENT, EEXIST
#if defined(_WIN32)
#include <direct.h>   // _mkdir
#endif



////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

T getVectorSum(std::vector<T> x)
{
	T sum =0;
	for (size_t i = 0; i < x.size(); i++)
	{
		sum += x[i];
	}
	return sum ;
}

inline void zeroVector(std::vector<T> x)
{
	std::fill(x.begin(), x.end(), 0);
}


inline void zeroVector2d(std::vector< std::vector<T> > x)
{
	for (std::size_t i = 0; i<x.size(); i++)
	{
		std::fill(x[i].begin(), x[i].end(), 0);
	}
}

inline T MIN(T a, T b)
{
	if (a<b)
		return a;
	else
		return b;
}


//T defined from dem.h // float or double
T DEM::uDist01()
{
	std::uniform_real_distribution<T> dist(0.0,1.0);
	return dist(m_gen);
}


//not math, but still helping
bool isDirExist(const std::string& path)
{
#if defined(_WIN32)
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & _S_IFDIR) != 0;
#else
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

bool makePath(const std::string& path)
{
#if defined(_WIN32)
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0777;
    int ret = mkdir(path.c_str(), mode);
#endif
    if (ret == 0)
        return true;

    switch (errno)
    {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            std::size_t pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
#if defined(_WIN32)
        return 0 == _mkdir(path.c_str());
#else
        return 0 == mkdir(path.c_str(), mode);
#endif

    case EEXIST:
        // done!
        return isDirExist(path);

    default:
        return false;
    }
}




#endif

