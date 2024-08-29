#ifndef VOLUMES_H
#define VOLUMES_H

#include <iostream>
#include <vector>

#define vector std::vector

class volume {
    vector<double> _mtx;
    vector<int> _shape; //shape of tensor
    int _dim = 0, _length = 0; //dimension count and length (product of all val in shape)

    public:
        volume();   //Void volume object

        volume(int H, int W); //2D
        volume(int H, int W, int Depth); //3D
        volume(int Layers, int H, int W, int Depth); //4D
        volume(int* shapes, int dimensions); //AnyD

        void init(const int* shapes, int dimensions); //Useful to postpone instantiation
        void rebuild(int* shapes, int dimentsions);

        int getShape(int dimNum); //Gets size of dimension dimNum
        int getLength();
        double getValue(int* index, int dimensions);
        vector<double>& getVector();

        void assign(double val, int* index, int dimensions);
        void sum(double val, int* index, int dimensions);

        volume& operator = (const volume &startVol);

        double& operator[](int index);
};

#endif