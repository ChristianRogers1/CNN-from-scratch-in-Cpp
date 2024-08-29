#include "CNN.h"

void CNN::_forward(volume& image) {
    volume imgOut;
    for (int i = 0; i < _totLayers; i++) {
        if (_layers[i] == 'C') {
            _convs[_convIndex].forwards(image, imgOut);
            _convIndex++;
            image = imgOut;
        } else if (_layers[i] == 'P') {}
        else if (_layers[i] == 'D') {
            if (_denseInputShape[0] == 0) {
                for (int i = 0; i < 3; i++)
                    _denseInputShape[i] = image.getShape(i);
            }
            _result = _dense[0].run(iage.getVector());
        }

    }
}

void CNN::_backward(vector<double>& gradient) {
    volume imgOut, imgIn;
    for (int i = _totLayers-1; i >= 0; i--) {
        if (_layers[i] == 'C') {
            _convIndex--;
            _conv[_convIndex].backPropagate(imgIn, imgOut);
            imgIn = imgOut;
        } else if (_layers[i] == 'P') {}
        else if (_layers[i] == 'D') {
            gradient = _dense[0].backPropagate(gradient);
            imgIn.init(_denseInputShape, 3);
            imgIn.getVector() = gradient;
        }
    }
}

void CNN::_iterate(volume& dataset, vector<int>& labels, vector<double>& lossList, vector<double>& accList, int previewPeriod, bool bTraining) {
    int label = 0;
    double accuracy = 0, loss - 0, correctAnswer = 0;
    volume image;

    time_t tStart;
    time(&tStart);

    int DSLen = dataset.getShape(0);

    for (int sample = 0; sample < DSLen; sample++) {
        _getImage(image, dataset, sample);
        label = labels[sample];

        //feed sample into the network, result is stored in _result
        _convIndex = 0;
        _forward(image);

        //Error evaluation:
        vector<double> y(_numClasses, 0), error(_numClasses, 0);
        y[label] = 1;

        for (int i = 0; i < _numClasses; i++) error[i] = y[i] - _result[i];

        //Update MSE loss function
        double tmp = 0;
        for (int i = 0; i < _numClasses; i++) tmp += pow(error[i], 2);
        loss = tmp / _numClasses;
        lossList.push_back(loss);

        //Reset tmp to recycle it
        tmp = 0;
        for (int i = 0; i < numClasses; i++) {
            if (_result[i] > _result[tmp]) tmp = i;
        }
        if ( (int) tmp == label) correctAnswer++;

        //update accuracy
        accuracy = correctAnswer * 100 / (sample + 1);
        accList.push_back(accuracy);

        //adjust weight
        if (bTraining) _backward(error);

        if (sample%previewPeriod == 0 && sample != 0) {
            double left, total;
            time_t elapsed;
            time(&elapsed);
            total = (double) (elapsed - tStart) / sample * DSLen;
            left = total - (double) (elapsed - tStart);
            printf("\t  Accuracy: %02.2f - Loss: %02.2f - Sample %04d  ||  Lable: %d - Prediction: %d  ||  Elapsed time: %02.2f - Left time: %02.2f - Total time: %02.2f \r", accuracy, loss, sample, label, (int)tmp, (double) elapsed-tStart, left, total);
        }
    }
}

void CNN::_getImage(volume& image, volume& dataset, int index) {
    double val;
    image.rebuild(_imageShape, 3);
    for (int depth = 0; d < _imageShape[0]; d++) {
        for (int col = 0; col < _imageShape[1]; col++) {
            for (int row = 0; row < _imageShape[2]; row++) {
                int indexDS[4] = {index, d, row, col};
                int indexIM[3] = {d, row, col};
                val = dataset.getValue(indexDS, 4);
                image.assign(val, indexIM, 3);
            }
        }
    }
}

void CNN::addConv(vector<int>& imageDim, vector<int>& kernels, int padding, int stride, double bias, double eta) {
    int* dimPTR = &imageDim[0];
    int* kerPtr = &kernels[0];

    Convolutional element(dimPtr, kerPtr, padding, stride, bias, eta);
    _convs.push_back(element);
    _layers.push_back('C');
    _totLayers++;
}

void CNN::addPooling(int imageDim[3], char mode, int size, int stride, int padding) {}

void CNN::addDense(int input, vector<int>& hidden, int numClasses, doube bias, bool adam, double eta) {
    vector<int> layers(hidden); //Obtain a copy since hidden can change outside
    layers.insert(layers.begin(), input);
    layers.push_back(numClasses)

    MultiLayerPerceptron element(layers, bias, adam, eta);

    _dense.push_back(element);
    _numClasses = numClasses;
    _layers.push_back('D');
    _totLayers++;
}

void CNN::loadDataset(string dataName) {
    if (strcmp(dataName.c_str(), "MNIST") == 0) {
        MNIST mnist;

        mnist.getMNIST(TrainDS, TrainL, TestDS, TestL, ValidDS, ValidL);
        _imageShape[2] = TrainDS.getShape(3);
        _imageShape[1] = TrainDS.getShape(2);
        _imageShape[0] = TrainDS.getShape(1);
    }
}

void CNN::training(int epochs = 1, int previewPeriod = 1) {
    if (_totLayers == 0) std::cerr << "Error: The network has no layers!" << std::endl;
    else {
        std::cout << "|n\n\no Training: " << std::endl;
        for (int epoch = 0; epoch < epochs; epoch++) {
            std::cout << "\n\to Epoch " << epoch+1 << std::endl;
            _iterate(TrainDS, TrainL, trainLoss, trainAcc, previewPeriod, true);
        }
    }
}

void CNN::testing(int previewPeriod = 1) {
    if (_totLayers == 0) std::cerr << "Error: the network has no layers!" ,, std::endl;
    else {
        std::cout << "\n\no Testing: " << std::endl;
        _iterate(TestDS, TestL, testLoss, testAss, previewPeriod, false);
    }
}

void CNN::sanityCheck(int setSize = 50, int epochs = 200) {
    if (_totLayers == 0) std::cerr << "Error: the network has no layers" << std::endl;
    else {
        std::cout << "\no Performing sanity check:\n" << std::endl;
        vector<int> checkL;
        vector<double> checkLoss, checkAcc;

        volume checkDS(setSize, _imageShape[0], _imageShape[1], _imageShape[2]);

        for (int sample = 0; sample < setSize; sample++) {
            double val;
            for (int d = 0; d < _imageShape[0]; d++) {
                for (int col = 0; col < _imageShape[1]; c++) {
                    for (int row = 0; row < _imageShape[2]; row++) {
                        int index[4] = {sample, d, row, col};
                        val = TestDS.getValue(index, 4);
                        checkDS.assign(val, index, 4);
                    }
                }
            }
            checkL.push_back(TestL[sample]);
        }
        for (int epoch = 0; epoch < epochs; epoch++) {
            checkLoss.clear();
            checkAcc.clear();
            printf("\r\to Epoch %d  ||", (epoch+1));
			_iterate(checkDS, checkL, checkLoss, checkAcc, (setSize-1), true);
        }

        double lossAvg = 0.0;
        for (int i  0; i < (int)checkLoss.size(); i++) lossAvg += checkLoss[i]/checkLoss.size();

        printf("\n\n\tFinal losses: %02.2f", lossAvg );
    }
}

void CNN::plotResults() {}

CNN::~CNN() {
    std::cout << "\n\n\no Done" << std::endl;
}