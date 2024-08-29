#include "MLP.h"

double frand() { //generates a random floating point number 0.0-0.099
    return (double)(rand() % 100) / 1000;
}

Perceptron::Perceptron(int inputs, double bias) {
    this->bias = bias;
    weights.resize(inputs+1);
    m.resize(inputs+1,0);
    v.resize(inputs+1,0);
    generate(weights.begin(), weights.end(), frand); //populate weights with frand()
}

//Run the perceptron. x is a vector with the input values
double Perceptron::run(vector<double> x) {
    x.push_back(bias);
    double sum = inner_product(x.begin(), x.end(), weights.begin(), (double)0.0); //dot product
    return sigmoid(sum);
}

//Set the weights. wInit is a vector with the weights
void Perceptron::setWeights(vector<double> wInit) {
    weights = wInit;
}

double Perceptron::sigmoid(double x) {
    return 1.0/(1.0+exp(-x));
}

MultiLayerPerceptron::MultiLayerPerceptron(vector<int> layers, double bias, bool adam, double eta) {
    srand(time(NULL));

    this->layers = layers;
    this->bias = bias;
    this->eta = eta;
    backIter = 0; //backpropagation iterations
    bAdam = adam; //boolean

    for (int i = 0; i < (int) layers.size(); i++) {
        values.push_back(vector<double>(layers[i], 0.0));
        d.push_back(vector<double>(layers[i], 0.0));
        lossGradient.push_back(vector<double>(layers[i], 0.0));
        network.push_back(vector<Perceptron>());
        if (i > 0)  //network[0] is the input layer, so has no neurons
            for (int j = 0; j < layers[i]; j++)
            network[i].push_back(Perceptron(layers[i-1], bias));
    }
}

//Set the weights, wInit is a vector of vectors of vectors with the weights for all but the input layer
void MultiLayerPerceptron::setWeights(vector<vector<vector<double> > > wInit) {
    for (int i = 0; i < (int) wInit.size(); i++)
        for (int j = 0; j < (int) wInit[i].size(); j++)
            network[i+1][j].setWeights(wInit[i][j]);
}

//iterates through every weight for every neuron in every layer and prints
void MultiLayerPerceptron::printWeights() {
    std::cout << std::endl;
    for (int i = 1; i < (int) network.size(); i++) {
        for (int j = 0; j < layers[i]; j++) {
            std::cout << "Layer " << i+1 << " Neuron " << j << ": ";
            for (auto &it: network[i][j].weights)
                std::cout << it << "   ";
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

//Feed a sample x into the MLP
vector<double> MultiLayerPerceptron::run(vector<double> x) {
    values[0] = x;
    for (int i = 1; i < (int) network.size(); i++)
        for (int j = 0; j < layers[i]; j++)
            values[i][j] = network[i][j].run(values[i-1]);
    return values.back();
}

//NB m and v are updates inside the function
//See ReadMe file for explanation of math behind this function
double MultiLayerPerceptron::Adam(double &m, double &v, double derivative) {
    int t = backIter;
    double dx = derivative;

    m = BETA1 * m + (1-BETA1) * dx; //First Moment Estimate
    double mt = m / (1-(double)pow(BETA1, t)); //Bias correction (early training stages = less mvmt)
    v = BETA2 * v + (1-BETA2) * ((double)pow(dx, 2)); //Updating Second Moment Estimate
    double vt = v / (1-(double)pow(BETA2, t)); //Bias correction
    double delta = eta * mt / (sqrt(vt) + EPS); //calculate parameter update

    return delta;
}

void MultiLayerPerceptron::gd() {//gradient descent
    //STEPS 5-6: Calculate deltas and update the weights
    for (int i = 1; i < (int) network.size(); i++) { //iter layers from 1st hidden
        for (int j = 0; j < layers[i]; j++) { //iter neurons
            for (int k = 0; k < layers[i-1]+1; k++) { //iter weights
                double delta;
                if (k == layers[i-1]) {
                    delta = eta * d[i][j];                      //learning rate * dscore
                } else {
                    double dw = d[i][j] * values[i-1][k];       //dw = dscore * X

                    if (!bAdam) delta = eta * dw;               //learning rate * dw
                    else delta = Adam(network[i][j].m[k], network[i][j].v[k], dw);
                }
                network[i][j].weights[k] += delta;          //update weight with delta
            }
        }
    }
}

vector<double> MultiLayerPerceptron::bp(vector<double> error) { //back propagation
    backIter++; //To update Adam

    vector<double> outputs = values.back();

    //STEP 3: Calculate the output error terms
    for (int i = 0; i < (int) outputs.size(); i++)
        d.back()[i] = outputs[i] * (1 - outputs[i]) * (error[i]);

    //STEP 4: Calculate the error term of each unit on each layer
    for (int i = ((int) network.size())-2; i > 0; i--) {
        for (int h = 0; h < (int) network[i].size(); h++) {
            double fwdError = 0.0;

            for (int k = 0; k < layers[i+1]; k++) //calculates contribution of each neuron in next layer to error term of current
                fwdError += network[i+1][k].weights[h] * d[i+1][k]; //sums products of weights h->next and error terms from next

            lossGradient[i][h] = fwdError;
            d[i][h] = values[i][h] * (1 - values[i][h]) * fwdError;
        }
    }
    gd();

    return lossGradient[1]; //Result from the first layer (not input)
}