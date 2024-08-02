#pragma once
#ifndef USINGS_H_
#define USINGS_H_

#include <vector>
#include <chrono>

using Price = std::int32_t;
using Quantity = std::uint32_t;
using Timestamp = std::uint64_t;
using OrderID = std::uint64_t;
using OrderIDs = std::vector<OrderID>;

inline Timestamp log_time() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::system_clock::now().time_since_epoch()
      ).count();
}
#endif // !USINGS_H_

