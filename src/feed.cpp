#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>

#include "../include/feed.h"
#include "../include/orderbook.h"

//void Feed::load_stream(){
//  
//}

//void Feed::read_from_stream(){
//  std::ifstream TIME(this->time_from_file.c_str());
//  std::ifstream BID(this->bids_from_file.c_str());
//  std::ifstream BIDSIZE(this->bid_size_from_file.c_str());
//  std::ifstream ASK(this->asks_from_file.c_str());
//  std::ifstream ASKSIZE(this->ask_size_from_file.c_str());
//
//  while (TIME >> _time, BID >> _bid, BIDSIZE >> _bidsize, ASK >> _asks, ASKSIZE >> _asksize){
//    if ((_bid > 0.01) && (_bids < 9999) && (_bid_size != 0) && (_asks > 0.01) && (_asksize != 0)){
//      std::shared_ptr<Order> bid = std<::make_shared<Order>(OrderType::GTC,id,Side::Buy,_bid,_bid_size);
//      std::shared_ptr<Order> ask = std<::make_shared<Order>(OrderType::GTC,id,Side::Sell,_asks,_ask_size);
//      _book.addOrder(bid);
//      _book.addOrder(match);
//      Trades log = _book.MatchOrders(); 
//    }
//  }

void Feed::open_communication(){
  this->_fd_write = open("OUT_pipe", O_WRONLY);
  this->_fd_read = open("INP_pipe", O_WRONLY);

  if (this->_fd_write == -1 || this->_fd_read == -1){
    perror("Failed to open pipes!");
    return;
  }
  this->_opencomm = true;
}

void Feed::read_from_pipe(){
  if (!this->_opencomm || this->_fd_read == -1){
    perror("Read pipe not found, failed to open communication.");
    return;
  }
  char buf[256];
  std::vector<std::string> tick;

  while (true){
    // tick format: SYMBOL SIDE PRICE QUANTITY 
    ssize_t size_in = read(this->_fd_read,buf,256);
    if (size_in > 0){
      char delimiter[2] = ",";
      char *token = strtok(buf,delimiter);
      while (token != NULL){
        tick.push_back(token);
        token = strtok(buf,delimiter);
      }
      OrderID n_id = _book.nextID();
      Side n_s = tick[1] == "B" ? Side::Buy:Side::Sell;
      Price n_p = std::stoll(tick[2]);
      Quantity n_q = std::stoll(tick[3]);

      std::shared_ptr<Order> ptr = std::make_shared<Order>(OrderType::GTC, n_id, n_s, n_p, n_q);
      _book.addOrder(ptr);
    } else if (size_in < 0){
      perror("Read failed");
      break;
    }
    if (!this->_opencomm){
      std::cout << "Closing read pipe.";
      close(_fd_read);
      break;
    }
  }
}

void Feed::write_all_to_pipe(){
  if (!this->_opencomm || this->_fd_write == -1){
    perror("Write pipe not found, failed to open communication.");
    return;
  }
  OrderBookLimitObj data = this->_book.getOrderData();

  const auto& bids = data.getBids();
  const auto& asks = data.getAsks();
  
  for (size_t i = 0; i < bids.size(); i++){
    // FORMAT: B/A, LIMIT, QUANTITY
    //         B,   100,   40 
    std::string str = "B, " + std::to_string(bids[i]._price) + ", " + std::to_string(bids[i]._quantity);
    if (write(this->_fd_write, str.c_str(), str.size()) == -1){
      perror("Failed to write to pipe");
      return;
    }
  }
  for (size_t i = 0; i < asks.size(); i++){
    std::string str = "A, " + std::to_string(asks[i]._price) + ", " + std::to_string(asks[i]._quantity);
    if (write(this->_fd_write, str.c_str(), str.size()) == -1){
      perror("Failed to write to pipe");
      return;
    }
  }
}

void Feed::write_best_to_pipe(int n){
  if (!this->_opencomm || this->_fd_write == -1){
    perror("Write pipe not found, failed to open communication.");
    return;
  }
  OrderBookLimitObj data = this->_book.getOrderData();

  const auto& bids = data.getBids();
  const auto& asks = data.getAsks();

  for (size_t i = 0; i < n; i++){
    if (i < bids.size()){
      std::string str = "B, " + std::to_string(bids[i]._price) + ", " + std::to_string(bids[i]._quantity);
      if (write(this->_fd_write, str.c_str(), str.size()) == -1){
        perror("Failed to write to pipe");
        return;
      }
    }
  }
  for (size_t i = 0; i < n; i++){
    if (i < asks.size()){
      std::string str = "A, " + std::to_string(asks[i]._price) + ", " + std::to_string(asks[i]._quantity);
      if (write(this->_fd_write, str.c_str(), str.size()) == -1){
        perror("Failed to write to pipe");
        return;
      }
    }
  }
}

