/* 
 * File:   main.cpp
 * Author: Owen Hichens, Carter Hale
 * Based on Genetic Algorithm found at
 * https://www.geeksforgeeks.org/genetic-algorithms/
 *
 * Created on October 14, 2020
 * Last Updated on February 22, 2021
 */

#include <bits/stdc++.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
#include <string>
#include <cmath>    
#include "ConwayClassifier.h"
#include "rapidxml.hpp"

using namespace std; 
using namespace rapidxml;

// Global Variables from XML Config
int generation = 0; 
int populationSize;
int elitismPercent;
int crossoverRate;
int mutationRate;
int timeElapsed;

// Possible characters to make up genome. Note that the genome is a binary
// number represented as a string for simplicity.
const string GENES = "01010"; // Testing different Ruleset Creation (2/5 Odds)

/**
 * Random number generator method
 * 
 * @param start: lowest possible number
 * @param end: highest possible number
 * @return int the random number
 */
int random_num(int start, int end) { 
    int range = (end-start)+1; 
    int random_int = start+(rand()%range); 
    return random_int; 
} 

/**
 * Dump created Rule Set files
 */
void dump() {
    const char *command = "rm rule_sets*.txt";
    system(command);
}

/**
 * Decodes binary strings into golly rule sets
 * 
 * @param chromosome: binary string
 * @return string golly rule set
 */
string decode(string chromosome) {
    string ruleSet = "";
    ruleSet += "b";
    for(int i = 0; i < 9; i++) {
        if (chromosome[i] == '1') {
            ruleSet += to_string(i);
        }
    }
    ruleSet += "/s";
    for(int i = 9; i < 18; i++) {
        if (chromosome[i] == '1') {
            ruleSet += to_string(i - 9);
        }
    }
    return ruleSet;
}

/**
 * Encodes golly rule sets as binary strings
 * 
 * @param ruleSet: golly rule set string
 * @return string binary string
 */
string encode(string ruleSet) {
    string chromosome = "000000000000000000";
    bool survive = false;
    for(int i = 0; i < ruleSet.length(); i++) {
        if (ruleSet[i] != 'b' && ruleSet[i] != 's') {
            int index = ruleSet[i] - '0';
            if (survive) {
                index += 9;
            }
            chromosome[index] = '1';
        } else if (ruleSet[i] == 's') {
            survive = true;
        }
    }
    return chromosome;
}

/**
 * Mutation function
 * 
 * @return char random 1 or 0 
 */
char mutated_genes() { 
    int len = GENES.size(); 
    int r = random_num(0, len-1); 
    return GENES[r]; 
} 

/**
 * Creates new genome
 * 
 * @return string new genome
 */
string create_gnome() { 
    int len = 18; // Binary String Size 
    string gnome = ""; 
    for(int i = 0;i<len;i++) {
        gnome += mutated_genes(); 
    }
    return gnome; 
} 

/**
 * Class for individuals
 */
class Individual { 
public: 
    string chromosome; 
    double fitness; 
    Individual(string chromosome); 
    Individual mate(Individual parent2); 
    double cal_fitness(); 
}; 

/**
 * Constructor that takes in a chromosome
 * 
 * @param chromosome chromosome to be associated with individual
 */
Individual::Individual(string chromosome) { 
    this->chromosome = chromosome; 
}; 

/**
 * Crossover function which creates a child from two parents
 * 
 * @param par2: Second Individual used in crossover
 * @return Individual the child for new generation
 */
Individual Individual::mate(Individual par2) { 
    string child_chromosome = ""; 
    int len = chromosome.size(); 
    // Determine Chromosome based on Parents and Mutation Rate
    for(int i = 0;i<len;i++) {
        float p = random_num(0, 100)/100.;
        if(p < (float)(100-mutationRate)/200) {
            child_chromosome += chromosome[i]; 
        } else if(p < (float)(100-mutationRate)/100) {
            child_chromosome += par2.chromosome[i]; 
        } else {
            child_chromosome += mutated_genes(); 
        }
    }
    return Individual(child_chromosome); 
}; 

/**
 * Calculates the fitness of the Individual
 * 
 * @return int fitness number
 */
double Individual::cal_fitness() {
    // FilePath is a constant on the Virtual Machine
    string filePath = "/home/CellAutomataGA/Desktop/Golly Patterns/Simulation/Generation_" + to_string(generation);
    // Rename Decoded Chromosome
    string fileName = decode(this->chromosome);
    std::replace(fileName.begin(), fileName.end(), '/', '_');
    // Create CC Object 
    ConwayClassifier c(filePath + "/" + fileName, timeElapsed, 2, 30);

    // Calculate Metrics and Weights
    double AliveCell = c.getAliveCellRatio(-1);
    double PercentChange = c.getPercentChange(-1);

    double AliveWeight = 0;
    double PercentWeight = 0;

    // Determine Closeness to 'Ideal' Metrics
    if (AliveCell > 0 && AliveCell < 0.4) {
        AliveWeight = (0.20  - abs(AliveCell - 0.20))/0.20;
    }
    if (PercentChange > 0 && PercentChange < 0.33) {
        PercentWeight = (0.165  - abs(PercentChange - 0.165))/0.165;
    }
    
     // Return Fitness Value (Max of 5, Min of 1)
    return c.classification() + AliveWeight + PercentWeight;
}; 

