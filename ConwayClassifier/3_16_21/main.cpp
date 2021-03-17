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
    int gens = 1000;
    int percent = 10;
    int startGen = gens - (((double) gens / (double) 100) * (double) percent); 
    ConwayClassifier c("/home/schonaem/NetBeansProjects/"
    "conwayClassifier/TestPatterns/b25_s26", gens, 50, percent);
//    for (int i = startGen; i <= gens; i++) {
//        std::cout << "PercentChange " << i << ": " << c.getPercentChange(i) << std::endl;
//    }
//    std::cout << "PercentChangeAvg: " << c.getPercentChange() << std::endl;
//    for (int i = startGen; i <= gens; i++) {
//        std::cout << "AliveCellRatio " << i << ": " << c.getAliveCellRatio(i) << std::endl;
//    }
//    std::cout << "AliveCellRatioAvg: " << c.getAliveCellRatio() << std::endl;
//    for (int i =  startGen; i <= gens; i++) {
//        std::cout << "ActiveCellRatio: " << i << ": " << c.getActiveCellRatio(i) << std::endl;
//    }
//    std::cout << "ActiveCellRatioAvg: " << c.getActiveCellRatio() << std::endl;

    for (int i = startGen; i <= gens; i++) {
        std::cout << "ExpansionRateX: " << i << ": " << c.getExpansionRateX(i) << std::endl;
    }
    std::cout << "ExpansionRateXAvg: " << c.getExpansionRateX() << std::endl;
    for (int i = startGen; i <= gens; i++) {
        std::cout << "ExpansionRateY: " << i << ": " << c.getExpansionRateY(i) << std::endl;
    }
    std::cout << "ExpansionRateYAvg: " << c.getExpansionRateY() << std::endl;
    for (int i = startGen; i <= gens; i++) {
        std::cout << "ExpansionRateArea: " << i << ": " << c.getExpansionRateArea(i) << std::endl;
    }
    std::cout << "ExpansionRateAreaAvg: " << c.getExpansionRateArea() << std::endl;
    return 0;
}

