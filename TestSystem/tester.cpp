#include "ConwayClassifier.h"

/*
 * To test certain Ruleset Simulations to obtain Metrics
 */
int main(int argc, char **argv)
{
    int gens = 150;
    int percent = 30;
    int startGen = gens - (((double)gens / (double)100) * (double)percent);
    ConwayClassifier c("/home/CellAutomataGA/Desktop/Golly Patterns/Test Patterns/b2_s23", gens, 20, percent);

    std::cout << "PercentChangeAvg: " << c.getPercentChange() << std::endl;
    std::cout << "AliveCellRatioAvg: " << c.getAliveCellRatio() << std::endl;
    std::cout << "ActiveCellRatioAvg: " << c.getActiveCellRatio() << std::endl;
    std::cout << "ExpansionRateXAvg: " << c.getExpansionRateX() << std::endl;
    std::cout << "ExpansionRateYAvg: " << c.getExpansionRateY() << std::endl;
    std::cout << "ExpansionRateAreaAvg: " << c.getExpansionRateArea() << std::endl;
    return 0;
}