#ifndef CONWAY_CLASSIFIER_CPP
#define CONWAY_CLASSIFIER_CPP

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <filesystem>
#include <thread>
#include <ctype.h>
#include "ConwayClassifier.h"

ConwayClassifier::ConwayClassifier(const std::string& dataDirPath,
        const int genNum, const int maxThrNum, const int endCalcPercent) {
    this->generationCount = genNum + 1;
    int statCalcLength = (int) (((double) this->generationCount / 100)
            * (double) endCalcPercent);
    this->statStartGen = this->generationCount - statCalcLength;
    this->rule = this->extractRule(dataDirPath);
    this->classNum = 3; // initialize classNum
    // Check and see if # of files in data path is less than genNum. If so
    // set classNum to 1 and then skip the following method calls.
    // Instead, initialize the instance variables so the API is fulfilled.
    this->checkForClass1(dataDirPath, genNum);
    if (this->classNum != 1) {
        std::vector<std::ifstream*> fileStreams = // to be used to check hdrs
                populateIStreamVec(dataDirPath, genNum);
        this->checkForClass2(fileStreams);
        if (this->classNum != 2) {
            // initialize rest of vars inside calcBoardSpecs and fillBoard
            calcBoardSpecs(fileStreams);
            // with necessary data grabbed now can initialize the array
            // and other data structures
            initializeGameBoard(genNum);
            fillBoard(fileStreams, maxThrNum);
            finishStats();
        } else
            this->voidInstanceVars();
        // now done with file streams so deallocate them
        this->deallocateIfstreams(fileStreams);
    } else
        this->voidInstanceVars();
}

ConwayClassifier::~ConwayClassifier() {
    // need to deallocate array
    delete[] this->gameBoard;
}

