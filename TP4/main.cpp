#include "ImageBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h> 
#include <numeric>
#include <vector>
#include <random>
using namespace std;

float moyenneur[9] = {1/9.f, 1/9.f, 1/9.f, 1/9.f, 1/9.f, 1/9.f, 1/9.f, 1/9.f, 1/9.f};
float moyenneur2[9] = {0, 1/5.f, 0, 1/5.f, 1/5.f, 1/5.f, 0, 1/5.f, 0};
float gauss[9] = {1/16.f, 2/16.f, 1/16.f, 2/16.f, 4/16.f, 2/16.f, 1/16.f, 2/16.f, 1/16.f};
float flou[9] = {1/16.f, 2/16.f, 1/16.f, 2/16.f, 4/16.f, 2/16.f, 1/16.f, 2/16.f, 1/16.f};

float sharpen[9] = {0,-1,0,-1,5,-1,0,-1,0};
float laplacien[9] = {0,1,0,1,-4,1,0,1,0};
float contourV[9] = {1,0,-1,1,0,-1,1,0,-1};
float contourH[9] = {1,1,1,0,0,0,-1,-1,-1};

float ReLU(float _value)
{
	return max(0.f, _value);
}

void Convolution(const ImageBase _imageIn, const float* _kernel, ImageBase& _outImage)
{
	_outImage = ImageBase(_imageIn.getWidth()-2, _imageIn.getHeight()-2, _imageIn.getColor());

	const float _outHeight = _outImage.getHeight();
	const float _outWidth = _outImage.getWidth();

	for(int _heightIndex = 0; _heightIndex<_outHeight; ++_heightIndex)
		for(int _widthIndex = 0; _widthIndex<_outWidth; ++_widthIndex)
		{
			for(int _plan = 0; _plan<3; ++_plan)
			{
				float _convulResult = 
					_kernel[0]*_imageIn[3*_heightIndex][3*_widthIndex+_plan]+
					_kernel[1]*_imageIn[3*(_heightIndex+1)][3*_widthIndex+_plan]+
					_kernel[2]*_imageIn[3*(_heightIndex+2)][3*_widthIndex+_plan]+
					_kernel[3]*_imageIn[3*_heightIndex][3*(_widthIndex+1)+_plan]+
					_kernel[4]*_imageIn[3*(_heightIndex+1)][3*(_widthIndex+1)+_plan]+
					_kernel[5]*_imageIn[3*(_heightIndex+2)][3*(_widthIndex+1)+_plan]+
					_kernel[6]*_imageIn[3*_heightIndex][3*(_widthIndex+2)+_plan]+
					_kernel[7]*_imageIn[3*(_heightIndex+1)][3*(_widthIndex+2)+_plan]+
					_kernel[8]*_imageIn[3*(_heightIndex+2)][3*(_widthIndex+2)+_plan];

				//Activation
				_outImage[3*_heightIndex][3*_widthIndex+_plan] = ReLU(_convulResult);
			}
		}
}

void MaxPooling(const ImageBase _imageIn, ImageBase& _outImage)
{
	_outImage = ImageBase(_imageIn.getWidth()/2, _imageIn.getHeight()/2, _imageIn.getColor());

	const float _outHeight = _outImage.getHeight();
	const float _outWidth = _outImage.getWidth();

	for(int _heightIndex = 0; _heightIndex<_outHeight; ++_heightIndex)
		for(int _widthIndex = 0; _widthIndex<_outWidth; ++_widthIndex)
		{
			for(int _plan = 0; _plan<3; ++_plan)
			{
				_outImage[3*_heightIndex][3*_widthIndex+_plan] = 
				max(
					max(_imageIn[3*(2*_heightIndex)][3*(2*_widthIndex)+_plan], _imageIn[3*(2*_heightIndex+1)][3*(2*_widthIndex)+_plan]),
					max(_imageIn[3*(2*_heightIndex)][3*(2*_widthIndex+1)+_plan], _imageIn[3*(2*_heightIndex+1)][3*(2*_widthIndex+1)+_plan])
				);
			}
		}
}

void Flatten(const vector<ImageBase> _imagesIn, vector<unsigned char>& _concatenation)
{
	_concatenation = vector<unsigned char>();
	for(const ImageBase& _img : _imagesIn)
	{
		const float _height = _img.getHeight();
		const float _width = _img.getWidth();

		for(int _heightIndex = 0; _heightIndex<_height; ++_heightIndex)
			for(int _widthIndex = 0; _widthIndex<_width; ++_widthIndex)
				for(int _plan = 0; _plan<3; ++_plan)
					_concatenation.push_back(_img[3*_heightIndex][3*_widthIndex+_plan]);
	}
}

