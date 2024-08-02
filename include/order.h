#pragma once
#ifndef ORDER_H_
#define ORDER_H_

#include <vector>
#include <list>
#include <exception>
#include <format>
#include <memory>

#include "usings.h"

enum class OrderType {
  GTC,
  IOC,
  FOK,
  GFD,
  AON,
  MARKET
};

enum class Side {
  Buy,
  Sell
};

class Order {
  public:
    Order (OrderType orderType, OrderID id, Side side, Price price, Quantity quantity){
      this->_orderType = orderType;
      this->_orderID = id;
      this->_side = side;
      this->_price = price;
      this->_timestamp = log_time();
      this->_initialQuantity = quantity;
      this->_remainingQuantity = quantity;
    }

    Order (OrderType orderType, OrderID id, Side side, Quantity quantity) 
      : Order(OrderType::MARKET, id, side, std::numeric_limits<Price>::quiet_NaN(), quantity){}

    OrderType getOrderType() const { return _orderType; }
    OrderID getOrderID() const { return _orderID; }
    Side getSide() const { return _side; }
    Price getPrice() const { return _price; }
    Timestamp getTimestamp() const { return _timestamp; }
    Quantity getRemainingQuantity() const { return _remainingQuantity; }
    Quantity getFilledQuantity() const { return _initialQuantity - _remainingQuantity; }

    bool isFull() const { return getRemainingQuantity() == 0; }
    void fill(Quantity quantity){
      if (quantity > getRemainingQuantity()){
        throw std::logic_error(std::format("Order ({}) cannot be filled for more than its remaining quantity)", getOrderID()));
      }
      _remainingQuantity -= quantity;
    }

  private:
    OrderType _orderType;
    OrderID _orderID;
    Side _side;
    Price _price;
    Timestamp _timestamp;
    Quantity _initialQuantity;
    Quantity _remainingQuantity;
  
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

class OrderModify {
  public:
    OrderModify(OrderID id, Side side, Price price, Quantity quantity){
      this->_id = id;
      this->_side = side;
      this->_price = price;
      this->_timestamp = log_time();
      this->_quantity = quantity;
    }

    OrderID getOrderID() const { return _id; }
    Side getSide() const { return _side; }
    Price getPrice() const { return _price; }
    Timestamp getTimestamp() const { return _timestamp; }
    Quantity getQuantity() const { return _quantity; }
    
    OrderPointer toOrder(OrderType type) const {
      return std::make_shared<Order>(type,getOrderID(), getSide(), getPrice(), getQuantity());
    }

  private:
    OrderID _id;
    Side _side;
    Price _price;
    Timestamp _timestamp;
    Quantity _quantity;
};

#endif // !ORDER_H_
