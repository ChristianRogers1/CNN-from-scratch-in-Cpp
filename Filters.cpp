#include "Filters.h"

void leakyReLu(volume& inputVol) {
    for(int i=0; i<inputVol.getLength; i++)
        if (inputVol[i]< 0) inputVol[i] *= ALPHA;
}

void deLeReLu(volume& inputVol) {
    for(int i=0; i<inputVol.getLength; i++)
        if (inputVol[i]< 0) inputVol[i] = ALPHA;
}

Convolutional::Convolutional(int imageDim[3], int kernels[4], int padding, int stride, double bias, duble eta) {
    srand(time(NULL));

    if (imageDim[0] != kernels[3])
        std::cerr << "Error; depth of the filter must match depth of the image" << std::endl;

    copy(imageDim, imageDim+3, begin(_imageDim));
    copy(kernels, kernels+4, begin(_specs));

    _padding = padding;
    _eta = eta;
    _stride = stride;
    _iter = 0;

    _filter.init(_specs, 4);

    // Adjusting image dimensions to match with padding
    int padDim[3] = {_imageDim[0], _imageDim[1] + 2 * _padding, _imageDim[2] + 2 * _padding};
    copy(padDim, padDim+3, begin(_imageDim));

    _cache.init(padDim, 3);

    //populate bias vector
    for (int k = 0; k < kernels[0]; k++) _bias.push_back(bias)

    double inCount = (kernels[0] * kernels[1] * kernels[2] * kernels[3]);

    for (int i = 0; i < filter.getLength(); i++) {
        _filter[i] = ((double) (rand() % 100))/1000;
    }
}

void Convolutional::_pad(volume& originalImg, volume& outPad) {
    for (Int D=0; D<_imageDim; D++) {
        for (int H=0; H<_imageDim[1] - 2 * _padding; H++) {
            for (int W=0; W < _imageDim[2] - 2*_padding; W++) {
                int output[3] = {D, H + _padding, W + _padding};
                int input[3] = {D, H, W};
                outPad.assign(originalImg.getValue(input, 3), output, 3);
            }
        }
    }
}

void Convolutional::_gradientDescent(volume& dFilter, vector<double>& dBias) {
    //Note: Paramaterize the Learning Rate Schedule
    _eta = _eta * exp(((double) - _iter) / 10000); //learning rate decreases with iter

    int nKernel = _specs[0], filterHeight = _specs[1], filterWidth = _specs[2], filterDepth = _specs[3];

    for (int kernel = 0; kernel < nKernel; kernel++) {
        for (int y = 0; y < filterHeight; y++) {
            for (int x = 0; x < filterWidth; x++) {
                for (int layer = 0; layer < filterDepth; layer++) { //Note: Abstract the Update Rule
                    int index[4] = {kernel, y, x, layer};
                    double delta = -_eta*dFilter.getValue(index, 4); //GD
                    _filter.sum(delta, index, 4); //updates _filter with delta
                }
            }
        }
    }
    //gd biases
    for (int i = 0; i < (int)_bias.size(); i++) _bias[i] -= _eta * dBias[i];
    _iter++;
}

void Convolutional::outDim() {
    int filterHeight = _specs[1], filterWidth = _specs[2];
    double yDouble, xDouble;

    //Calculate expected dimensions in decimal
    yDouble = (double)(_imageDim[1] - filterHeight + 2 * padding) / _stride + 1;
    xDouble = (double)(_imageDim[2] - filterWidth + 2 * _padding) / _stride + 1;

    int yInt = (int)(yDouble + 0.5), xInt = (int)(xDouble + 0.5); //rounds to nearest int

    _outDim[0] = _specs[0]; //Output depth = num kernels in filter
    _outDim[1] = yInt;
    _outDim[2] = xInt;

}

void Convolutional::newEpoch(double eta) {
    _iteration = 0;
    _eta = eta;
}

