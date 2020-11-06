#ifndef CONWAY_CLASSIFIER_CPP
#define CONWAY_CLASSIFIER_CPP

#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <fstream>
#include <filesystem>
#include <thread>
#include <ctype.h>
#include "ConwayClassifier.h"

ConwayClassifier::ConwayClassifier(const std::string& dataDirPath,
        const int genNum, const int maxThrNum) {
    this->generationCount = genNum + 1;
    this->rule = this->extractRule(dataDirPath);
    this->classNum = 5; // initialize classNum
    // Check and see if # of files in data path is less than genNum. If so
    // set classNum to 1 and then skip the following method calls.
    // Instead, initialize the instance variables so the API is fulfilled.
    this->checkForClass1(dataDirPath, genNum);
    if (this->classNum != 1) {
        std::vector<std::ifstream*> fileStreams = // to be used to check hdrs
                populateIStreamVec(dataDirPath, genNum);
        // initialize rest of vars inside calcBoardSpecs and fillBoard
        calcBoardSpecs(fileStreams);
        // with necessary data grabbed now can initialize the array
        initializeGameBoard(genNum);
        fillBoard(fileStreams, maxThrNum);
        // now done with file streams so deallocate them
        this->deallocateIfstreams(fileStreams);
    } else {
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
    delete[] this->gameBoard;
}

std::string
ConwayClassifier::extractRule(const std::string& rleDataDirPath) const {
    // get index of last '/'
    int index = rleDataDirPath.length() - 1;
    while (rleDataDirPath.at(index) != '/') {
        index--;
    }
    // now take substring from there to the end
    return rleDataDirPath.substr(index + 1,
            rleDataDirPath.length() - index - 1);
}

void ConwayClassifier::checkForClass1(const std::string& dataDirPath,
        const int genNum) {
    // works but not in linux so I have it commented out
    //    auto dirIter = std::filesystem::directory_iterator(dataDirPath);
    //    int fileCount = 0;
    //
    //    for (auto& entry : dirIter) {
    //        if (entry.is_regular_file()) {
    //            ++fileCount;
    //        }
    //    }
    //    if (fileCount != (genNum + 1)) {
    //        this->classNum = 1;
    //    }
}

std::vector<std::ifstream*>
ConwayClassifier::populateIStreamVec(const std::string& dataPath,
        const int genNum) const {
    std::vector<std::ifstream*> rVec;
    for (int i = 0; i <= genNum; i++) {
        std::string path = dataPath + "/" + this->rule + "_" +
                std::to_string(i) + ".rle";
        std::ifstream *is = new std::ifstream(path);
        rVec.push_back(is);
    }
    return rVec;
}

void ConwayClassifier::deallocateIfstreams(std::vector<std::ifstream*>&
        streamsToClose) const {
    for (int i = 0; i < streamsToClose.size(); i++) {
        delete streamsToClose[i];
    }
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
        std::pair<int, int> minInfo = this->readPos(firstLine);
        int tempMinX = minInfo.first;
        int tempMinY = minInfo.second;
        std::pair<int, int> maxPr = this->readWidthHeight(secLine);
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
            std::stoi(this->split(posStr, ",", true)),
            std::stoi(this->split(posStr, ",", false)));
    return posPair;
}