void CNNLayer(const vector<ImageBase>& _imgs, const vector<float*> _kernels, vector<ImageBase>& _imgsOut)
{
	_imgsOut = vector<ImageBase>(_imgs.size()*_kernels.size());
	ImageBase _imgConv;
	ImageBase _imgMaxPool;
	for(int i=0; i<_kernels.size(); ++i)
	{
		const float* _kernel = _kernels[i];
		for(int j=0; j<_imgs.size(); ++j)
		{
			const ImageBase& _img = _imgs[j];
			Convolution(_img, _kernel, _imgConv);
			MaxPooling(_imgConv, _imgMaxPool);
			_imgsOut[i*_imgs.size()+j] = _imgMaxPool;
		}	
	}
}

void CNNLayer(const ImageBase& _img, const vector<float*> _kernels, vector<ImageBase>& _imgsOut)
{
	_imgsOut = vector<ImageBase>(_kernels.size());
	ImageBase _imgConv;
	ImageBase _imgMaxPool;
	for(int i=0; i<_kernels.size(); ++i)
	{
		const float* _kernel = _kernels[i];
		Convolution(_img, _kernel,_imgConv);
		MaxPooling(_imgConv, _imgMaxPool);
		_imgsOut[i] = _imgMaxPool;
	}	
}

ImageBase VectorToImage(const vector<unsigned char>& _vector)
{
	const unsigned int _sizeImg = sqrt((_vector.size()/3.f));
	ImageBase _out = ImageBase(_sizeImg, _sizeImg, true);
	for(unsigned long i = 0; i<_vector.size(); i+=3)
	{
		if(i >= _out.getTotalSize() || i+1 >= _out.getTotalSize() || i+2 >= _out.getTotalSize()) return _out;
		_out[i/_sizeImg][i%_sizeImg] = _vector[i];
		_out[i/_sizeImg][(i%_sizeImg)+1] = _vector[i+1];
		_out[i/_sizeImg][(i%_sizeImg)+2] = _vector[i+2];
	}
}

void Normalisation(const vector<unsigned char>& _vector, vector<float>& _output)
{
	_output = vector<float>(_vector.size());
	for(int i=0; i<_vector.size(); ++i)
		_output[i] = _vector[i]/255.f;
}


template <typename T>
void NeuralLayer(unsigned int _nbNeurones, vector<T> _previousLayer, vector<float>& _layer)
{
	const int _previousLayerSize = _previousLayer.size();
	_layer = vector<float>(_nbNeurones);
	vector<float> _weights = vector<float>(_previousLayerSize*_nbNeurones);

	srand(time(NULL));
	for(int i=0; i<_weights.size(); ++i)
		_weights[i] = ((float)rand()/(RAND_MAX))*2.f-1.f;

	for(int i=0; i<_nbNeurones; ++i)
		for(int j=0; j<_previousLayerSize; ++j)
		{
			_layer[i] += _previousLayer[j]*_weights[i*_previousLayerSize+j];
		}
	
	//Activation
	for(int i=0; i<_nbNeurones; ++i)
		_layer[i] = ReLU(_layer[i]);
}

int main(int argc, char **argv)
{
	char _pathIn[250];
	sscanf (argv[1],"%s",_pathIn);
	string _pathInStr = _pathIn;

	ImageBase _in;
	_in.load(_pathIn);

	vector<ImageBase> _outputLayer1, _outputLayer2, _outputLayer3, _outputLayer4;

	CNNLayer(_in, {gauss, moyenneur, laplacien}, _outputLayer1);
	CNNLayer(_outputLayer1, {contourH, moyenneur2, contourV}, _outputLayer2);
	CNNLayer(_outputLayer2, {sharpen}, _outputLayer3);
	CNNLayer(_outputLayer3, {moyenneur, flou}, _outputLayer4);

	vector<unsigned char> _outputFlattened;
	Flatten(_outputLayer4, _outputFlattened);

	VectorToImage(_outputFlattened).save((_pathInStr+"_VC.ppm").c_str());

	vector<float> _normalizedLayer, _outputLayer;
	Normalisation(_outputFlattened, _normalizedLayer);

	NeuralLayer(2, _normalizedLayer, _outputLayer);
	cout<<"Neurone 1: "<<_outputLayer[0]<<endl;
	cout<<"Neurone 2: "<<_outputLayer[1]<<endl;
	return 0;
}
