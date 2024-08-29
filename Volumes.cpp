#include "Volumes.h"

volume::volume() {

}

volume::volume(int H, int W) {
    _mtx.resize(H *W);
    _length = H * W;
    _dim = 2;
    _shape.resize(_dim);
    _shape[0] = H;
    _shape[1] = W;
}

volume::volume(int H, int W, int Depth) {
    _mtx.resize(H *W *Depth, 0);
    _length = H * W * Depth;
    _dim = 3;
    _shape.resize(_dim);
    _shape[0] = H;
    _shape[1] = W;
    _shape[2] = Depth;
}

volume::volume(int Layers, int H, int W, int Depth) {
    _mtx.resize(H *W *Depth *Layers);
    _length = Layers * H * W * Depth;
    _dim = 4;
    _shape.resize(_dim);
    _shape[0] = Layers;
    _shape[1] = H;
    _shape[2] = W;
    _shape[3] = Depth;
}

volume::volume(int* shapes, int dimensions) {
    _dim = dimensions;
    _length = 1;

    _shape.assign(shapes, shapes+dimensions);

    for (int i = 0; i < dimensions; i++) _length *= shapes[i];

    _mtx.resize(_length);
}

void volume::init(const int* shapes, int dimensions) {
    if (_mtx.size() != 0) std::cerr << ("Error: volume already allocated") << std::endl;
    else {
        _length = 1;
        _dim = dimensions;
        _shape.assign(shapes, shapes + dimensions);

        for (int i = 0; i < dimensions; i++) _length *= shapes[i];

        _mtx.resize(_length);
    }
}

void volume::rebuild(int* shapes, int dimensions) {
    _dim = dimensions;
    _shape.assign(shapes, shapes + dimensions);
    _length = 1;

    for (int i = 0; i < dimensions; i++) _length *= shapes[i];

    _mtx.assign(_length, 0); //resizes and initializes all values to 0;
}

int volume::getShape(int dimNum) {
    if (dimNum > _dim) std::cerr << ("Error: getShape() cannot access a dimension outside of the shape!") << std::endl;
    else {
        return _shape[dimNum];
    }
    return 0;
}

int volume::getLength() {
    return _length;
}

double volume::getValue(int* index, int dimensions) {
    if (dimensions != _dim) std::cerr << "Error, dimensions must match the dimensions of the volume" << std::endl;
    else {
        int mtxIndex = 0, diff = 1;
        for (int i = 0; i < dimensions; i++) {
            diff = 1;
            for (int iterShape=0; iterShape < i; iterShape++) diff *= _shape[iterShape];
            mtxIndex += index[i] * diff;
        }
        return _mtx[mtxIndex];
    }
}

vector<double>& volume::getVector() {
    return _mtx;
}

void volume::assign(double val, int* index, int dimensions) {
    if (dimensions != _dim) std::cerr << "Error, dimensions must match the dimensions of the volume" << std::endl;
    else {
        int mtxIndex = 0, diff = 1;
        for (int i = 0; i < dimensions; i++) {
            diff = 1;
            for (int iterShape=0; iterShape < i; iterShape++) diff *= _shape[iterShape];
            mtxIndex += index[i] * diff;
        }
        _mtx[mtxIndex] = val;
    }
}

void volume::sum(double val, int* index, int dimensions) {
    if (dimensions != _dim) std::cerr << "Error, dimensions must match the dimensions of the volume" << std::endl;
    else {
        int mtxIndex = 0, diff = 1;
        for (int i = 0; i < dimensions; i++) {
            diff = 1;
            for (int iterShape=0; iterShape < i; iterShape++) diff *= _shape[iterShape];
            mtxIndex += index[i] * diff;
        }
        _mtx[mtxIndex] += val;
    }
}

//Deallocate internal _mtx memory, allocate for startVol, copy, then return *this
volume& volume::operator=(const volume &startVol) {
    if (this == &startVol) return *this;
    else {
        _shape.resize(0), _mtx.resize(0);
        _length = 0, _dim = 0;

        this->init(&(startVol._shape[0]), startVol._dim);

        this->_mtx=startVol._mtx;
    }

    return *this;
}

double& volume::operator[](int index) {
    if (index > _length) {
        std:cerr << "Array index out of bound, returned last element." << std::endl;
        return _mtx.back();
    }
    
    return _mtx[index]
}