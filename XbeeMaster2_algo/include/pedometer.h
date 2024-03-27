#include "LinkedList.h"
#include <Arduino.h>

#ifndef PEDOMETER_H
#define PEDOMETER_H

#include "LinkedList.h"
#include <Arduino.h>

#define WINDOW_SIZE 31
#define MIDPOINT 15 //Half of WINDOW_SIZE
#define THRESHOLD 1.2
#define COUNTDOWN 30 //Wait COUNTDOWN samples before detecting another step(should be period of fastest steps possible)
#define FIR_FILTER_SIZE 9
enum algorithm {MEAN_DIFFERENCE, MAX_DIFFERENCE, PANTOMKINS};

class Pedometer {
public:
    Pedometer();
    Pedometer(algorithm);
    algorithm algo;
    virtual ~Pedometer();
    bool addInput(double input);
protected:
    double magnitudeBuffer[WINDOW_SIZE];
    //arduino list
    LinkedList<double> magnitudeList = LinkedList<double>();
    LinkedList<double> filteredList = LinkedList<double>();
    double firFilter(LinkedList<double> *input, int inputSize, const double* coefficients, int coefficientsSize);
    double meanDifference();
    double maxDifference();
    double PanTomkins();
    double runningMean;
    double runningStandardDeviation;
    void calculateRunningStats(double input);
    bool detectStep(LinkedList<double> *input);
    const double coefficients[FIR_FILTER_SIZE] = {0.0158467315853596,0.0457943754888393,0.121235976281198,0.200244477464560,0.233756878360087,0.200244477464560,0.121235976281198,0.0457943754888393,0.0158467315853596};
    bool checkForStep(double input);
    uint8_t countdown = 0;
    size_t count = 0;
};


Pedometer::Pedometer() {
    runningMean = 0;
    runningStandardDeviation = 0;
}
Pedometer::Pedometer(algorithm algo) {
    runningMean = 0;
    runningStandardDeviation = 0;
    this->algo = algo;

}
Pedometer::~Pedometer() {
}



double Pedometer::firFilter(LinkedList<double> *input, int inputSize, const double* coefficients, int coefficientsSize) {
        double output = 0;
        for (int j = 0; j < FIR_FILTER_SIZE; j++) {
                output += input->get(WINDOW_SIZE - j - 1) * coefficients[j];
        }
    
    // Serial.print("fir output:");
    // Serial.println(output);
    return output;
}

double Pedometer::meanDifference() {
    double mean = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        mean += filteredList.get(i);
    }
    mean /= filteredList.size();
    double difference = filteredList.get(MIDPOINT) - mean;
    
    return difference;
}
double Pedometer::maxDifference() {
    double max = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        if (abs(filteredList.get(i)) > max) {
            max = abs(filteredList.get(i));
        }
    }
    return max;
}

double Pedometer::PanTomkins() {
    double mean = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        mean += filteredList.get(i);
    }
    mean /= filteredList.size();
    double difference = filteredList.get(MIDPOINT) - mean;
    if (difference > 0) {
        return difference * difference;
    } else {
        return 0;
    }
}
void Pedometer::calculateRunningStats(double input) {
    
    if (count < 100000){ //Careful, overflow can cause division by 0, also high numbers make this step pointless
        count += 1;
    }
    runningMean = (runningMean * (count - 1) + input) / count;
    runningStandardDeviation = sqrt((runningStandardDeviation * runningStandardDeviation * (count - 1) + (input - runningMean) * (input - runningMean)) / count);
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
    if (abs(input - runningMean) > THRESHOLD * runningStandardDeviation) {
        return true;
    } else {
        return false;
    }
}

//firfilter on linkedlist
bool Pedometer::detectStep(LinkedList<double> *input) {//TODO can optimaze to use list instead of array
    //Serial.println(firFilter(input, 64, coefficients, 64), 4);
    filteredList.add(firFilter(&magnitudeList, WINDOW_SIZE, coefficients, 9));
    // Serial.print("filtered list size:");
    // Serial.println(filteredList.size());
    if (filteredList.size() > WINDOW_SIZE) {
        filteredList.shift();
        double difference;
        if (algo == MAX_DIFFERENCE){
            difference = maxDifference();
        }
        else if (algo == PANTOMKINS){
            difference = PanTomkins();
        }
        else{
            difference = meanDifference();
        }
        calculateRunningStats(difference);
        bool stepDetected = checkForStep(difference);
        if (countdown > 0){
            countdown--;
        }
        // Serial.print("countdown:");
        // Serial.println(countdown);
        if (stepDetected && countdown <= 0) {
            countdown = COUNTDOWN;
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
    if (magnitudeList.size() > WINDOW_SIZE) {
        magnitudeList.shift();
        return detectStep(&magnitudeList);
    }
    return false;
}
#endif // PEDOMETER_H