/**
 * Operator override for sort function allowing the sort function to apply
 * to our custom Individual class
 * 
 * @param ind1: First individual
 * @param ind2: Second individual
 * @return bool true if ind1's fitness number is greater than ind2's fitness
 *         number, false otherwise
 */
bool operator<(const Individual &ind1, const Individual &ind2) { 
    return ind1.fitness > ind2.fitness; 
} 

/**
 * Method to save population of rule sets to a text file
 * 
 * @param population Rule sets to save
 */
void toFile(vector<Individual> population, int gen) {
    ofstream out;
    string str = "rule_sets" + to_string(gen) + ".txt"; 
    out.open(str);
    for(int i = 0; i < population.size(); i++) {
        out << decode(population[i].chromosome) << endl;
    }
    out.close();
}

/**
 * Method to Fork and Run Golly or Fork and Run a 
 * Python Script that resets 'CurrentGeneration' field
 * 
 * @param reset To determine if Configuration XML needs reset
 */
void generatePatterns(bool reset) {
    const int pid= fork();
    if (reset) {
        if (pid== 0) {
            execlp("python3", "python3", "resetXML.py", nullptr);
        } else {
            waitpid(pid, nullptr, 0);
        }
    } else {
        if (pid== 0) {
            execlp("golly", "golly", "golly-script.py", nullptr);
        } else {
            waitpid(pid, nullptr, 0);
        }
    }
}

/**
 * Method to Iterate over Population and Calculate Fitness
 * after Simulation.
 * 
 * @param population Vector of Individuals
 */
void cal_PopFitness(vector<Individual> &population) {
    for(Individual& i : population) {
        i.fitness = i.cal_fitness();
        // Print each Individual's Fitness
        printf("%8s%20s%13s%5.3f\n", "Ruleset: ", decode(i.chromosome).c_str(), "Fitness: ", i.fitness);
    }
}

/**
 * Main method to drive GA
 * 
 * @return int exit code (0 if successful)
 */
int main() {
    xml_document<> doc;
	xml_node<> * root_node;
	// Read the xml file into a vector
	ifstream theFile ("config.xml");
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);
	// Find our root node
	root_node = doc.first_node("System");
    timeElapsed = atoi(root_node->first_node("CellAutomata")->first_node("TimeElapsed")->value());
    populationSize = atoi(root_node->first_node("GeneticAlgo")->first_node("PopulationSize")->value());
    elitismPercent = atoi(root_node->first_node("GeneticAlgo")->first_node("ElitismPerc")->value());
    crossoverRate = atoi(root_node->first_node("GeneticAlgo")->first_node("CrossoverRate")->value());
    mutationRate = atoi(root_node->first_node("GeneticAlgo")->first_node("MutationRate")->value());
    // Create initial population
    srand((unsigned)(time(0))); 
    string rules;
    vector<Individual> population; 
    bool found = false;
    generatePatterns(true); // Reset XML
    for(int i = 0;i<populationSize;i++) { 
        string gnome = create_gnome(); 
        population.push_back(Individual(gnome)); 
    } 
    // Until target is found, crossover and mutate individuals
    while(!found) {
        toFile(population, generation);
        generatePatterns(false);
        cal_PopFitness(population);
        sort(population.begin(), population.end());
        // Converge after five Generations
        if(generation == 5) { 
            found = true; 
            break; 
        } 
        vector<Individual> new_generation; 
        // Send the top percentage through to the next generation with no
        // crossover or mutation
        int s = (elitismPercent*populationSize)/100; 
        for(int i = 0;i<s;i++) {
            new_generation.push_back(population[i]); 
        }

        // Crossover and mutate the rest of the population
        s = ((100-elitismPercent)*populationSize)/100; 
        int c = (crossoverRate*populationSize)/100;
        for(int i = 0;i<s;i++) { 
            int len = population.size(); 
            // Choose random parent from top percentage of performers
            int r = random_num(0, c); 
            Individual parent1 = population[r]; 
            r = random_num(0, c); 
            Individual parent2 = population[r]; 
            Individual offspring = parent1.mate(parent2); 
            new_generation.push_back(offspring);  
        } 
        population = new_generation;
        // Print Top Performer
        rules = decode(population[0].chromosome);
        printf("%8s%20s%13s%5.3f%16s%d\n\n", "Ruleset: ", rules.c_str(), "Fitness: ", 
            population[0].fitness, "Generation: ", generation);
        generation++; 
    }
    // Print Top Performer of Last Generation
    rules = decode(population[0].chromosome);
    printf("%8s%20s%13s%5.3f%16s%d\n", "Ruleset: ", rules.c_str(), "Fitness: ", 
        population[0].fitness, "Generation: ", generation);
    dump();
}
