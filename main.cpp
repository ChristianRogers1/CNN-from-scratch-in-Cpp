#include <iostream>
#include <vector>

#include "CNN.h"

bool adam;
double bias, eta;
vector<int> image1{1, 28, 28}, kernels1{8, 3, 3, 1};
vector<int> image2{8, 13, 13}, kernels2{2, 3, 3, 8}, hidden{72};
int inLayer, numClasses, epochs, padding, stride;

int main(int argc, char** argv) {
    CNN network;
    
    network.addConv(image1, kernels1, padding= 0, stride= 2, bias= 0.1, eta= 0.01 );
    network.addConv(image2 , kernels2 , padding= 0, stride= 2, bias= 0.1, eta= 0.01);
    network.addDense(inputLayer=2*6*6, hidden, numClasses=10, bias=1.0,  adam=false, eta=0.5);

    network.loadDataset("MNIST");

    //Sanity check:
    //network.sanityCheck();

    network.training(epochs = 1, 10);

    network.testing(10);

    return 0;
}