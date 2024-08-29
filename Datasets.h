#ifndef DATASETS_H
#define DATASETS_H

#define MNIST_TRAIN_LEN 60000
#define MNIST_TEST_LEN 10000
#define MNIST_TRAIN_SHAPES MNIST_TRAIN_LEN, 1, 28, 28
#define MNIST_TEST_SHAPES MNIST_TEST_LEN, 1, 28, 28
#define IMAGE_DATA 784 // 28 * 28

#include <iostream>
#include <vector>
#include <fstream>

#include "Volumes.h"

int reverseInt (int i);
void _normalizeSet(volume &set, int len, int nCols,  int nRows)

class MNIST {
    void _getSet(string path, volume& set);
    void _getLabel(string path, vector<int>& label);
    void _initMNIST(volume& TrainDS, volume& TestDS, volume& ValidDS,
                    vector<int>& TrainL, vector<int>& TestL, vector<int>& ValidL);
    public:
        void getMNIST(volume& TrainDS, volume& TestDS, volume& ValidDS,
                    vector<int>& TrainL, vector<int>& TestL, vector<int>& ValidL)
};

//<Insert other datasets here>

#endif