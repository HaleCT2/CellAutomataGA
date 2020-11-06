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
    ConwayClassifier c("/home/schonaem/NetBeansProjects/"
    "conwayClassifier/TestPatterns/b25_s26", 1000, 50);
    //c.printGameBoard(1000, std::cout, '@', '.');
    // now compare results to rle's in golly
    return 0;
}

