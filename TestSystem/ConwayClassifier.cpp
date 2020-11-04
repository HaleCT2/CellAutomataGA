#ifndef CONWAY_CLASSIFIER_CPP
#define CONWAY_CLASSIFIER_CPP

#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <fstream>
#include <cstdlib>
#include <ctype.h>
#include <filesystem>
#include "ConwayClassifier.h"

ConwayClassifier::ConwayClassifier(const std::string& dataDirPath,
        const int genNum) {
    this->classNum = 5; // initialize classNum
    // Check and see if # of files in data path is less than genNum. If so
    // set classNum to 1 and then skip the following method calls.
    // Instead, initialize the instance variables so the API is fulfilled.
    ConwayClassifier::checkForClass1(dataDirPath, genNum);
    if (this->classNum != 1) {
        std::vector<std::ifstream*> fileStreams = // to be used to check hdrs
                populateIStreamVec(dataDirPath, genNum);
        // initialize rest of vars inside calcBoardSpecs and fillBoard
        calcBoardSpecs(fileStreams);
        // with necessary data grabbed now can initialize the array
        initializeGameBoard(genNum);
        fillBoard(fileStreams);
    }
    else {
        this->x = 0;
        this->y = 0;
        this->width = 0;
        this->height = 0;
        this->gameBoard = NULL;
        this->boardSize = 0;
    }
}

ConwayClassifier::~ConwayClassifier() {
    // need to deallocate array
    free(this->gameBoard);
}

void ConwayClassifier::checkForClass1(const std::string& dataDirPath, 
        const int genNum) {
    auto dirIter = std::filesystem::directory_iterator(dataDirPath);
    int fileCount = 0;

    for (auto& entry : dirIter) {
        if (entry.is_regular_file()) {
        ++fileCount;
        }
    }
    if (fileCount != (genNum + 1)) {
        this->classNum = 1;
    }
}

std::vector<std::ifstream*>
ConwayClassifier::populateIStreamVec(const std::string& dataPath,
        const int genNum) const {
    std::vector<std::ifstream*> rVec;
    for (int i = 0; i <= genNum; i++) {
        std::string path = dataPath + "/" + dataPath + "_" +
                std::to_string(i) + ".rle";
        std::ifstream *is = new std::ifstream(path);
        rVec.push_back(is);
    }
    return rVec;
}

// I wish this method wasn't so chonky but idk how to shorten it and still
// have it work efficiently
void ConwayClassifier::calcBoardSpecs(std::vector<std::ifstream*>& dataFiles) {
    int minX, maxX, minY, maxY;
    bool firstFile = true;
    for (auto is : dataFiles) {
        // for each file get first and second line and read in necessary data
        std::string firstLine, secLine;
        std::getline(*is, firstLine);
        std::getline(*is, secLine);
        // calculate relative mins for given generation
        std::pair<int, int> minInfo = ConwayClassifier::readPos(firstLine);
        int tempMinX = minInfo.first;
        int tempMinY = minInfo.second;
        std::pair<int, int> maxPr = ConwayClassifier::readWidthHeight(secLine);
        int tempMaxX = tempMinX + maxPr.first;
        int tempMaxY = tempMinY + maxPr.second;
        std::pair<int, int> xSpecs(tempMinX, tempMaxX);
        std::pair<int, int> ySpecs(tempMinY, tempMaxY);
        this->minMaxX.push_back(xSpecs);
        this->minMaxY.push_back(ySpecs);
        if (firstFile) {
            // set max and min vars 
            minX = tempMinX;
            minY = tempMinY;
            maxX = tempMaxX;
            maxY = tempMaxY;
            firstFile = false;
        } else {
            // update max and min vars as necessary
            if (tempMinX < minX)
                minX = tempMinX;
            if (tempMinY < minY)
                minY = tempMinY;
            if (tempMaxX > maxX)
                maxX = tempMaxX;
            if (tempMaxY > maxY)
                maxY = tempMaxY;
        }
        is->seekg(0, std::ios::beg); // reset ifstream for next use
    }
    this->x = minX;
    this->y = minY;
    this->width = maxX - minX;
    this->height = maxY - minY;
}

std::pair<int, int>
ConwayClassifier::readPos(const std::string& firstLine) const {
    std::istringstream posStream(firstLine);
    std::string posStr;
    posStream >> posStr >> posStr;
    posStr = posStr.substr(this->posQualifierLen, posStr.length()
            - this->posQualifierLen);
    std::pair<int, int> posPair(
            std::stoi(ConwayClassifier::split(posStr, ",", true)),
            std::stoi(ConwayClassifier::split(posStr, ",", false)));
    return posPair;
}

std::pair<int, int>
ConwayClassifier::readWidthHeight(const std::string& secLine) const {
    std::istringstream lineStream(secLine);
    std::string xInfo, yInfo;
    lineStream >> xInfo >> xInfo >> xInfo >> yInfo >> yInfo >> yInfo;
    std::pair<int, int> WHPair(std::stoi(ConwayClassifier::split(xInfo,
            ",", true)), std::stoi(ConwayClassifier::split(yInfo,
            ",", true)));
    return WHPair;
}

