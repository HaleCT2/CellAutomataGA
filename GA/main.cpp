/*
 * File:   main.cpp
 * Author: Owen Hichens
 * Based on Genetic Algorithm found at
 * https://www.geeksforgeeks.org/genetic-algorithms/
 *
 * Created on October 14, 2020, 3:03 PM
 */

#include <bits/stdc++.h>

using namespace std;

// Define population size
#define POPULATION_SIZE 10

// Possible characters to make up genome. Note that the genome is a binary
// number represented as a string for simplicity.
const string GENES = "01";

// Target string. Set to "b2s23" to find Conway's Game of Life
const string TARGET_RULE_SET = "b2s23";

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
 * Decodes binary strings into golly rule sets
 *
 * @param chromosome: binary string
 * @return string golly rule set
 */
string binToStr(string chromosome) {
    string ruleSet = "";
    ruleSet += "b";
    for(int i = 0; i <= 9; i++) {
        if (chromosome[i] == '1') {
            ruleSet += to_string(i);
        }
    }
    ruleSet += "s";
    for(int i = 10; i <= 19; i++) {
        if (chromosome[i] == '1') {
            ruleSet += to_string(i - 10);
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
string strToBin(string ruleSet) {
    string chromosome = "00000000000000000000";
    bool survive = false;
    for(int i = 0; i < ruleSet.length(); i++) {
        if (ruleSet[i] != 'b' && ruleSet[i] != 's') {
            int index = ruleSet[i] - '0';
            if (survive) {
                index += 10;
            }
            chromosome[index] = '1';
        } else if (ruleSet[i] == 's') {
            survive = true;
        }
    }
    return chromosome;
}

// Have to define this target after strToBin is defined, but still global
const string TARGET = strToBin(TARGET_RULE_SET);

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
    int len = TARGET.size();
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
    int fitness;
    Individual(string chromosome);
    Individual mate(Individual parent2);
    int cal_fitness();
};

/**
 * Constructor that takes in a chromosome
 *
 * @param chromosome chromosome to be associated with individual
 */
Individual::Individual(string chromosome) {
    this->chromosome = chromosome;
    fitness = cal_fitness();
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
    // For each character, if random number is less than 0.45, use parent 1's
    // value, if the random number is greater than 0.45 but less than 0.9, use
    // parent 2's value, and if it is greater than 0.9, mutate the gene (random
    // value).
    for(int i = 0;i<len;i++) {
        float p = random_num(0, 100)/100.;
        if(p < 0.45) {
            child_chromosome += chromosome[i];
        } else if(p < 0.90) {
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
int Individual::cal_fitness() {
    int len = TARGET.size();
    int fitness = 0;
    // Fitness based on how many correct characters there are
    for(int i = 0;i<len;i++) {
        if(chromosome[i] != TARGET[i]) {
            fitness++;
        }
    }
    return fitness;
};

/**
 * Operator override for sort function allowing the sort function to apply
 * to our custom Individual class
 *
 * @param ind1: First individual
 * @param ind2: Second individual
 * @return bool true if ind1's fitness number is less than ind2's fitness
 *         number, false otherwise
 */
bool operator<(const Individual &ind1, const Individual &ind2) {
    return ind1.fitness < ind2.fitness;
}

/**
 * Main method to drive GA
 *
 * @return int exit code (0 if successful)
 */
int main() {
    // Create initial population
    srand((unsigned)(time(0)));
    string rules;
    int generation = 0;
    vector<Individual> population;
    bool found = false;
    for(int i = 0;i<POPULATION_SIZE;i++) {
        string gnome = create_gnome();
        population.push_back(Individual(gnome));
    }
    // Until target is found, crossover and mutate individuals
    while(! found) {
        sort(population.begin(), population.end());
        if(population[0].fitness <= 0) {
            found = true;
            break;
        }
        vector<Individual> new_generation;
        // Send the top 10% of the through to the next generation with no
        // crossover or mutation
        int s = (10*POPULATION_SIZE)/100;
        for(int i = 0;i<s;i++) {
            new_generation.push_back(population[i]);
        }
        // Crossover and mutate the rest of the population
        s = (90*POPULATION_SIZE)/100;
        for(int i = 0;i<s;i++) {
            int len = population.size();
            // Choose random parent from top half of performers
            int r = random_num(0, POPULATION_SIZE/2);
            Individual parent1 = population[r];
            r = random_num(0, POPULATION_SIZE/2);
            Individual parent2 = population[r];
            Individual offspring = parent1.mate(parent2);
            new_generation.push_back(offspring);
        }
        population = new_generation;
        cout<< "Generation: " << generation << "\t";
        rules = binToStr(population[0].chromosome);
        if (rules.length() <= 21) {
            rules += "\t";
        }
        if (rules.length() <= 14) {
            rules += "\t";
        }
        if (rules.length() <= 7) {
            rules += "\t";
        }
        cout<< "Rule Set: "<< rules << "\t";
        cout<< "Fitness: "<< population[0].fitness << "\n";
        generation++;
    }
    rules = binToStr(population[0].chromosome);
    if (rules.length() <= 21) {
        rules += "\t";
    }
    if (rules.length() <= 13) {
        rules += "\t";
    }
    if (rules.length() <= 7) {
        rules += "\t";
    }
    cout << "Generation: " << generation << "\t";
    cout << "Rule Set: "<< rules << "\t";
    cout << "Fitness: "<< population[0].fitness << "\n";
}