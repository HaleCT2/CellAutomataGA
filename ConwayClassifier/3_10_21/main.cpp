/* 
 * File:   main.cpp
 * Author: Eric Schonauer
 *
 * Created on September 18, 2020, 1:23 PM
 */

#include "ConwayClassifier.h"

/*
 * To test the ConwayClassifier class
 */
int main(int argc, char** argv) {
    int gens = 750;
    int percent = 10;
    int startGen = gens - (((double) gens / (double) 100) * (double) percent); 
    ConwayClassifier c("/home/schonaem/NetBeansProjects/"
    "conwayClassifier/TestPatterns/b25_s26", gens, 50, percent);
    for (int i = startGen; i <= gens; i++) {
        std::cout << "PercentChange " << i << ": " << c.getPercentChange(i) << std::endl;
    }
    std::cout << "PercentChangeAvg: " << c.getPercentChange() << std::endl;
    for (int i = startGen; i <= gens; i++) {
        std::cout << "AliveCellRatio " << i << ": " << c.getAliveCellRatio(i) << std::endl;
    }
    std::cout << "AliveCellRatioAvg: " << c.getAliveCellRatio() << std::endl;
    return 0;
}

