#include "ImageBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h> 
#include <numeric>
#include <vector>
#include <random>
using namespace std;

#define PEAK 255
#define NB_GRAYSCALES 256

void PlanBinaire(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _plan;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_plan);

	ImageBase imIn;
	imIn.load(_pathIn);

	ImageBase imOut;
	imOut = imIn;
	unsigned char _randBitMask = 1 << _plan;
	
	for(int i = 0; i<imIn.getWidth(); ++i)
		for(int j = 0; j<imIn.getHeight(); ++j)
		{
			imOut[i][j] &= _randBitMask;
			if(imOut[i][j] != 0) imOut[i][j] = 255;			
		}

	imOut.save(_pathOut);
}

void Insertion(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _plan;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_plan);

	ImageBase imIn;
	imIn.load(_pathIn);

	vector<unsigned char> _message = vector<unsigned char>(imIn.getTotalSize());
	srand(time(NULL));
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_message[i] = rand()%2;	

	ImageBase imOut;
	imOut = imIn;

	unsigned char _randBitMask = (1 << _plan)^255;

	for(int i = 0; i<imIn.getWidth(); ++i)
		for(int j = 0; j<imIn.getHeight(); ++j)
		{
			unsigned char _msgValue = _message[i*imIn.getHeight()+j];
			imOut[i][j] = (imOut[i][j] & _randBitMask) | (_msgValue <<_plan);
		}

	imOut.save(_pathOut);	
}

void PSNR(char** argv)
{
	char _pathIn[250], _pathOut[250];
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);

	ImageBase imIn;
	imIn.load(_pathIn);

	ImageBase imOut;
	imOut.load(_pathOut);
	cout<<imIn.PSNR(imOut, PEAK)<<endl;
}

void Histogram(char** argv)
{
	char _pathIn[250], _pathOut[250];
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);

	ImageBase imIn;
	imIn.load(_pathIn);

	imIn.Histogram(_pathOut, NB_GRAYSCALES);
}

int main(int argc, char **argv)
{
	Histogram(argv);
	return 0;
}
