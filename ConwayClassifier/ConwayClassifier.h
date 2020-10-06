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
    // takes path to data directory and number of gens run
    ConwayClassifier(const std::string& dataDirPath, const int genNum);

    //destructor
    ~ConwayClassifier();

    /*
     * if classNum instance var is not set (=5) then calculate classification
     * and return it; if var is set because classification() has already been
     * called then just return classNum
     */
    unsigned short int classification() const;

    // returns x and y coordinates of top left corner as a pair
    std::pair<int, int> getCoords() const;

    // returns width and height as a pair
    std::pair<int, int> getDimensions() const;

    // returns value of given cell
    bool getCellVal(const int gen, const int xCoord, const int yCoord) const;

    // prints a given generation of the gameBoard to the given output stream
    void printGameBoard(const int genNum, std::ostream& os = std::cout,
            const char onChar = '1', const char offChar = '0') const;

private:
    // in golly x is positive to the right and y is positive going DOWN NOT UP
    int x; // x-coordinate of top left corner
    int y; // y-coordinate of top left corner
    int width; // width measures length of board from x to right end of board
    int height; // height measures length of board from y to bottom end
    unsigned short int classNum; // represents classification num of this rule
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
    int boardSize; // size of the gameBoard array
    const int posQualifierLen = 4;  // "pos=" length in rle header

    // takes vector of ifstream objects to figure out coords and dimensions
    // when finished resets ifstreams
    // sets the x, y, width and height vars and dynamically resizes gameBoard
    void calcBoardSpecs(std::vector<std::ifstream*>& dataFiles);

    // with the board specs calculated fill 3d array with data from files
    // then close inputStreams
    void fillBoard(std::vector<std::ifstream*>& dataFiles);

    // takes path to the data directory and creates ifstream object for every
    // file and adds its address (pointer) to a vector; then returns that vector
    std::vector<std::ifstream*> populateIStreamVec(const std::string& dataPath,
            const int genNum) const;

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
    int get1DIndex(const int gen, const int xCoord,
            const int yCoord) const;

    // sets a given cell to a given value
    void setCellVal(const int gen, const int xCoord, const int yCoord,
            const bool val);

    // malloc's the gameBoard instance var and sets every elt to 0
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

