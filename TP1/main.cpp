#include "ImageBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h> 
#include <numeric>
#include <vector>
using namespace std;

#define PEAK 255
#define NB_GRAYSCALES 256

void permutation(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	ImageBase imPermute;
	imIn.Permutation(&imPermute, _seed);

	imPermute.save(_pathOut);

	cout<<"PSNR : "<<imIn.PSNR(imPermute, PEAK)<<endl;
	cout<<"Entropie originale : "<<imIn.Entropie(NB_GRAYSCALES)<<endl;
	cout<<"Entropie chiffrée : "<<imPermute.Entropie(NB_GRAYSCALES)<<endl;
}

void permutation_inv(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	ImageBase imPermute_inv;
	imIn.Permutation_inverse(&imPermute_inv, _seed);

	imPermute_inv.save(_pathOut);
}

void substition(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	ImageBase imSubstitute;
	imIn.Substition(&imSubstitute, _seed);

	imSubstitute.save(_pathOut);

	cout<<"PSNR : "<<imIn.PSNR(imSubstitute, PEAK)<<endl;
	cout<<"Entropie originale : "<<imIn.Entropie(NB_GRAYSCALES)<<endl;
	cout<<"Entropie chiffrée : "<<imSubstitute.Entropie(NB_GRAYSCALES)<<endl;
}

void substition_inv(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	ImageBase imSubstitute_inv;
	imIn.Substition_inverse(&imSubstitute_inv, _seed);

	imSubstitute_inv.save(_pathOut);
}

void bruteforced(char** argv)
{
	char _pathIn[250], _pathOut[250];
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);

	ImageBase imIn;
	imIn.load(_pathIn);

	ImageBase imSubstitute_inv_bruteforced;
	unsigned int _optimalSeed;
	imIn.Bruteforce_Substition_inverse(&imSubstitute_inv_bruteforced, &_optimalSeed, 0, 256, NB_GRAYSCALES);

	string _pathWithSeed = string(_pathOut)+"_seed_"+to_string(_optimalSeed)+".pgm";
	imSubstitute_inv_bruteforced.save(_pathWithSeed.c_str());
}

void histogram(char** argv)
{
	char _pathIn[250], _pathOut[250];
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);

	ImageBase imIn;
	imIn.load(_pathIn);
	int _histogram[NB_GRAYSCALES];

	imIn.Histogram(_pathOut, _histogram, NB_GRAYSCALES);
}

int main(int argc, char **argv)
{
	//histogram(argv);
	//permutation(argv);
	//permutation_inv(argv);
	substition(argv);
	//substition_inv(argv);
	//bruteforced(argv);
	return 0;
}
