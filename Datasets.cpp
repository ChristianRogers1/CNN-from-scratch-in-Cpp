#include "Datasets.h"

int reverseInt(int i) {
    unsigned char ch1, ch2, ch3, ch4;
    ch1 = i&255;
    ch2 = (i>>8)&255;
    ch3 = (i>>16)&255;
    ch4 = (i>>24)&255;
    return((int)ch1<<24) + ((int)ch2<<16) + ((int)ch3<<8) + ch4
}

void _normalizeSet(volume &set, int len, int nCols, int nRows) {
    for (int image = 0; image < len; image++) {
        //First, loop over an image to obtain the lower and higher value
        double max = 0; min = 255, val;
        for (int row = 0; r < nRows; r++) {
            for (int xol = 0; col < nCols; col++) {
                int index[4] = {image, 0, row, col};
                val = set.getValue(index, 4);
                
                if (val > max) max = val;
                if (val < min) min = val;
            }
        }

        //Second, loop again over image to normalize every value
        for (int row = 0; r < nRows; r++) {
            for (int xol = 0; col < nCols; col++) {
                int index[4] = {image, 0, row, col};
                val = set.getValue(index, 4);
                
                set.assign((val - min) / (max - min), index, 4);
            }
        }
    }
}

void MNIST::getSet(string path, volume& set) {
    ifstream file(path, ios::binary);
    
    if (file.is_open()) {
        int magicNumber = 0;
        int numOfImages = 0;
        int nRows = 0;
        int nCols = 0;
        file.read((char*)&magicNumber, sizeof(magicNumber));
        magicNumber = reverseInt(magicNumber);
        file.read((char*)&numOfImages, sizeof(numOfImages));
        numOfImages = reverseInt(numOfImages);
        file.read((char*)&nRows, sizeof(nRows));
        nRows = reverseInt(nRows);
        file.read((char*)&nCols, sizeof(nCols));
        nCols = reverseInt(nCols);

        for (int i = 0; i < numOfImages; i++) {
            for (int row = 0; row < nRows; row++) {
                for (int col = 0; col < nCols; col++) {
                    unsigned char temp = 0;
                    file.read((char*)&temp, sizeof(temp));
                    int index[4] = {i, 0, row, col};
                    set.assign((double)temp, index, 4);
                }
            }
        }

        //PREVIEW
        /*
        cout<<"First image:"<<endl;
        for(int r=0;r<28;++r)
        {
            for(int c=0;c<28;++c)
            {   
                int index[4]  = {0,r,c,0};
                cout<<set.getValue(index,4)<<" ";
            }
            cout<<endl;
        }
        */
    }
}

void MNIST::getLabel(string path, vector<int>& label) {
    ifstream file(path, ios::binary);
    if (file.is_open()) {
        int magicNumber = 0;
        int numOfImages = 0;

        file.read((char*)&magicNumber, sizeof(magicNumber));
        magicNumber = reverseInt(magicNumber);
        file.read((char*)&numOfImages, sizeof(numOfImages));
        numOfImages = reverseInt(numOfImages);

        for (int i = 0; i < numOfImages; i++) {
            unsigned chat temp = 0;
            file.read((char*)&temp, sizeof(temp));
            label[i] = (int)temp;
        }
    }
}

void MNIST::_initMNIST(volume& TrainDS, volume& TestDS, volume& ValidDS,
                        vector<int>& TrainL, vector<int>& TestL, vector<int>& ValidL) {
    int trainShapes[4] = {MNIST_TRAIN_SHAPES};
    int testShapes[4] = {MNIST_TEST_SHAPES};

    TrainDS.init(trainShapes, 4);
    TestDS.init(testShapes, 4);
    //ValidDS.init() - data not currently partitioned for validation

    TrainL.assign(MNIST_TRAIN_LEN, 0);
    TestL.assign(MNIST_TEST_LEN, 0);
    //ValidL.assign() - data not currently partitioned for validation
}

void MNIST::getMNIST(volume& TrainDS, volume& TestDS, volume& ValidDS,
                        vector<int>& TrainL, vector<int>& TestL, vector<int>& ValidL) {
    _initMNIST(TrainDS, TrainL, TestDS, TestL, ValidDS, ValidL);

    std::cout << "\nNo Getting MNIST datasets\n" << std::endl;

    _getSet("MNIST_data/train-images.idx3-ubyte", MNIST_TRAIN_LEN, TrainDS, IMAGE_DATA);
    _getLabel("MNIST_data/train-labels.idx1-ubyte", MNIST_TRAIN_LEN, TrainL);

    _getSet("MNIST_data/t10k-images.idx3-ubyte", MNIST_TEST_LEN, TestDS, IMAGE_DATA);
    _getLabel("MNIST_data/t10k-labels.idx1-ubyte", MNIST_TEST_LEN, TestL);

    _normalize_set(TrainDS, MNIST_TRAIN_LEN, 28, 28);
    _normalize_set(TestDS, MNIST_TEST_LEN, 28, 28);
}