void Feed::write_recent_to_pipe(){
  for (size_t i = 0; i < this->_recent.size(); i++){
    // FORMAT: B/A, LIMIT, QUANTITY
    if (_recent[i].getBid() != NULL){
      std::string str = "B, " + std::to_string(_recent[i].getBid()_price) + ", " + std::to_string(_recent[i].getBid()_quantity);
      if (write(this->_fd_write, str.c_str(), str.size()) == -1){
        perror("Failed to write to pipe");
        return;
      }
    }
  }
  for (size_t i = 0; i < this->_recent.size(); i++){
    if (_recent[i].getAsk() != NULL){
      std::string str = "A, " + std::to_string(_recent[i].getAsk()._price) + ", " + std::to_string(_recent[i].getAsk()._quantity);
      if (write(this->_fd_write, str.c_str(), str.size()) == -1){
        perror("Failed to write to pipe");
        return;
      }
    }
  }
}


void Feed::simulate_cl(){
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

    std::cout << " Select option: " << std::endl << " 1 - Submit bid | 2 - Submit ask | 3 - Quit" << std::endl << " ";
    std::cin >> inp_side;
    std::cout << std::endl;
    
    if (inp_side == 3) break;

    std::cout << " Select order type: 1 - Market | 2 - Limit(GTC)" << std::endl << " ";
    std::cin >> inp_type;
    std::cout << std::endl;

    if (inp_type == 2){
      std::cout << " Input limit price: ";
      std::cin >> inp_p;
      std::cout << std::endl;
    }

    std:: cout << " Input order quantity: ";
    std::cin >> inp_q;
    std::cout << std::endl;

    Side n_s = static_cast<Side>(inp_side-1);
    OrderID n_id = _book.nextID();

    if (inp_type == 1){
      std::shared_ptr<Order> ptr = std::make_shared<Order>(OrderType::MARKET, n_id, n_s, inp_q);
      _book.addOrder(ptr);

      std::cout << " Market " << (inp_side == 1 ? "bid ":"ask ") << "order #" << n_id << " " << "for " << inp_q << " " << "units has been submitted!" << std::endl;
    } else if (inp_type == 2){
      std::shared_ptr<Order> ptr = std::make_shared<Order>(OrderType::GTC, n_id, n_s, inp_p, inp_q);
      _book.addOrder(ptr);

      std::cout << " GTC " << (inp_side == 1 ? "bid ":"ask ") << "order #" << n_id << " " << "for " << inp_q << " " << "units @ " << inp_p << " " << "has been submitted!" << std::endl;
    }

    OrderBookLimitObj data = _book.getOrderData();
    printOB(data);
  }
  std::cout << " Order book successfully terminated." << std::endl;
}

void Feed::printOB(OrderBookLimitObj& data){
  const auto& bids = data.getBids();
  const auto& asks = data.getAsks();
    
  size_t size = std::max(bids.size(), asks.size());
  size = std::min(static_cast<int>(size), 8); // limit to 8 orders closest to midprice

  std::cout << std::endl << " " << std::setw(6) << "B" << std::setw(13) << "Bids" << std::setw(15) << "Quantity" << std::setw(9) << "Asks" << std::setw(14) << "A" << std::endl;
  
  Quantity maxB = 0;
  Quantity maxA = 0;

  for (size_t i = 0; i < size; i++){
    if (i < bids.size()){
      maxB = std::max(maxB,bids[i]._quantity);
    }
    if (i < asks.size()){
      maxA = std::max(maxA,asks[i]._quantity);
    }
  }

  for (size_t i = 0; i < size; i++){
    std::cout << " ";
    if (i < bids.size()){
      std::cout << std::setw(6) << bids[i]._price << " ";
      double f = static_cast<double>(bids[i]._quantity)/maxB;
      int barsize = static_cast<int>(f*12);
      for (int i = 0; i < 12-barsize; i++){
        std::cout << " ";
      }
      for (int i = 0; i < barsize; i++){
        std::cout << "█";
      }
      std::cout << " " << std::setw(6) << bids[i]._quantity << "      ";
    } else {
      std::cout << "                                ";
    }
    if (i < asks.size()){
      std::cout << std::setw(6) << asks[i]._quantity << " ";
      double f = static_cast<double>(asks[i]._quantity)/maxA;
      int barsize = static_cast<int>(f*12);
      for (int i = 0; i < barsize; i++){
        std::cout << "█";
      }
      for (int i = 0; i < 12-barsize; i++){
        std::cout << " ";
      }
      std::cout << " " << std::setw(6) << asks[i]._price;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << " ---------------------------------------------------" << std::endl;
}

// 6  1    12  1 6  8 6 1 12     1 6
//   B     Bids          Asks      A
// 100 ████████ 10     9 ██████    99
// 101   ██████  8     8 █████     98
// 102   ██████  8    12 ████████  97
// 103    █████  7     1 █         96
