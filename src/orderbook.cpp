#include "../include/orderbook.h"

#include <numeric>
#include <chrono>
#include <ctime>

Trades OrderBook::addOrder(OrderPointer order){
  if (_orders.contains(order->getOrderID())){
    return { };
  }

  if (order->getOrderType() == OrderType::MARKET){
    if (order->getSide() == Side::Buy && !_asks.empty()){
      const auto& [ask,_] = *_asks.rbegin();
      order->fillAtMarket(ask);
    } else if (order->getSide() == Side::Sell && !_bids.empty()){
      const auto& [bid,_] = *_bids.rbegin();
      order->fillAtMarket(bid);
    }
  }

  if (order->getOrderType() == OrderType::FOK && !canMatch(order->getSide(), order->getPrice())){
    return { };
  }

  OrderPointers::iterator IT;

  if (order->getSide() == Side::Buy){
    auto& orders  = _bids[order->getPrice()];
    orders.push_back(order);
    IT = std::next(orders.begin(), orders.size()-1);
  } else {
    auto& orders = _asks[order->getPrice()];
    orders.push_back(order);
    IT = std::next(orders.begin(), orders.size()-1);
  }
  _orders.insert({ order->getOrderID(), OrderEntry{ order, IT } });

  return MatchOrders();
}

void OrderBook::cancelOrder(OrderID id){
  if (!_orders.contains(id)){
    return;
  }  

  const auto& [order, IT] = _orders.at(id);
  _orders.erase(id);
  
  if (order->getSide() == Side::Sell){
    auto price = order->getPrice();
    auto& orders = _bids.at(price);
    orders.erase(IT);
    if (orders.empty()){
      _bids.erase(price);
    }
  } else {
    auto price = order->getPrice();
    auto& orders = _asks.at(price);
    orders.erase(IT);
    if (orders.empty()){
      _asks.erase(price);
    }
  }
}

Trades OrderBook::modifyOrder(OrderModify order){
  if (!_orders.contains(order.getOrderID())){
    return { };
  }
  const auto& [prevOrder,_] = _orders.at(order.getOrderID());
  cancelOrder(order.getOrderID());
  return addOrder(order.toOrder(prevOrder->getOrderType()));
}

bool OrderBook::canMatch(Side side, Price price) const {
  if (side == Side::Buy){
    if (_asks.empty()){
      return false;
    }
    const auto& [bestBid, _] = *_bids.begin();
    return price >= bestBid;
  } else {
    if (_bids.empty()){
      return false;
    }
    const auto& [bestAsk, _] = *_asks.begin();
    return price >= bestAsk;
  }
}

Trades OrderBook::MatchOrders(){
  Trades trades;
  trades.reserve(_orders.size());

  while (true){
    if (_bids.empty() || _asks.empty()){
      break;
    }
    
    auto& [bidPrice, bids] = *_bids.begin();
    auto& [askPrice, asks] = *_asks.begin();

    if (bidPrice < askPrice){
      break;
    }
    while (bids.size() && asks.size()){
      auto& bid = bids.front();
      auto& ask = asks.front();

      Quantity quantity = std::min(bid->getRemainingQuantity(), ask->getRemainingQuantity());
      
      bid->fill(quantity);
      ask->fill(quantity);

      if (bid->isFull()){
        bids.pop_front();
        _orders.erase(bid->getOrderID());
      }
      if (ask->isFull()){
        asks.pop_front();
        _orders.erase(ask->getOrderID());
      }
      
      if (bids.empty()){
        _bids.erase(bidPrice);
      }
      if (asks.empty()){
        _asks.erase(askPrice);
      }

      trades.push_back(
          Trade{
            TradeInfo{ bid->getOrderID(), bid->getPrice(), quantity},
            TradeInfo{ ask->getOrderID(), ask->getPrice(), quantity}
          });
    }
  } 
  if (!_bids.empty()){
    auto& [_,bids] = *_bids.begin();
    auto& order = bids.front();
    if (order->getOrderType() == OrderType::FOK){
      cancelOrder(order->getOrderID());
    }
  }
  if (!_asks.empty()){
    auto& [_,asks] = *_asks.begin();
    auto& order = asks.front();
    if (order->getOrderType() == OrderType::FOK){
      cancelOrder(order->getOrderID());
    }
  }
  _recent = trades;
  return trades;
}

OrderBookLimitObj OrderBook::getOrderData() const {
  LimitObjs bidData, askData;
  bidData.reserve(_orders.size());
  askData.reserve(_orders.size());

  auto createLimitObj = [](Price price, const OrderPointers& orders){
    return LimitObj{ price, std::accumulate(orders.begin(), orders.end(), (Quantity)0, [](Quantity runningSum, const OrderPointer& order){
        return runningSum + order->getRemainingQuantity();
        })
    };
  };
  
  for (const auto& [price,orders]: _bids){
    bidData.push_back(createLimitObj(price,orders));
  }

  for (const auto& [price,orders]: _asks){
    askData.push_back(createLimitObj(price,orders));
  }
  
  return OrderBookLimitObj{ bidData, askData };
} 

OrderID OrderBook::nextID(){
  _prevID += 1;
  return _prevID;
}