void ConwayClassifier::voidInstanceVars() {
    this->x = 0;
    this->y = 0;
    this->width = 0;
    this->height = 0;
    this->gameBoard = NULL;
    this->boardSize = 0;
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

void ConwayClassifier::checkForClass2(std::vector<std::ifstream*>& dataFiles) {
    std::unordered_map<std::string, int>
            patternMap = std::unordered_map<std::string, int>();
    std::string unneededHeader, data, pattern;
    int genNum = 0;
    for (auto& is : dataFiles) {
        std::getline(*is, unneededHeader);
        std::getline(*is, unneededHeader); // toss out headers
        data = "";
        pattern = "";
        while ((*is) >> data) { // get all of encoded pattern
            pattern += data;
        }
        // check if its in the hash map
        // if it isn't, add it
        if (patternMap.find(pattern) == std::end(patternMap)) {
            patternMap[pattern] = genNum;
        }// if it is set classNum to 2 and exit loop and method as it is no
            // longer necessary to search more
        else {
            this->classNum = 2;
            break;
        }
        is->clear();
        is->seekg(0, std::ios::beg); // reset ifstream for next use
        genNum++;
    }
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
    for (auto& is : dataFiles) {
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
        is->clear();
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
                        this->setAliveCount(genNum);
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

void ConwayClassifier::setAliveCount(const int genNum) {
    if (genNum >= this->statStartGen)
        this->aliveCellRatio[genNum - this->statStartGen] += 1;
}

void ConwayClassifier::finishStats() {
    calculateAliveCellRatio();
    calculatePercentChange();
    calculateActiveCellRatio();
}

void ConwayClassifier::calculateAliveCellRatio() {
    // turn counts into ratios by dividing number of alive cells by the area of
    // the board
    for (int gen = this->statStartGen; gen < this->generationCount; gen++) {
        int width = abs(this->minMaxX[gen].second - this->minMaxX[gen].first);
        int height = abs(this->minMaxY[gen].second - this->minMaxY[gen].first);
        this->aliveCellRatio[gen - this->statStartGen] =
                this->aliveCellRatio[gen - this->statStartGen]
                / (width * height);
    }
}

void ConwayClassifier::calculatePercentChange() {
    // since calculating stats for generation n requires the previous gen 
    // (n - 1), need to start from statStartGen - 1 and then stop at
    // generationCount - 2 since you would then be looking at generationCount-1
    // and that is the maximum generation index
    for (int gen = this->statStartGen - 1; gen <= this->generationCount - 2; gen++) {
        int changeCount = 0;
        for (int y = this->y; y < this->y + this->height; y++) {
            for (int x = this->x; x < this->x + this->width; x++) {
                if (this->getCellVal(gen, x, y) != this->getCellVal(gen + 1, x, y))
                    changeCount++;
            }
        }
        // add one since i actually refers to gen n - 1 when calculating
        // percent change for generation n
        int width = abs(this->minMaxX[gen + 1].second
                - this->minMaxX[gen + 1].first);
        int height = abs(this->minMaxY[gen + 1].second
                - this->minMaxY[gen + 1].first);
        this->percentChange[gen + 1 - this->statStartGen] =
                (double) changeCount / (width * height);
    }
}

void ConwayClassifier::calculateActiveCellRatio() {
    for (int gen = this->statStartGen; gen < this->generationCount; gen++) {
        int activeCellCount = 0;
        for (int y = this->y; y < this->y + this->height; y++) {
            for (int x = this->x; x < this->x + this->width; x++) {
                if (cellAliveRecently(gen, x, y) && cellDeadRecently(gen, x, y))
                    activeCellCount++;
            }
        }
        int width = abs(this->minMaxX[gen].second - this->minMaxX[gen].first);
        int height = abs(this->minMaxY[gen].second - this->minMaxY[gen].first);
        this->activeCellRatio[gen - this->statStartGen] =
                (double) activeCellCount / (width * height);
    }
}

bool ConwayClassifier::cellAliveRecently(
        const int gen, const int x, const int y) const {
    // generation asked for is too early to check for being alive recently so 
    // by given definition, this method will return false
    if (gen < this->consecutiveAliveLen)
        return false;
    for (int i = gen - this->consecutiveAliveLen; i <= gen; i++) {
        if (!this->getCellVal(i, x, y))
            return false;
    }
    return true;
}

bool ConwayClassifier::cellDeadRecently(
        const int gen, const int x, const int y) const {
    int startGen = startGen = gen - this->deadWithinLen;
    // just in case generation requested is less than deadWithinLen
    if (gen < this->deadWithinLen)
        startGen = 0;
    for (int i = startGen; i < gen; i++) {
        if (!this->getCellVal(i, x, y))
            return true;
    }
    return false;
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

double ConwayClassifier::getAliveCellRatio(const int genNum) const {
    if (genNum == -1) { // return average alive cell ratio
        return averageVector(this->aliveCellRatio);
    } else {
        if (genNum >= this->statStartGen && genNum < this->generationCount) {
            return this->aliveCellRatio[genNum - this->statStartGen];
        } else return -1;
    }
}

double ConwayClassifier::getPercentChange(const int genNum) const {
    if (genNum == -1) { // return average percent change
        return averageVector(this->percentChange);
    } else {
        if (genNum >= this->statStartGen && genNum < this->generationCount) {
            return this->percentChange[genNum - this->statStartGen];
        } else return -1;
    }
}

double ConwayClassifier::getActiveCellRatio(const int genNum) const {
    if (genNum == -1) { // return average
        return averageVector(this->activeCellRatio);
    } else {
        if (genNum >= this->statStartGen && genNum < this->generationCount) {
            return this->activeCellRatio[genNum - this->statStartGen];
        } else return -1;
    }
}

double ConwayClassifier::getExpansionRateX(const int genNum) const {
    // can't get expansion rate on these gens
    if (genNum == 0 || genNum >= this->generationCount)
        return -1;
    if (genNum == -1) { // return average
        std::vector<double> expansionRates(this->generationCount
                - this->statStartGen);
        for (int i = this->statStartGen; i < this->generationCount; i++) {
            expansionRates[i - this->statStartGen] = getExpansionRateX(i);
        }
        return averageVector(expansionRates);
    }
    return (double) (this->minMaxX[genNum].second
            - this->minMaxX[genNum].first)
            / (double) (this->minMaxX[genNum - 1].second
            - this->minMaxX[genNum - 1].first);
}

double ConwayClassifier::getExpansionRateY(const int genNum) const {
    // can't get expansion rate on these gens
    if (genNum == 0 || genNum >= this->generationCount)
        return -1;
    if (genNum == -1) { // return average
        std::vector<double> expansionRates(this->generationCount
                - this->statStartGen);
        for (int i = this->statStartGen; i < this->generationCount; i++) {
            expansionRates[i - this->statStartGen] = getExpansionRateY(i);
        }
        return averageVector(expansionRates);
    }
    return (double) (this->minMaxY[genNum].second
            - this->minMaxY[genNum].first)
            / (double) (this->minMaxY[genNum - 1].second
            - this->minMaxY[genNum - 1].first);
}

double ConwayClassifier::getExpansionRateArea(const int genNum) const {
    // can't get expansion rate on these gens
    if (genNum == 0 || genNum >= this->generationCount)
        return -1;
    if (genNum == -1) { // return average
        std::vector<double> expansionRates(this->generationCount
                - this->statStartGen);
        for (int i = this->statStartGen; i < this->generationCount; i++) {
            expansionRates[i - this->statStartGen] = getExpansionRateArea(i);
        }
        return averageVector(expansionRates);
    }
    return getExpansionRateX(genNum) * getExpansionRateY(genNum);
}

double
ConwayClassifier::averageVector(const std::vector<double>& statVec) const {
    double sum = 0;
    for (auto& element : statVec) {
        sum += element;
    }
    return (sum / statVec.size());
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
    // now set this->aliveCellRatio to correct length
    this->aliveCellRatio.resize(this->generationCount - this->statStartGen);
    // set this->percentChange to correct length as well
    this->percentChange.resize(this->generationCount - this->statStartGen);
    // same here :)
    this->activeCellRatio.resize(this->generationCount - this->statStartGen);
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
        if ((i - (index + 1)) % this->width == 0)
            os << std::endl;
    }
    os << std::endl;
}

#endif /* CONWAY_CLASSIFIER_CPP */
