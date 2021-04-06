/*
 * File:         calcSpecRule.cpp
 * Author:       Carter Hale
 * Date Created: March 24, 2021
 * Last Updated: April 6, 2021
*/

#include <iostream>
#include "ConwayClassifier.h"


/*
 * To test certain Ruleset Simulations to obtain Metrics
 */
int main(int argc, char **argv)
{   
    // Variables
    int gens;
    std::string rule;
   
    // Get Inputs
    std::cout << "Ruleset (e.g. b2_s23):";
    std::cin >> rule;
    std::cout << "Generation Count:";
    std::cin >> gens;

    // CC Constructor with 3 Threads and Calculating Back 30 Percent
    ConwayClassifier c("/home/CellAutomataGA/Desktop/Golly Patterns/Test Patterns/" + rule, gens, 3, 30);

    // Display Metrics
    std::cout << std::endl << "PercentChangeAvg: " << c.getPercentChange() << std::endl;
    std::cout << "AliveCellRatioAvg: " << c.getAliveCellRatio() << std::endl;
    std::cout << "ActiveCellRatioAvg: " << c.getActiveCellRatio() << std::endl;
    return 0;
}