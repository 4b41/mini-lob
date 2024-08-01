#pragma once
#ifndef ORDERBOOK_H_
#define ORDERBOOK_H_

#include <map>
#include <unordered_map>
#include <functional>
#include <thread>

#include "order.h"
#include "trade.h"

struct LimitObj {
  Price _price;
  Quantity _quantity;
};

using LimitObjs = std::vector<LimitObj>;

class OrderBookLimitObj {
  public:
    OrderBookLimitObj(const LimitObjs& bids, const LimitObjs& asks) {
      this->_bids = bids;
      this->_asks = asks;
    }

    const LimitObjs& getBids() const { return _bids; }
    const LimitObjs& getAsks() const { return _asks; }
    
  private:
    LimitObjs _bids;
    LimitObjs _asks;
};

class OrderBook {
  public:
    
    Trades addOrder(OrderPointer order);
    void cancelOrder(OrderID id);
    Trades modifyOrder(OrderModify order);

    std::size_t size() const { return _orders.size(); }

  private:
    struct OrderEntry {
      OrderPointer _order { nullptr };
      OrderPointers::iterator _location;
    };

    struct LimitData {
      OrderPointer _order{ nullptr };
      OrderPointers::iterator _IT;
    };

    std::map<Price,OrderPointers, std::greater<Price>> _bids;
    std::map<Price,OrderPointers, std::less<Price>> _asks;
    std::unordered_map<OrderID,OrderEntry> _orders;
    
    bool canMatch(Side side, Price price) const;
    bool canFill(Side side, Price price, Quantity quantity);

    Trades MatchOrders();

    OrderBookLimitObj getOrderData() const;

};

#endif // !ORDERBOOK_H_
