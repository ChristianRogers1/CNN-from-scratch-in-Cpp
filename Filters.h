#ifndef FILTERS_H
#define FILTERS_H

#define ALPHA 0.001

#include <iostream>
#include <random>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <iterator>
#include <ctime>
#include <vector>

#include "Volumes.h"

void leakyReLu(volume& inputVol);
void deReLu(volume& inputVol);

class Convolutional {
    int _imageDim[3] = {1,16,16};    //image specification
    int _specs[4]     = {2,3,3,1};    //filter specs
    int _outDim[3]   = {2,13,13};    //convoluted output dimensions

    int _padding = 1;
    int _stride = 2;
    int _iter = 0;          //To update gradient descent
    double _eta = 0.1;

    vector<double> _bias;   //List of bias

    volume _filter;
    volume _cache;

    void _pad(volume& rawImage, volume& outImage);

    void _gd(volume& dFilter, vector,double>& dBias);

    void _outDim():

    public:
        Convolutional(int imageDim[3], int kernels[4], int padding=1, int stride=1, double bias=0.1, duble eta=0.01);

        void newEpoch(double eta);

        void forwards(volume image, volume& out);

        void backPropagate(volume dOutVol, volume& dInput);
};

#endif