#pragma once

/* For each symbol, we have an order book. Each order book contains the bid/ask
 * information from the L2 market data. Here, we can calculate:
 * Spread
 * Market sentiment direction
 * Support and Resistance values
 * Volume direction
*/

#include <map>
#include <algorithm>
#include <utility>
#include <vector>

enum Side {
  Bid,
  Ask
};

struct Order {
  Side side;
  double price;
  float quantity;
};

class OrderBook {
  std::map<double, float, std::greater<double>> bids;
  std::map<double, float, std::less<double>> asks; 

public:

  void addOrder(Order order) {
    switch (order.side) {
      case Bid: bids[order.price] += order.quantity; break;
      case Ask: asks[order.price] += order.quantity; break;
      default: std::unreachable(); // Must panic here
    }
  }

  void addOrder(std::vector<Order> orders) {
    for (auto order : orders)
      addOrder(order);
  }
};
