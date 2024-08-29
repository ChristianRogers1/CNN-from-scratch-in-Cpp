#ifndef CNN_H
#define CNN_H

#include <vector>
#include <ctime>
#include <math.h>
#include <string.h>

#include "Volumes.h"
#include "Filters.h"
#include "Datasets.h"
#include "MLP.h"

class CNN {
    vector<char> _layers;
    vector<double> _result;
    vector<Convolutional> _convs;
    vector<Pooling> _pools;
    vector<MultiLayerPerceptron> _dense;

    int _convIndex = 0, _poolIndex = 0, _totLayers = 0, _numClasses = 0;
    int _denseInputShape[3] = {0,0,0};
    int _imageShape[3] = {0,0,0};

    volume TrainDS, TestDS, ValidDS;

    vector<int> TrainL, TestL, ValidL;

    vector<double> trainAcc, validAcc, testAcc;
    vector<double> trainLoss, validLoss, testLoss;

    void _forward(volume& image);
    void _backward(vector<double>& gradient);
    void _iterate(volume& dataset, vector<int>& labels, vector<double>& lossList, vector<double>& accList, int previewPeriod, bool bTraining = true);
    void _getImage(volume& image, volume& dataset, int index);

    public:
        CNN() = default;
        void addConv(vector<int>& imageDim, vector<int>& kernels, int padding = 1, int stride = 1, double bias = 0.1, double eta = 0.01);
        void addPooling(int imageDim[3], char mode = 'a', int size = 2, int stride = 2, int padding = 0);
        void addDense(int input, vector<int>& hidden, int numClasses = 10, doube bias = 1.0, bool adam = true, double eta = 0.01);
        void loadDataset(string dataName);
        void training(int epochs = 1, int previewPeriod = 1);
        void testing(int previewPeriod = 1);
        void sanityCheck(int setSize = 50, int epochs = 200);
        void plotResults();
        ~CNN();
};

#endif