#pragma once
#ifndef TRADE_H_
#define TRADE_H_

#include "usings.h"

struct TradeInfo {
  OrderID _id;
  Price price;
  Quantity quantity;
};

class Trade {
  public:
    Trade(const TradeInfo& bid, const TradeInfo& ask){
      this->_bid = bid;
      this->_ask = ask;
    }

    const TradeInfo& getBid() const return { return bidTrade; }
    const TradeInfo& getAsk() const return { return askTrade; }

  private:
    TradeInfo _bid;
    TradeInfo _ask;
}

using Trades = std::vector<Trade>;

#endif // !TRADE_H_

