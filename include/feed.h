#pragma once
#ifndef FEED_H_
#define FEED_H_

#include "orderbook.h"
#include <string>

class Feed {
  public:
    Feed();
    void simulate_cl();

  private:
    TimeStamp _time;
    Price _bid;
    Price _ask;
    Quantity _bidsize;
    Quantity _asksize;

    std::string time_from_file; 
    std::string bids_from_file;
    std::string asks_from_file;
    std::string bid_size_from_file;
    std::string ask_size_from_file;

    OrderBook _book;

    void read_from_stream();
    void printOB(OrderBookLimitObj& data);
}

#endif // !FEED_H_

