/******************************************************************************
* ICAR_Library
*
* Fichier : ImageBase.cpp
*
* Description : Voir le fichier .h
*
* Auteur : Mickael Pinto
*
* Mail : mickael.pinto@live.fr
*
* Date : Octobre 2012
*
*******************************************************************************/

#include "ImageBase.h"
#include "image_ppm.h"
#include <fstream>
using namespace std;


ImageBase::ImageBase(void)
{
	isValid = false;
	init();
}

ImageBase::ImageBase(int imWidth, int imHeight, bool isColor)
{
	isValid = false;
	init();

	color = isColor;
	height = imHeight;
	width = imWidth;
	nTaille = height * width * (color ? 3 : 1);
	
	if(nTaille == 0)
		return;
	
	allocation_tableau(data, OCTET, nTaille);
	dataD = (double*)malloc(sizeof(double) * nTaille);
	isValid = true;
}

ImageBase::ImageBase(const ImageBase& _other)
{
	copy(_other);
}


ImageBase::~ImageBase(void)
{
	reset();
}

void ImageBase::init()
{
	if(isValid)
	{
		free(data);
		free(dataD);
	}

	data = 0;
	dataD = 0;
	height = width = nTaille = 0;
	isValid = false;
}

void ImageBase::reset()
{
	if(isValid)
	{
		free(data);
		free(dataD);
	}
	isValid = false;
}

void ImageBase::load(const char *filename)
{
	init();

	int l = strlen(filename);

	if(l <= 4) // Le fichier ne peut pas etre que ".pgm" ou ".ppm"
	{
		printf("Chargement de l'image impossible : Le nom de fichier n'est pas conforme, il doit comporter l'extension, et celle ci ne peut �tre que '.pgm' ou '.ppm'");
		exit(0);
	}

	int nbPixel = 0;

	if( strcmp(filename + l - 3, "pgm") == 0) // L'image est en niveau de gris
	{
		color = false;
		lire_nb_lignes_colonnes_image_pgm(filename, &height, &width);
		nbPixel = height * width;
  
		nTaille = nbPixel;
		allocation_tableau(data, OCTET, nTaille);
		lire_image_pgm(filename, data, nbPixel);
	}
	else if( strcmp(filename + l - 3, "ppm") == 0) // L'image est en couleur
	{
		color = true;
		lire_nb_lignes_colonnes_image_ppm(filename, &height, &width);
		nbPixel = height * width;
  
		nTaille = nbPixel * 3;
		allocation_tableau(data, OCTET, nTaille);
		lire_image_ppm(filename, data, nbPixel);
	}
	else 
	{
		printf("Chargement de l'image impossible : Le nom de fichier n'est pas conforme, il doit comporter l'extension, et celle ci ne peut �tre que .pgm ou .ppm");
		exit(0);
	}
	
	dataD = (double*)malloc(sizeof(double) * nTaille);

	isValid = true;
}

bool ImageBase::save(const char *filename) const
{
	if(!isValid)
	{
		printf("Sauvegarde de l'image impossible : L'image courante n'est pas valide");
		exit(0);
	}

	if(color)
		ecrire_image_ppm(filename, data,  height, width);
	else
		ecrire_image_pgm(filename, data,  height, width);

	return true;
}

ImageBase *ImageBase::getPlan(PLAN plan)
{
	if( !isValid || !color )
		return 0;

	ImageBase *greyIm = new ImageBase(width, height, false);
	
	switch(plan)
	{
	case PLAN_R:
		planR(greyIm->data, data, height * width);
		break;
	case PLAN_G:
		planV(greyIm->data, data, height * width);
		break;
	case PLAN_B:
		planB(greyIm->data, data, height * width);
		break;
	default:
		printf("Il n'y a que 3 plans, les valeurs possibles ne sont donc que 'PLAN_R', 'PLAN_G', et 'PLAN_B'");
		exit(0);
		break;
	}

	return greyIm;
}

void ImageBase::copy(const ImageBase &copy)
{
	reset();
	
	isValid = false;
	init();
	
	color = copy.color;
	height = copy.height;
	width = copy.width;
	nTaille = copy.nTaille;
	isValid = copy.isValid;
	
	if(nTaille == 0)
		return;
	
	allocation_tableau(data, OCTET, nTaille);
	dataD = (double*)malloc(sizeof(double) * nTaille);
	isValid = true;

	for(int i = 0; i < nTaille; ++i)
	{
		data[i] = copy.data[i];
		dataD[i] = copy.dataD[i];
	}

}

unsigned char *ImageBase::operator[](int l)
{
	if(!isValid)
	{
		printf("L'image courante n'est pas valide");
		exit(0);
	}
	
	if((!color && l >= height) || (color && l >= height*3))
	{
		printf("L'indice se trouve en dehors des limites de l'image");
		exit(0);
	}
	
	return data+l*width;
}

unsigned char *ImageBase::operator[](int l) const
{
	if(!isValid)
	{
		printf("L'image courante n'est pas valide");
		exit(0);
	}
	
	if((!color && l >= height) || (color && l >= height*3))
	{
		printf("L'indice se trouve en dehors des limites de l'image");
		exit(0);
	}
	
	return data+l*width;
}

void ImageBase::operator=(const ImageBase &_copy)
{
	copy(_copy);
}

double ImageBase::MSE(const ImageBase& _other) const
{
	double _MSE = 0;
	for(int j = 0; j < height; ++j)
		for(int i = 0; i < width; ++i)
			_MSE += ((*this)[j][i] - _other[j][i]) * ((*this)[j][i] - _other[j][i]);

	_MSE /= width*height;
	return _MSE;
}

