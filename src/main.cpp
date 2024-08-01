#include <iostream>
#include "../include/orderbook.h"

int main(){
  OrderBook book;
  const OrderID id = 1;
  std::shared_ptr<Order> ptr = std::make_shared<Order>(OrderType::GTC, id, Side::Buy, 100, 10);
  OrderPointer optr = ptr;
  book.addOrder(ptr);
  std::cout << book.size() << std::endl;
  std::cout << "Test Success" << std::endl;
  return 0;
}
