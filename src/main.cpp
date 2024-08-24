#include <iostream>
#include <fstream>
#include "../include/orderbook.h"

int main(){
  std::ifstream f("./res/ascii.txt");

  if (f.is_open()){
    std::cout << f.rdbuf() << std::endl;
  } else {
    throw std::runtime_error("File \"ascii.txt\" could not be opened.");
  }

  OrderBook book;
  const OrderID id = 1;
  std::shared_ptr<Order> ptr = std::make_shared<Order>(OrderType::GTC, id, Side::Buy, 100, 10);
  book.addOrder(ptr);
  std::cout << book.size() << std::endl;
  std::cout << "Test Success" << std::endl;
  return 0;
}
