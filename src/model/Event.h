#ifndef EVENT_H
#define EVENT_H

#include <string>

class Event {
public:
    Event(const std::string& ticker, double time, const std::string& action,
          const std::string& side, int size, double price, double ts_delta,
          int bq, double bp, int aq, double ap)
        : ticker(ticker), time(time), action(action), side(side),
          size(size), price(price), ts_delta(ts_delta),
          bq(bq), bp(bp), aq(aq), ap(ap) {}

    // You can add getters and other member functions as needed

    std::string ticker;
    long double time;
    std::string action;
    std::string side;
    int size;
    double price;
    double ts_delta;
    int bq;
    double bp;
    int aq;
    double ap;
};

#endif // EVENT_H

