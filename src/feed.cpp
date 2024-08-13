#include <iostream>
#include <fstream>

#include "../include/feed.h"
#include "../include/orderbook.h"

void feed::Best_Bid_Best_Offer(){

  std::ifstream TIME(this->time_from_file.c_str());
  std::ifstream BID(this->bids_from_file.c_str());
  std::ifstream BIDSIZE(this->bid_size_from_file.c_str());
  std::ifstream ASK(this->asks_from_file.c_str());
  std::ifstream ASKSIZE(this->ask_size_from_file.c_str());

  while (TIME >> _time, BID >> _bid, BIDSIZE >> _bidsize, ASK >> _asks, ASKSIZE >> _asksize){
    if ((_bid > 0.01) && (_bids < 9999) && (_bid_size != 0) && (_asks > 0.01) && (_asksize != 0)){
      std::shared_ptr<Order> bid = std<::make_shared<Order>(OrderType::GTC,id,Side::Buy,_bid,_bid_size);
      std::shared_ptr<Order> ask = std<::make_shared<Order>(OrderType::GTC,id,Side::Sell,_asks,_ask_size);
      _book.addOrder(bid);
      _book.addOrder(match);
      Trades log = _book.MatchOrders(); 
    }
  }
