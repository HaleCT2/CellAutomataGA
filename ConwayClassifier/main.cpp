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
    ConwayClassifier c("b3457_s3", 1000);
    c.printGameBoard(685);
    std::cout << c.classification() << std::endl;
    // will check how it works by calling different public methods and
    // comparing it to the data
    return 0;
}

