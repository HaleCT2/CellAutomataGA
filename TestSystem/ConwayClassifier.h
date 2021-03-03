/* 
 * File:   conwayProcessor.h
 * Author: Eric Schonauer
 *
 * Created on September 17, 2020, 8:21 PM
 */

#ifndef CONWAY_CLASSIFIER_H
#define CONWAY_CLASSIFIER_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>

class ConwayClassifier {
public:
    // constructor
    // takes path to data directory and number of gens run as well as the max
    // number of threads allowed
    ConwayClassifier(const std::string& dataDirPath, const int genNum, 
            const int maxThrNum);

    // destructor to deallocate
    ~ConwayClassifier();

    /*
     * return classification num which is initialized to 3 and has the 
     * chance to be set to either 1 or 2 in constructor as necessary
     */
    unsigned short int classification();

    // returns x and y coordinates of top left corner as a pair
    std::pair<int, int> getCoords() const;

    // returns width and height as a pair
    std::pair<int, int> getDimensions() const;
    
    // returns number of generations, which means the max generation number
    // plus one due to the starting condition, generation 0
    int getGenNum() const;

    // returns min and max x-coords as pair for given gen if giveXCoords is true
    // else returns min and max y-coords as pair for given gen
    std::pair<int, int> getMinMax(const int gen, const bool giveXCoords) const;

    // returns value of given cell
    bool getCellVal(const int gen, const int xCoord, const int yCoord) const;

    // returns the rule of the given data as a string ex:b234_s67
    std::string getRule() const;

    // prints a given generation of the gameBoard to the given output stream
    void printGameBoard(const int genNum, std::ostream& os = std::cout,
            const char onChar = '1', const char offChar = '0') const;

private:
    std::string rule; // the rule of the given data as a string ex:b234_s67
    // in golly x is positive to the right and y is positive going DOWN NOT UP
    int generationCount; // number of generations specified plus one for the
    // starting conditions
    int x; // x-coordinate of top left corner
    int y; // y-coordinate of top left corner
    int width; // width measures length of board from x to right end of board
    int height; // height measures length of board from y to bottom end
    // represents classification num of this rule. Anything greater than class
    // 2 will be assigned a value of 3.
    unsigned short int classNum; 
    // the gameboard will be resized such that it covers the max range of the
    // game
    /*
     * IMPORTANT GAMEBOARD NOTE: when "pressing" 3d game board into 1d
     * it is done by generation, then y-coord, then x-coord.
     * This means that when you go one element over in the array (assuming that
     * you don't cross a boundary between gen's or differing rows) you are 
     * increasing the x-coord value by one. Another way to think of this is
     * that the data was "pressed" row-wise, reading down the 0th row
     * then increasing the y-coord by one and reading down the 1st row and
     * so on.
     */
    bool* gameBoard; // 1d array to represent 3d board for speed
    long long int boardSize; // size of the gameBoard array
    const int posQualifierLen = 4; // "pos=" length in rle header
    // saves the min and the max x-coord for every gen
    std::vector<std::pair<int, int>> minMaxX;
    // saves the min and the max y-coord for every gen
    std::vector<std::pair<int, int>> minMaxY;
    
    // this is used to set relevant instance variables to 0/null if
    // class is determined to be 1/2 before classification method is called
    void voidInstanceVars();

    // return the string containing the rule that has been extracted from a
    // larger path
    std::string extractRule(const std::string& rleDataDirPath) const;

    // checks to see if number of files in directory specified by dataDirPath
    // is less than the genNum given. If so, it sets the classNum instance
    // variable to 1.
    void checkForClass1(const std::string& dataDirPath, const int genNum);
    
    // adds encoded pattern in each rle file to hash map to check for repeat
    // patterns. If it is class 2, the classNum variable will be set to 2
    void checkForClass2(std::vector<std::ifstream*>& dataFiles);

    // takes vector of ifstream objects to figure out coords and dimensions
    // when finished resets ifstreams
    // sets the x, y, width and height vars and dynamically resizes gameBoard
    void calcBoardSpecs(std::vector<std::ifstream*>& dataFiles);

    // with the board specs calculated fill gameBoard array with data from files
    // by calling fillGen for every ifstream
    void fillBoard(std::vector<std::ifstream*>& dataFiles, const int maxThrNum);

    // reads files corresponding to genStartNum through (and including) 
    // genEndNum and fills the gameBoard accordingly. Once done with a given
    // ifstream object it closes it.
    void fillGen(std::vector<std::ifstream*>& dataStreams,
            const int genStartNum, const int genEndNum);

    // takes path to the data directory and creates ifstream object for every
    // file and adds its address (pointer) to a vector; then returns that vector
    std::vector<std::ifstream*> populateIStreamVec(const std::string& dataPath,
            const int genNum) const;

    // deallocate all the ifstreams that are allocated in the populateIstreamVec
    // method
    void deallocateIfstreams(std::vector<std::ifstream*>& streamsToClose) const;

    // splits a string based on the given delimiter, returns the before the
    // delimiter if firstStr parameter is true, returns after the delimiter
    // if firstStr is false
    // returns empty string if no split can be made
    // used to help parse .rle headers 
    std::string split(const std::string& str, const std::string& delimiter,
            const bool firstStr) const;

    // takes what would be the 3 values needed to get a value of a cell in 
    // Conway's game and calculates at what 1D index that cell data is stored
    // in the gameBoard instance variable
    long long int get1DIndex(const int gen, const int xCoord,
            const int yCoord) const;

    // sets a given cell to a given value
    void setCellVal(const int gen, const int xCoord, const int yCoord,
            const bool val);

    // allocates memory for the gameBoard instance var and sets every elt to 0
    void initializeGameBoard(const int genNum);

    // takes the first line of an rle file and extracts the x and y values
    // for the position of the upper right corner of the data
    // return the x and y values as pair in that order
    std::pair<int, int> readPos(const std::string& firstLine) const;

    // takes the second line of an rle file as a string and extracts the 
    // width and height data ("x =" and "y =" in the rle) and returns
    // the data as a pair
    std::pair<int, int> readWidthHeight(const std::string& secLine) const;
};



#endif /* CONWAY_CLASSIFIER_H */