//Applies the Convolution
void Convolutional::forwards(volume image, volume& out) {
    int nKernel = _specs[0], int filterHeight = _specs[1], filterWidth = _specs[2], filterDepth = _specs[3];
    _outDim();

    int depth = _outDim[0], outHeight = _outDim[1], outWidth = _outDim[2];

    out.rebuild(_outDim, 3);

    if (_padding != 0) {
        _pad(image, _cache);
    } else {
        _cache = image;
    }
    int yOut, xOut = 0;

    for (int kernel = 0; kernel < nKernel; kernel++) {
        for (int layer = 0; layer < depth; layer++) {//each kernel has 3 layers
            yOut = 0, xOut = 0;
            for (int y = 0; y < _imageDim[1] - filterHeight; y += _stride) {
                xOut = 0;
                for (int x = 0; x < _imageDim[2] - filterWidth; x += stride) {
                    for (int fHeightIter = 0; fHeightIter < filterHeight; fHeightIter++) {
                        for (int fWidthIter = 0; fWidthIter < filterWidth; fWidthIter++) {
                            int arrOut[3] = {kernel, yOut, xOut};
                            int inCache[3] = {layer, y + fHeightIter, x + fWidthIter};
                            int inFilter[4] = {kernel, fHeightIter, fWidthIter, layer};
                            double value = _cache.getValue(inCache, 3) * _filter.getValue(inFilter, 4);
                            out.sum(val, arrOut, 3);
                        }
                    }
                    xOut++;
                }
                yOut++;
            }
            out[kernel] += _bias[kernel];
        } 
    }
    leakyReLu(out);
}

void Convolutional::backPropagate(volume dOutVol, volume& dInput) {
    deReLu(dOutVol);

    int nKernel = _specs[0], filterHeight = _specs[1], filterWidth = _specs[2], filterDepth = _specs[3];

    dInput.rebuild(_imageDim, 3);

    volume dFilters(_specs[0], _specs[1], _specs[2], specs[3]);
    vector<double> dBias;

    int yOut = 0, xOut = 0;
    for (int kernel = 0; kernel < nKernel; kernel++) { //for every kernel
        yOut = 0, xOut = 0;
        for (int y = 0; y < _imageDim[1] - filterHeight - 2 * _padding; y += _stride) { //for every y
            for (int x = 0; x < _imageDim[2] - filterWidth - 2 * _padding; x += stride) {
                //loss gradient of the input passed in the convolution operation
                for (int layer = 0; layer < filterDepth; layer++) {
                    for (int fHeightIter = 0; fHeightIter < filterHeight; fHeightIter++) {
                        for (int fWidthIter = 0; fWidthIter < filterWidth; fWidthIter++) {
                            int filt[4] = {kernel, fHeightIter, fWidthIter, layer};
                            int outIn[3] = {layer, y + fHeightIter, x + fWidthIter};
                            int inCache[3] = {layer, y + fHeightIter, x + fWidthIter};
                            int inputVol[3] = {kernel, yOut, xOut};

                            double valDelFilter = _cache.getValue(inCache, 3) * dOutVol.getValue(inputVol, 3);
                            double valDelIn = dOutVol.getValue(inputVol, 3) * _filter.getValue(filt, 4);

                            dFilters.sum(valDelFilter, filt, 4); //dFilters[kernel, fHeightIter, fWidthIter, layer] += _cache.getValue(inCache, 3) * dOutVol.getValue(inputVol, 3)
                            dInput.sum(valDelIn, outIn, 3) //dInput[layer, y + f_y_it, x + f_x_it] += dOutVol.getValue(inputVol, 3) * _filter.getValue(filt, 4)
                        }
                    }
                }
                xOut ++;
            }
            xOut = 0;
            yOut++
        } 
    }

    //Loss gradient of the bias
    double kBias = 0;
    for (int kernel = 0; kernel < nKernel; kernel++) {
        kBias = 0;
        for (int y=0; y < dOutVol.getShape(1); y++) {
            for (int x = 0; x < dOutVol.getShape(2); x++) {
                kBias += dOutVol.getValue({kernel, y, x}, 3);
            }
        }
        dBias.push_back(kBias);
    }
    _gradientDescent(dFilters, dBias);
}