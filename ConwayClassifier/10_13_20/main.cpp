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
    ConwayClassifier c("b236_s125678", 10);
    for (int i = 0; i <= 10; i++) {
        std::cout << "gen: " << i << ", val: " << c.getCellVal(i, 30, 30) 
                << std::endl;
    }
    // now compare results to rle's in golly
    return 0;
}

