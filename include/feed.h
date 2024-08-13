#pragma once
#ifndef FEED_H_
#define FEED_H_

#include "orderbook.h"
#include <string>

class Feed {
  public:


  private:
    TimeStamp _time;
    Price _bid;
    Price _ask;
    Quantity _bidsize;
    Quantity _asksize;

    string time_from_file; // check type from alexabosi
    string bids_from_file;
    string asks_from_file;
    string bid_size_from_file;
    string ask_size_from_file;

    OrderBook _book;
}

#endif // !FEED_H_