std::string ConwayClassifier::split(const std::string& str,
        const std::string& delimiter, const bool firstStr) const {
    auto index = str.find(delimiter);
    if (index != std::string::npos) {
        if (firstStr && index != 0)
            return str.substr(0, index);
        else if (index != str.length() - 1)
            return str.substr(index + 1, str.length() - index - 1);
    }
    return "";
}

// ToDo change logic so iterates directly through 1d gameBoard without using
// set cell as set cell is rather inefficient (too much calculation each time
// when you should just be increasing index by 1 for every new char read)

void ConwayClassifier::fillBoard(std::vector<std::ifstream*>& dataFiles) {
    int genNum = 0;
    for (auto is : dataFiles) {
        // get necessary info from header
        std::string firstLine, secLine;
        std::getline(*is, firstLine);
        std::getline(*is, secLine); // read line to toss it out, its not needed
        std::pair<int, int> posInfo = ConwayClassifier::readPos(firstLine);
        // top left coords of this gen
        int currentX = posInfo.first;
        int currentY = posInfo.second;
        // read file char by char now that headers have been processed
        char c;
        while (is->get(c)) {
            if (firstLine == "")  // ToDo: figure out what this is for
                std::cout << c << " ";
            int repCount = 1;
            if (std::isdigit(c)) {
                std::string repStr = "";
                // on last iteration through loop c becomes the char to be repeated
                do {
                    repStr += c;
                    is->get(c);
                } while (std::isdigit(c));
                repCount = std::stoi(repStr);
            }
            for (int i = 0; i < repCount; i++) {
                if (c == 'o' || c == 'b') { // live/dead cell
                    if (c == 'o') { // set cell alive
                        ConwayClassifier::setCellVal(genNum, currentX,
                                currentY, true);
                    }
                    // don't need to set cell dead as all cells initialized dead
                    // increment currentX and currentY correctly
                    currentX++;
                } else if (c == '$') { // new row
                    // set x value back to original and increment y value
                    currentX = posInfo.first;
                    currentY++;
                }
                // else c == ! and file is over
            }
        }
        // now done with ifstream object so close it
        is->close();
        genNum++;
    }
}

int ConwayClassifier::get1DIndex(const int gen, const int xCoord,
        const int yCoord) const {
    // since the 2d game board for each gen does not necessarily have the 
    // top left corner set relative to the origin so we adjust the x and y 
    // coords so that they are relative to the x and y instance vars 
    // (translation dawg)
    int newX = xCoord - this->x;
    int newY = yCoord - this->y;
    int retVal = (gen * this->width * this->height)
            + (newY * this->width) + newX;
    // ToDo throw exception if retVal is negative
    return retVal;
}

unsigned short int ConwayClassifier::classification() const {
    if (this->classNum == 5) {
        // determine class #
        // ToDo
    }
    // else just return already calculated class
    return this->classNum;
}

std::pair<int, int> ConwayClassifier::getCoords() const {
    std::pair<int, int> coords(this->x, this->y);
    return coords;
}

std::pair<int, int> ConwayClassifier::getDimensions() const {
    std::pair<int, int> dimensions(this->width, this->height);
    return dimensions;
}

bool ConwayClassifier::getCellVal(const int gen, const int xCoord,
        const int yCoord) const {
    // error handling if xCoord or yCoord is less than this->x/y?
    return this->gameBoard[ConwayClassifier::get1DIndex(gen, xCoord, yCoord)];
}

// this method copies the data from the instance variable to the pair object
// to be returned as to not allow for a memory leak
std::pair<int, int> 
ConwayClassifier::getMinMax(const int gen, const bool giveXCoords) const {
    if (giveXCoords) {
        std::pair<int, int> rPair(this->minMaxX[gen]);
        return rPair;
    }
    else {
        std::pair<int, int> rPair(this->minMaxY[gen]);
        return rPair;
    }
}

void ConwayClassifier::setCellVal(const int gen, const int xCoord,
        const int yCoord, const bool val) {
    this->gameBoard[ConwayClassifier::get1DIndex(gen, xCoord, yCoord)] = val;
}

void ConwayClassifier::initializeGameBoard(const int genNum) {
    // genNum has 1 added to it because we need the initial layout in addition
    // to the specified number of generations
    this->boardSize = (genNum + 1) * this->width * this->height;
    this->gameBoard = (bool*) malloc(this->boardSize * sizeof (bool));
    for (int i = 0; i < boardSize; i++) {
        this->gameBoard[i] = false;
    }
}

void ConwayClassifier::printGameBoard(const int genNum, std::ostream& os,
        const char onChar, const char offChar) const {
    // get index of beginning of gen by giving top 
    // left corner coords to get1DIndex
    int index = ConwayClassifier::get1DIndex(genNum, this->x, this->y);
    int genLen = this->width * this->height;
    // go until next gen
    for (int i = index; i < index + genLen; i++) {
        if (this->gameBoard[i])
            os << onChar;
        else
            os << offChar;
        // new row
        if (i - index + 1 % this->width == 0)
            os << std::endl;
    }
    os << std::endl;
}

#endif /* CONWAY_CLASSIFIER_CPP */
