#include "orderbook.h"

#include <numeric>
#include <chrono>
#include <ctime>

Trades Orderbook::addOrder(OrderPointer order){
  if (_orders.contains(order)){
    return { };
  }

  if (order->getOrderType() == OrderType::FOK && !canMatch(order->getSide(), order->getPrice())){
    return { }
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

void Orderbook::cancelOrder(OrderID id){
  if (!_orders.contains(id)){
    return;
  }  

  const auto& [order, IT] = _orders.at(orderID);
  _orders.erase(order);
  
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

Traders Orderbook::modifyOrder(OrderModify order){
  if (!_orders.contains(order.getOrderID())){
    return { };
  }
  const auto& [prevOrder,_] = _orders.at(order.getOrderID());
  cancelOrder(order.getOrderID);
  return addOrder(order.toOrder(prevOrder->getOrderType()));
}

bool Orderbook::canMatch(Side side, Price price){
  if (side == Side::Buy){
    if (_asks.empty()){
      return false;
    }
    const auto& [bestBid, _] = *bids_.begin();
    return price >= bestBid;
  } else {
    if (_bids.empty()){
      return false;
    }
    const auto& [bestAsk, _] = *asks_.begin();
    return price >= bestAsk;
  }
}

Trades Orderbook::MatchOrders(){
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
      auto& bid = bids.from();
      auto& ask = asks.from();

      Quantity quantity = std::min(bid->getRemainingQuantity(), ask->getRemainingQuantity());
      
      bid->fill(quantity);
      ask->fill(quantity);

      if (bid->isFull()){
        bids.pop_front();
        orders.erase(bid->getOrderID());
      }
      if (ask->isFull()){
        asks.pop_front();
        orders.erase(ask->getOrderID());
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
    if (order->getOrder()Type() == OrderType::FOK){
      cancelOrder(order->getOrderID());
    }
  }
  return trades;
}

OrderBookLimitObj Orderbook::getOrderData() const {
  LimitObjs bidData, askData;
  bidData.reserve(_orders.size());
  askData.reserve(_orders.size());

  auto createLimitObj = [](Price price, const OrderPointers& orders){
    return LimitObj{ price, std::accumulate(orders.begin(), orders.end(), (Quantity)0, [](Quantity runningSum, const OrderPointer& order){
        return runningSum + order->getRemainingQuantity(); }
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
