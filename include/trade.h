#pragma once
#ifndef TRADE_H_
#define TRADE_H_

#include "usings.h"

struct TradeInfo {
  OrderID _id;
  Price _price;
  Quantity _quantity;
  Timestamp _timestamp;
};

class Trade {
  public:
    Trade(const TradeInfo& bid, const TradeInfo& ask){
      this->_bid = bid;
      this->_ask = ask;
      this->_timestamp = log_time();
    }

    const TradeInfo& getBid() const { return _bid; }
    const TradeInfo& getAsk() const { return _ask; }

  private:
    TradeInfo _bid;
    TradeInfo _ask;
    Timestamp _timestamp;
};

using Trades = std::vector<Trade>;

#endif // !TRADE_H_

