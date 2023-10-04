/******************************************************************************
* ICAR_Library
*
* Fichier : ImageBase.h
*
* Description : Classe contennant quelques fonctionnalit�s de base
*
* Auteur : Mickael Pinto
*
* Mail : mickael.pinto@live.fr
*
* Date : Octobre 2012
*
*******************************************************************************/

#pragma once
#include <stdio.h>
#include <stdlib.h>

class ImageBase
{
	///////////// Enumerations
public:
	typedef enum { PLAN_R, PLAN_G, PLAN_B} PLAN;


	///////////// Attributs
protected:
	unsigned char *data;
	double *dataD;

	bool color;
	int height;
	int width;
	int nTaille;
	bool isValid;


	///////////// Constructeurs/Destructeurs
protected:
	void init();
	void reset();

public:
	ImageBase(void);
	ImageBase(const ImageBase&);
	ImageBase(int imWidth, int imHeight, bool isColor);
	~ImageBase(void);
	
	///////////// Methodes
protected:
	void copy(const ImageBase &copy);

public:
	int getHeight() { return height; };
	int getWidth() { return width; };
	int getTotalSize() { return nTaille; };
	int getValidity() { return isValid; };
	bool getColor() { return color; };
	unsigned char *getData() { return data; };


	void load(const char *filename);
	bool save(const char *filename) const;

	ImageBase *getPlan(PLAN plan);

	unsigned char *operator[](int l);
	unsigned char *operator[](int l) const;
	void operator=(const ImageBase &copy);

//Measures
	double MSE(const ImageBase& _other) const;
	double PSNR(const ImageBase& _other, int _peak) const;
	void DDP(double* _ddpGreyscale, const int _nbGreyScales) const;
	double Entropie(const int _nbGreyScales) const;
	void Histogram(const char* _pathSave, const int _nbGreyScales) const;
	void Histogram(const char* _pathSave, int* _histogram, const int _nbGreyScales) const;

//Cryptage
	void Substition(ImageBase* _outImage, const int _seed) const;
	void Substition_inverse(ImageBase* _outImage, const int _seed) const;
	void Bruteforce_Substition_inverse(ImageBase* _outImage, unsigned int* _optimalSeed, const unsigned int _minSeed, const unsigned int _maxSeed, const int _nbGreyScales) const;

	void Permutation(ImageBase* _toPermute, const int _seed) const;
	void Permutation_inverse(ImageBase* _outImage, const int _seed) const;
};

