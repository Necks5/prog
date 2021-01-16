#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "LinearRegression.h"

using namespace std;



int main() {
    vector < vector <double> > X_train(4, vector <double> (2) );
    X_train[0][0] = 1;
    X_train[0][1] = 1;
    X_train[1][0] = 1;
    X_train[1][1] = 2;
    X_train[2][0] = 2;
    X_train[2][1] = 2;
    X_train[3][0] = 2;
    X_train[3][1] = 3;

    vector<double> y_train = {6, 8, 9, 11};

    vector<double> y_test(4);

    LinearRegression linreg(150, 0.05);

    cout << "FIT\n";
    linreg.fit(X_train, y_train);

    cout << "\n\nPREDICT\n";
    linreg.predict(X_train, y_test);

    for(int i = 0; i < 4; ++i) {
        cout << y_test[i] << endl;
    }

    return 0;
}