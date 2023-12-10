#include "LinkedList.h"
#include <Arduino.h>

#ifndef PEDOMETER_H
#define PEDOMETER_H

#include "LinkedList.h"
#include <Arduino.h>

class Pedometer {
public:
    Pedometer();
    virtual ~Pedometer();
    bool addInput(double input);
protected:
    double magnitudeBuffer[64];
    //arduino list
    LinkedList<double> magnitudeList = LinkedList<double>();
    LinkedList<double> filteredList = LinkedList<double>();
    double firFilter(LinkedList<double> *input, int inputSize, const double* coefficients, int coefficientsSize);
    double meanDifference();
    double runningMean;
    double runningStandardDeviation;
    void calculateRunningStats(double input);
    bool detectStep(LinkedList<double> *input);
    const double coefficients[9] = {0.0158467315853596,0.0457943754888393,0.121235976281198,0.200244477464560,0.233756878360087,0.200244477464560,0.121235976281198,0.0457943754888393,0.0158467315853596};
    bool checkForStep(double input);
    uint8_t countdown = 0;
};


Pedometer::Pedometer() {
    runningMean = 0;
    runningStandardDeviation = 0;
}

Pedometer::~Pedometer() {
}

//write fir filter from linked list 

double Pedometer::firFilter(LinkedList<double> *input, int inputSize, const double* coefficients, int coefficientsSize) {
        double output = 0;
        for (int j = 0; j < coefficientsSize; j++) {
            if (63 - j >= 0) {
                output += input->get(63 - j) * coefficients[j];
            }
        }
    
    // Serial.print("fir output:");
    // Serial.println(output);
    return output;
}

double Pedometer::meanDifference() {
    double mean = 0;
    for (int i = 0; i < filteredList.size(); i++) {
        mean += filteredList.get(i);
    }
    mean /= filteredList.size();
    double difference = filteredList.get(32) - mean;
    
    return difference;
}

void Pedometer::calculateRunningStats(double input) {
    runningMean = (runningMean * 63 + input) / 64;
    runningStandardDeviation = sqrt((runningStandardDeviation * runningStandardDeviation * 63 + (input - runningMean) * (input - runningMean)) / 64);
}
//This stage identifies potential candidate peaks to be associated with a step by statistically
// detecting outliers. As the algorithm processes the signal,
// it keeps track of a running mean and standard deviation.
// These two quantities are used to determine whether any given
// sample is an outlier. If the difference between the sample and
// the mean is above c times the standard deviation, then it is
// marked as a potential step. The parameter c is considered for
// optimisation.
bool Pedometer::checkForStep(double input) {
    if (abs(input - runningMean) > 1.2 * runningStandardDeviation) {
        return true;
    } else {
        return false;
    }
}

//firfilter on linkedlist
bool Pedometer::detectStep(LinkedList<double> *input) {//TODO can optimaze to use list instead of array
    //Serial.println(firFilter(input, 64, coefficients, 64), 4);
    filteredList.add(firFilter(&magnitudeList, 64, coefficients, 9));
    // Serial.print("filtered list size:");
    // Serial.println(filteredList.size());
    
    if (filteredList.size() > 64) {
        
        filteredList.shift();
        double difference = meanDifference();
        calculateRunningStats(difference);
        bool stepDetected = checkForStep(difference);
        if (countdown > 0){
            countdown--;
        }
        Serial.println(countdown);
        if (stepDetected && countdown <= 0) {
            countdown = 30;
            return true;
        }
    }
    return false;

}



bool Pedometer::addInput(double input) {
    // Serial.print("running mean:");
    // Serial.println(runningMean);
    // Serial.print("running standard deviation:");
    // Serial.println(runningStandardDeviation);
    magnitudeList.add(input);
    if (magnitudeList.size() > 64) {
        magnitudeList.shift();
        return detectStep(&magnitudeList);
    }
    return false;
}
#endif // PEDOMETER_H
