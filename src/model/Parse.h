#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <cassert>

#include "Types.h"

const int seconds_in_day = 60*60*24;

class EventIterator {
	public:
		EventIterator(const std::string& filename) : file(filename), done(false) {
			if (!file.is_open()) {
				done = true;
			} else {
				std::string line;
				std::getline(file, line);	
				readNextLine();
			}
		}

		bool operator!=(const EventIterator& other) const {
			return !done;
		}

		const Event* operator*() const {
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

				// instrument,ts_event,ts_recv,seconds_since_start,order_id,action,side,size,price,bq,bp,aq,apES,1726617600001337031,1726617600001502573,0.001337031,6413845537760,C,A,1,5644250000000,11,5644.0,5,5644.25
				// ES,1726617600001338261,1726617600001502573,0.001338261,6413845537764,C,A,1,5644250000000,11,5644.0,4,5644.25
				assert(currentRow.size() == 12);

				std::string ticker = currentRow[0];
				long double ts_event = std::stod(currentRow[1]);
				long double ts_recv = std::stod(currentRow[1]);
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

				//'AB', 'AA', 'CB', 'CA', 'MA', 'MB', 'TA', 'FB', 'TB', 'FA'
				int event_type = -1;
				if (action=="A") {
					event_type = 1;
				} else if (action=="C") {
					event_type = 2;
				} else if (action=="M") {
					event_type = 3;
				} else if (action=="T") {
					event_type = 4;
				} else if (action=="F") {
					event_type = 5;
				}

				if (event_type != -1) {
					event_type *= 2;
					if (side=="A") {
						event_type += 1;
					} else if (side=="N") {
						event_type = -1;
					}
				}

				if (event_type==-1) {
					readNextLine();
				} else {
					if (currentEvent) {
						delete currentEvent;
					}
					currentEvent = new Event(time, event_type, {}, 1.0);
				}

			} else {
				done = true; // No more lines to read
			}
		}
};

class Realisation {
public:
    Realisation(const std::string& filename) : filename(filename) {}

    EventIterator begin() {
        return EventIterator(filename);
    }

    EventIterator end() {
        return EventIterator("");
    }

private:
    std::string filename;
};

#endif //PARSE_H