double ImageBase::PSNR(const ImageBase& _other, const int _peak) const
{
	const double _MSE = MSE(_other);
	return 10*log10(_peak*_peak/_MSE);
}

void ImageBase::Histogram(const char* _pathSave, int* _histogram, const int _nbGreyScales) const
{
	for(int i = 0; i < _nbGreyScales; ++i)
		_histogram[i] = 0;

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
			++_histogram[(int)(*this)[i][j]];

	if(!_pathSave) return;

	ofstream _writeStream(_pathSave);
	for(int i = 0; i < 256; ++i)
		_writeStream << i << " " << (int)_histogram[i] << endl;
	_writeStream.close();
}

void ImageBase::DDP(double* _ddpGreyscale, const int _nbGreyScales) const
{	
	for(int i = 0; i < _nbGreyScales; ++i)
		_ddpGreyscale[i] = 0;

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
			++_ddpGreyscale[(int)(*this)[i][j]];

	for(int i = 0; i < _nbGreyScales; ++i)
		_ddpGreyscale[i] /= (float)nTaille;
}

double ImageBase::Entropie(const int _nbGreyScales) const
{
	double _greyScalesDDP[_nbGreyScales];
	DDP(_greyScalesDDP, _nbGreyScales);

	double _entropie = 0;
	for(int i = 0; i < 256; ++i)
		_entropie +=  _greyScalesDDP[i] == 0 ? 0 : _greyScalesDDP[i] * log2(_greyScalesDDP[i]);

	_entropie *=-1;
	return _entropie;
}


void ImageBase::Substition(ImageBase* _outImage, const int _seed) const
{
	*_outImage = ImageBase(width, height, color);
	ImageBase imKey(width, height, color);

	srand(_seed);

	unsigned char* _dataOut = _outImage->getData();

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
			imKey[i][j] = rand()%256; 

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
		{
			if(i*width+j-1 < 0) (*_outImage)[i][j] = ((*this)[i][j] + imKey[i][j])%256;
			else (*_outImage)[i][j] = (_dataOut[i*width+j-1] + (*this)[i][j] + imKey[i][j])%256;
		}
}

void ImageBase::Substition_inverse(ImageBase* _outImage, const int _seed) const
{
	*_outImage = ImageBase(width, height, color);
	ImageBase imKey(width, height, color);

	srand(_seed);

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
			imKey[i][j] = rand()%256; 

	for(int i = height-1; i >= 0; --i)
		for(int j = width-1; j >= 0; --j)
		{
			if(i*width+j-1 < 0) (*_outImage)[i][j] = ((*this)[i][j] - imKey[i][j])%256;
			else (*_outImage)[i][j] = ((*this)[i][j] - data[i*width+j-1] - imKey[i][j])%256;
		}
}

void ImageBase::Bruteforce_Substition_inverse(ImageBase* _outImage, unsigned int* _optimalSeed, const unsigned int _minSeed, const unsigned int _maxSeed, const int _nbGreyScales) const
{
	int _bestSeed = 0;
	double _lowerEntro = numeric_limits<double>().max();

	*_outImage = ImageBase(width, height, color);

	for(int _seed = _minSeed; _seed < _maxSeed; ++_seed)
	{
		ImageBase imTemp(width, height, color);
		ImageBase imKey(width, height, color);

		srand(_seed);

		for(int i = 0; i < height; ++i)
			for(int j = 0; j < width; ++j)
				imKey[i][j] = rand()%256; 

		for(int i = height-1; i >= 0; --i)
			for(int j = width-1; j >= 0; --j)
			{
				if(i*width+j-1 < 0) imTemp[i][j] = ((*this)[i][j] - imKey[i][j])%256;
				else imTemp[i][j] = ((*this)[i][j] - data[i*width+j-1] - imKey[i][j])%256;
			}

		const double _currentEntro = imTemp.Entropie(_nbGreyScales);
		if(_lowerEntro > _currentEntro)
		{
			*_outImage = imTemp;
			*_optimalSeed = _seed;
			_lowerEntro = _currentEntro;
		}
	}
}

void ImageBase::Permutation(ImageBase* _outImage, const int _seed) const
{
	*_outImage = ImageBase(width, height, color);
	ImageBase map(width, height, color);

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
			map[i][j] = 0; 

	srand(_seed);

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
		{
			int _newPos = rand() % nTaille;
			bool _hasPixel = map[_newPos/width][_newPos%width];
			while(_hasPixel)
			{
				_newPos = (_newPos+1)% nTaille;
				_hasPixel = map[_newPos/width][_newPos%width];
			}
			map[_newPos/width][_newPos%width] = 1;
			(*_outImage)[_newPos/width][_newPos%width] = (*this)[i][j];
		}
}

void ImageBase::Permutation_inverse(ImageBase* _outImage, const int _seed) const
{
	*_outImage = ImageBase(width, height, color);
	ImageBase map(width, height, color);

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
			map[i][j] = 0; 

	srand(_seed);

	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
		{
			int _newPos = rand() % nTaille;
			bool _hasPixel = map[_newPos/width][_newPos%width];
			while(_hasPixel)
			{
				_newPos = (_newPos+1)% nTaille;
				_hasPixel = map[_newPos/width][_newPos%width];
			}
			map[_newPos/width][_newPos%width] = 1;
			(*_outImage)[i][j] = (*this)[_newPos/width][_newPos%width];
		}
}
