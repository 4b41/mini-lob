#include <iostream>
#include <fstream>
#include <iomanip>

#include "../include/feed.h"
#include "../include/orderbook.h"

void feed::load_stream(){
  
}

void feed::read_from_stream(){
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

void feed::simulate_cl(){
  std::ifstream f("./res/ascii.txt");

  if (f.is_open()){
    std::cout << f.rdbuf() << std::endl;
  } else {
    throw std::runtime_error("File \"ascii.txt\" could not be opened.");
  }

  while(true){
    int inp_side;
    int inp_type;
    Quantity inp_q;
    Price inp_p;

    OrderID n_id = _book.nextOrder();

    std::cout << "Select side: 1 - Submit bid | 2 - Submit ask" << std::endl;
    std::cin >> inp_side;
    std::cout << std::endl;

    std::cout << "Select order type: 1 - Market | 2 - Limit(GTC)" << std::endl;
    std::cin >> inp_type;
    std::cout << std::endl;

    std:: cout << "Input order quantity: ";
    std::cin >> inp_q;
    std::cout << std::endl;

    Side n_s = static_cast<Side>(i-1);

    if (inp_type == 1){
      std::shared_ptr<Order> ptr = std::make_shared<Order>(OrderType::Market);
      book.addOrder(ptr);

      std::cout << "Market " << inp_side == 1 ? "bid ":"ask " << "order #" << n_id << " " << "for " << inp_q << " units has been submitted!";
    } else {
      std::cout << "Input limit price: ";
      std::cin >> inp_p;
      std::cout << std::endl;

      std::shared_ptr<Order> ptr = std::make_shared<Order>(OrderType::GTC, n_id, n_s, inp_p, inp_q);
      book.addOrder(ptr);

      std::cout << "GTC " << inp_side == 1 ? "bid ":"ask " << "order #" << n_id << " " << "for " << inp_q << "units has been submitted!";
    }

    OrderBookLimitObj data = _book.getOrderData();
    printOB(data);
  }
}

void feed::printOB(OrderBookLimitObj& data){
  const auto& bids = data.getBids();
  const auto& asks = data.getAsks();
    
  size_t size = std::max(bids.size(), asks.size());

  std::cout << std::setw(6) << "A" << std::setw(13) << "Asks" << std::setw(22) << "Bids" << std::setw(10) << "B" << std::endl;
  
  Quantity maxA = 0;
  Quantity maxB = 0;

  for (size_t i = 0; i < size; i++){
    if (i < asks.size()){
      maxA = std::max(maxA,asks[i]._quantity);
    }
    if (i < bids.size()){
      maxB = std::max(maxB,bids[i]._quantity);
    }
  }

  for (size_t i = 0; i < size; i++){
    if (i < asks.size()){
      std::cout << std::setw(6) << asks[i]._price << " " << std::setw(12);
      double f = static_cast<double>(asks[i]._quantity)/maxA;
      int barsize = static_cast<int>(f*12);
      for (int i = 0; i < barsize; i++){
        std::cout << "█";
      }
      std::cout << " " << std::setw(6) << asks[i]._quantity << "    ";
    }
    if (i < bids.size()){
      std::cout << std::setw(6) << bids[i]._price << " ";
      double f = static_cast<double>(bids[i]._quantity)/maxB;
      int barsize = static_cast<int>(f*12);
      for (int i = 0; i < barsize; i++){
        std::cout << "█";
      }
      for (int i = 0; i < 12-barsize; i++){
        std::cout << " ";
      }
      std::cout << " " << std::setw(6) << bids[i]._price()
    }
    std::cout << std::endl;
  }
}

// 6  1    12  1 6  8 6 1 12     1 6
//   A     Asks          Bids      B
// 100 ████████ 10     9 ██████    99
// 101   ██████  8     8 █████     98
// 102   ██████  8    12 ████████  97
// 103    █████  7     1 █         96
