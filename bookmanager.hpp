#pragma once

/* Considering we are managing orderbooks for multiple symbols. BookManager
 * will aggregate the market data and assign it to the respective symbol.
*/

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <ranges>
#include <nlohmann/json.hpp>

#include "orderbook.hpp"

using json = nlohmann::json;

class BookManager {
  static std::shared_ptr<BookManager> instance;
  std::unordered_map<std::string, OrderBook> orderbooks;

  std::string currSymbol;

public:
  static std::shared_ptr<BookManager> getInstance() {
    if (not instance)
      instance = std::make_shared<BookManager>();

    return instance;
  }

  void addL2Update(json json_order) {
    std::string event_type = json_order["type"].template get<std::string>();
    if (event_type != "l2_updates")
      return;

    auto changes = 
      json_order["changes"].template get<std::vector<std::vector<std::string>>>();
    std::string symbol = json_order["symbol"].template get<std::string>();

    for (auto change : changes) {
      Side side;
      if (change[0] == "buy")
        side = Side::Bid;
      else if (change[0] == "sell")
        side = Side::Ask;

      Order order = {
        .side = side,
        .price = std::stod(change[1]),
        .quantity = std::stof(change[2]),
      };

      orderbooks[symbol].addOrder(order);
    }
  }

  OrderBook getCurrentBook() { return orderbooks[currSymbol]; }

  std::string getCurrentSymbol() { return currSymbol; }
  void setCurrentSymbol(std::string symbol) { currSymbol = symbol; }

  //std::vector<std::string> getSymbols() { return std::views::keys(orderbooks); }
};
