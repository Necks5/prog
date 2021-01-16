#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>

using namespace std;

class LinearRegression {
    int max_epoch;
    double alpha;
    double c;
    double threshold;
    int data_size;

    vector<double> weights;

    double max_norm;  // for normalization
    bool normalize;

public:
    LinearRegression(int max_epoch = 100,  double alpha = 0.01, 
        double c = 1, bool normalize = true){
        this->max_epoch = max_epoch;
        this->alpha = alpha;
        this->c = c;
        this->normalize = normalize;
    }

    int fit(vector< vector<double> > X, vector<double> y) {
        if(X.size() == 0 || y.size() == 0) {
            cout << "No data" << endl;
            return 1;
        }
        if(X.size() != y.size()) {
            cout << "X.size() != y.size()" << endl;
            return 1;
        }

        int data_len = y.size();
        data_size = X[0].size();
        //normalization
        if(normalize) {
            max_norm = -1;
            for(int i = 0; i < data_len; ++i) {
                for(int j = 0; j < data_size; ++j) {
                    if(abs(X[i][j]) > max_norm) {
                        max_norm = abs(X[i][j]);
                    }
                }
            }

            for(int i = 0; i < data_len; ++i) {
                for(int j = 0; j < data_size; ++j) {
                    X[i][j] /= max_norm;
                }
            }
        }
        
        // weights initialization
        data_size = X[0].size();
        weights.reserve(data_size + 1);
        for(int i = 0; i < data_size + 1; ++i) {
            weights[i] = (double)rand() / RAND_MAX;
        }
        

        for(int epoch = 0; epoch < max_epoch; ++epoch) {
            double epoch_loss = 0;
            for(int ind = 0; ind < data_len; ++ind) {
                double y_pred = 0.0;
                for(int i = 0; i < data_size; i++) {
                    y_pred += weights[i] * X[ind][i];
                }
                y_pred += weights[data_size];

                epoch_loss += (y_pred - y[ind]) * (y_pred - y[ind]);
                double grad = 2 * (y_pred - y[ind]);

                for(int i = 0; i < data_size; i++) {
                    weights[i] -=  alpha * (grad * X[ind][i]); 
                    // weights[i] -=  alpha * (grad * X[ind][i] + weights[i]/c); 

                }
                weights[data_size] -= alpha * grad;
            }
            epoch_loss /= data_len;
            cout << "Epoch: " << epoch << " loss: " << epoch_loss << endl;
        }

        return 0;
    }

    int predict(vector< vector<double> > X, vector<double> & y) {
        if(X.size() == 0) {
            cout << "No data \n";
            return 1;
        }
        if(X[0].size() != data_size) {
            cout << "Wrong shape \n";
            return 1;
        }

        int X_size = X.size();

        //normalization
        if(normalize) {
            for(int i = 0; i < X_size; ++i) {
                for(int j = 0; j < data_size; ++j) {
                    X[i][j] /= max_norm;
                }
            }
        }


        
        for(int ind = 0; ind < X_size; ++ind) {
            y[ind] = 0.0;
            for(int i = 0; i < data_size; ++i) {
                y[ind] += weights[i] * X[ind][i];
            }
            y[ind] += weights[data_size];
        }
        return 0;
    }
};