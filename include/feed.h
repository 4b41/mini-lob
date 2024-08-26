#pragma once
#ifndef FEED_H_
#define FEED_H_

#include "orderbook.h"

#include <string>
#include <thread>
#include <fcntl.h>
#include <unistd.h>

class Feed {
  public:
    void simulate_cl();
    void open_communication();

  private:
    Timestamp _time;
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

    int _fd_write;
    int _fd_read;
    bool _opencomm = false;

    void read_from_txt();
    void read_from_pipe();

    void printOB(OrderBookLimitObj& data);
};

#endif // !FEED_H_

