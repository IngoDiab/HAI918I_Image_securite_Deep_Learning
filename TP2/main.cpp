#include "ImageBase.h"
#include "AES.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h> 
#include <numeric>
#include <vector>
using namespace std;

#define PEAK 255
#define NB_GRAYSCALES 256

AES _aes;

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

void ECB(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(_seed);
	vector<unsigned char> _imKey = vector<unsigned char>(imIn.getTotalSize());
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_imKey[i] = rand()%256;
	unsigned char* _outImage = _aes.EncryptECB(imIn.getData(), imIn.getTotalSize(), _imKey.data());

	ImageBase imOut = ImageBase(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	for(int i = 0; i<imIn.getTotalSize(); ++i)
		imOut[i/imOut.getWidth()][i%imOut.getHeight()] = _outImage[i];
	imOut.save(_pathOut);

	cout<<"PSNR : "<<imIn.PSNR(imOut, PEAK)<<endl;
	cout<<"Entropie originale : "<<imIn.Entropie(NB_GRAYSCALES)<<endl;
	cout<<"Entropie chiffrée : "<<imOut.Entropie(NB_GRAYSCALES)<<endl;
}

void D_ECB(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(_seed);
	vector<unsigned char> _imKey = vector<unsigned char>(imIn.getTotalSize());
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_imKey[i] = rand()%256;
	unsigned char* _outImage = _aes.DecryptECB(imIn.getData(), imIn.getTotalSize(), _imKey.data());

	ImageBase imOut = ImageBase(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	for(int i = 0; i<imIn.getTotalSize(); ++i)
		imOut[i/imOut.getWidth()][i%imOut.getHeight()] = _outImage[i];
	imOut.save(_pathOut);
}

void CBC(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(_seed);
	vector<unsigned char> _imKey = vector<unsigned char>(imIn.getTotalSize());
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_imKey[i] = rand()%256;

	vector<unsigned char> _iv = vector<unsigned char>(imIn.getTotalSize());
	unsigned char* _outImage = _aes.EncryptCBC(imIn.getData(), imIn.getTotalSize(), _imKey.data(), _iv.data());

	ImageBase imOut = ImageBase(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	for(int i = 0; i<imIn.getTotalSize(); ++i)
		imOut[i/imOut.getWidth()][i%imOut.getHeight()] = _outImage[i];
	imOut.save(_pathOut);

	cout<<"PSNR : "<<imIn.PSNR(imOut, PEAK)<<endl;
	cout<<"Entropie originale : "<<imIn.Entropie(NB_GRAYSCALES)<<endl;
	cout<<"Entropie chiffrée : "<<imOut.Entropie(NB_GRAYSCALES)<<endl;
}

void D_CBC(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(_seed);
	vector<unsigned char> _imKey = vector<unsigned char>(imIn.getTotalSize());
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_imKey[i] = rand()%256;

	vector<unsigned char> _iv = vector<unsigned char>(imIn.getTotalSize());
	unsigned char* _outImage = _aes.DecryptCBC(imIn.getData(), imIn.getTotalSize(), _imKey.data(), _iv.data());

	ImageBase imOut = ImageBase(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	for(int i = 0; i<imIn.getTotalSize(); ++i)
		imOut[i/imOut.getWidth()][i%imOut.getHeight()] = _outImage[i];
	imOut.save(_pathOut);
}

void CFB(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(_seed);
	vector<unsigned char> _imKey = vector<unsigned char>(imIn.getTotalSize());
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_imKey[i] = rand()%256;

	vector<unsigned char> _iv = vector<unsigned char>(imIn.getTotalSize());
	unsigned char* _outImage = _aes.EncryptCFB(imIn.getData(), imIn.getTotalSize(), _imKey.data(), _iv.data());

	ImageBase imOut = ImageBase(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	for(int i = 0; i<imIn.getTotalSize(); ++i)
		imOut[i/imOut.getWidth()][i%imOut.getHeight()] = _outImage[i];
	imOut.save(_pathOut);

	cout<<"PSNR : "<<imIn.PSNR(imOut, PEAK)<<endl;
	cout<<"Entropie originale : "<<imIn.Entropie(NB_GRAYSCALES)<<endl;
	cout<<"Entropie chiffrée : "<<imOut.Entropie(NB_GRAYSCALES)<<endl;
}

void D_CFB(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(_seed);
	vector<unsigned char> _imKey = vector<unsigned char>(imIn.getTotalSize());
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_imKey[i] = rand()%256;

	vector<unsigned char> _iv = vector<unsigned char>(imIn.getTotalSize());
	unsigned char* _outImage = _aes.DecryptCFB(imIn.getData(), imIn.getTotalSize(), _imKey.data(), _iv.data());

	ImageBase imOut = ImageBase(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	for(int i = 0; i<imIn.getTotalSize(); ++i)
		imOut[i/imOut.getWidth()][i%imOut.getHeight()] = _outImage[i];
	imOut.save(_pathOut);
}

void OFB(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(_seed);
	vector<unsigned char> _imKey = vector<unsigned char>(imIn.getTotalSize());
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_imKey[i] = rand()%256;

	vector<unsigned char> _iv = vector<unsigned char>(imIn.getTotalSize());
	unsigned char* _outImage = _aes.EncryptOFB(imIn.getData(), imIn.getTotalSize(), _imKey.data(), _iv.data());

	ImageBase imOut = ImageBase(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	for(int i = 0; i<imIn.getTotalSize(); ++i)
		imOut[i/imOut.getWidth()][i%imOut.getHeight()] = _outImage[i];
	imOut.save(_pathOut);

	cout<<"PSNR : "<<imIn.PSNR(imOut, PEAK)<<endl;
	cout<<"Entropie originale : "<<imIn.Entropie(NB_GRAYSCALES)<<endl;
	cout<<"Entropie chiffrée : "<<imOut.Entropie(NB_GRAYSCALES)<<endl;
}

void D_OFB(char** argv)
{
	char _pathIn[250], _pathOut[250];
	int _seed;
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);
	sscanf (argv[3],"%d",&_seed);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(_seed);
	vector<unsigned char> _imKey = vector<unsigned char>(imIn.getTotalSize());
	for(int i=0; i<imIn.getTotalSize(); ++i)
		_imKey[i] = rand()%256;

	vector<unsigned char> _iv = vector<unsigned char>(imIn.getTotalSize());
	unsigned char* _outImage = _aes.DecryptOFB(imIn.getData(), imIn.getTotalSize(), _imKey.data(), _iv.data());

	ImageBase imOut = ImageBase(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	for(int i = 0; i<imIn.getTotalSize(); ++i)
		imOut[i/imOut.getWidth()][i%imOut.getHeight()] = _outImage[i];
	imOut.save(_pathOut);
}

void Bruite(char** argv)
{
	char _pathIn[250], _pathOut[250];
	sscanf (argv[1],"%s",_pathIn);
	sscanf (argv[2],"%s",_pathOut);

	ImageBase imIn;
	imIn.load(_pathIn);

	srand(time(NULL));

	int _sizeBlock = 16;
	int _nbBlockWidth = imIn.getWidth()/_sizeBlock;
	int _nbBlockHeight = imIn.getHeight()/_sizeBlock;

	for(int i=0; i<_nbBlockWidth; ++i)
		for(int j=0; j<_nbBlockHeight; ++j)
		{
			int _randWidth = rand()%((i+1)*_sizeBlock);
			int _randHeight = rand()%((j+1)*_sizeBlock);

			unsigned char _randBitMask = 1 << rand()%(sizeof(unsigned char)*8);
			imIn[_randWidth][_randHeight] = _randBitMask;
		}

	imIn.save(_pathOut);
}

int main(int argc, char **argv)
{
	D_CFB(argv);
	return 0;
}
