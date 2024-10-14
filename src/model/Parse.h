#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <cassert>

#include "Event.h"

class EventIterator {
public:
    EventIterator(const std::string& filename) : file(filename), done(false) {
        if (!file.is_open()) {
            done = true; // Set done if the file couldn't be opened
        } else {
	    std::string line;
            std::getline(file, line);	
            readNextLine(); // Read the first line
        }
    }

    bool operator!=(const EventIterator& other) const {
        return !done;
    }

    const Event* operator*() const {
        //return currentRow;
	return currentEvent;
    }

    EventIterator& operator++() {
        readNextLine();
        return *this;
    }

private:
    std::ifstream file;
    std::vector<std::string> currentRow;
    Event *currentEvent = NULL;
    bool done;

    void readNextLine() {
        std::string line;
        if (std::getline(file, line)) {
            std::stringstream lineStream(line);
            std::string cell;
            currentRow.clear();

            while (std::getline(lineStream, cell, ',')) {
                currentRow.push_back(cell);
            }

            // ESH5-ESM5, 1722902701.8464227, A, B, 1, 5319000000000, 14801, 1, 5319.75, 2, 5320.75, True
            assert(currentRow.size() == 12);

            std::string ticker = currentRow[0];
	    long double time = std::stod(currentRow[1]);
            std::string action = currentRow[2];
            std::string side = currentRow[3];
            int size = std::stoi(currentRow[4]);
            double price = std::stod(currentRow[5]);
            double ts_delta = std::stod(currentRow[6]);
            int bq = 0;//std::stoi(currentRow[7]);
            double bp = 0;//std::stod(currentRow[8]);
            int aq = 0;//std::stoi(currentRow[9]);
            double ap = 0;//std::stod(currentRow[10]);


	    if (currentEvent) {
	    	delete currentEvent;
	    }
            currentEvent = new Event(ticker, time, action, side, size, price, ts_delta, bq, bp, aq, ap);
        } else {
            done = true; // No more lines to read
        }
    }
};

class Realisation {
public:
    Realisation(const std::string& filename) : filename(filename) {} // Fixed constructor

    EventIterator begin() {
        return EventIterator(filename);
    }

    EventIterator end() {
        return EventIterator(""); // This is generally not recommended. Consider using a proper end condition.
    }

private:
    std::string filename;
};

#endif //PARSE_H