std::pair<int, int>
ConwayClassifier::readWidthHeight(const std::string& secLine) const {
    std::istringstream lineStream(secLine);
    std::string xInfo, yInfo;
    lineStream >> xInfo >> xInfo >> xInfo >> yInfo >> yInfo >> yInfo;
    std::pair<int, int> WHPair(std::stoi(this->split(xInfo,
            ",", true)), std::stoi(this->split(yInfo,
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

// I don't know how much of a difference this (^^^) will make. Also, to
// implementing this requires more that just incrementing the index for every
// char read as the board information being read it being "set into" a board
// of a larger size that is padded with 0's.

void ConwayClassifier::fillBoard(std::vector<std::ifstream*>& dataFiles,
        const int maxThrNum) {
    int genWidth = this->generationCount / maxThrNum;
    int extraWidth = this->generationCount % genWidth;
    int lastThrStartIndex = this->generationCount - (genWidth + extraWidth);
    std::vector<std::thread> threadList;
    for (int i = 0; i < lastThrStartIndex; i += genWidth) {
        threadList.push_back(std::thread(&ConwayClassifier::fillGen, this,
                std::ref(dataFiles), i, i + genWidth - 1));
    }
    threadList.push_back(std::thread(&ConwayClassifier::fillGen, this,
            std::ref(dataFiles), lastThrStartIndex, this->generationCount - 1));
    for (auto& thr : threadList) {
        thr.join();
    }
}

void ConwayClassifier::fillGen(std::vector<std::ifstream*>& dataStreams,
        const int genStartNum, const int genEndNum) {
    int genNum = genStartNum;
    while (genNum <= genEndNum) {
        std::ifstream* is = dataStreams.at(genNum);
        // get necessary info from header
        std::string firstLine, secLine;
        std::getline(*is, firstLine);
        std::getline(*is, secLine); // read line to toss it out, its not needed
        std::pair<int, int> posInfo = this->readPos(firstLine);
        // top left coords of this gen
        int currentX = posInfo.first;
        int currentY = posInfo.second;
        // read file char by char now that headers have been processed
        char c;
        while (is->get(c)) {
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
                        this->setCellVal(genNum, currentX,
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

long long int ConwayClassifier::get1DIndex(const int gen, const int xCoord,
        const int yCoord) const {
    // since the 2d game board for each gen does not necessarily have the 
    // top left corner set relative to the origin 
    // (x and y instance vars are considered 0,0) so we adjust the x and y 
    // coords so that they are relative to the x and y instance vars 
    // (translation dawg)
    long long int newX = xCoord - this->x;
    long long int newY = yCoord - this->y;
    long long int retVal = static_cast<long long> (gen *
            static_cast<long long> (this->width * this->height))
            + static_cast<long long> (newY * this->width) + newX;
    if (retVal < 0 || retVal >= this->boardSize)
        throw "Invalid coordinates resulting in out of bounds array index";
    return retVal;
}

unsigned short int ConwayClassifier::classification() {
    if (this->classNum == 5) {
        // determine class #
        // ToDo
    }
    // else just return already calculated class
    return this->classNum;
}

std::string ConwayClassifier::getRule() const {
    return this->rule;
}

std::pair<int, int> ConwayClassifier::getCoords() const {
    std::pair<int, int> coords(this->x, this->y);
    return coords;
}

int ConwayClassifier::getGenNum() const {
    return this->generationCount;
}

std::pair<int, int> ConwayClassifier::getDimensions() const {
    std::pair<int, int> dimensions(this->width, this->height);
    return dimensions;
}

bool ConwayClassifier::getCellVal(const int gen, const int xCoord,
        const int yCoord) const {
    // error handling if xCoord or yCoord is less than this->x/y?
    return this->gameBoard[this->get1DIndex(gen, xCoord, yCoord)];
}

std::pair<int, int>
ConwayClassifier::getMinMax(const int gen, const bool giveXCoords) const {
    if (giveXCoords) {
        return this->minMaxX[gen];
    }
    return this->minMaxY[gen];
}

void ConwayClassifier::setCellVal(const int gen, const int xCoord,
        const int yCoord, const bool val) {
    this->gameBoard[this->get1DIndex(gen, xCoord, yCoord)] = val;
}

void ConwayClassifier::initializeGameBoard(const int genNum) {
    // genNum has 1 added to it because we need the initial layout in addition
    // to the specified number of generations
    this->boardSize = static_cast<long long> (genNum + 1) *
            static_cast<long long> (this->width) *
            static_cast<long long> (this->height);
    // dynamically allocate array of given boardSize to all false
    this->gameBoard = new bool[this->boardSize] {
        false
    };
}

void ConwayClassifier::printGameBoard(const int genNum, std::ostream& os,
        const char onChar, const char offChar) const {
    // get index of beginning of gen by giving top 
    // left corner coords to get1DIndex
    int index = this->get1DIndex(genNum, this->x, this->y);
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
