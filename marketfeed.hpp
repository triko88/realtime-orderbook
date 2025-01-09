#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <unordered_set>
#include <memory>
#include <sstream>
#include <vector>

namespace beast     = boost::beast;
namespace asio      = boost::asio;
namespace websocket = beast::websocket;

using tcp         = asio::ip::tcp;
using ssl_socket  = websocket::stream<asio::ssl::stream<tcp::socket>>;
using json        = nlohmann::json;

const std::string WEBSOCKET_PORT = "443";

class MarketFeed {
  /* 
   * MarketFeed is a websocket client. 
   * It is supposed to fetch market data and format it for our order book.
   * We are only looking into Level 2 orderbook data, as this is the lowest
   * level data publically available through Gemini Websocket API.
  */

  std::string hostname;
  std::unordered_set<std::string> symbols;
  std::unique_ptr<ssl_socket> web;

public:
  MarketFeed(std::string hostname, std::vector<std::string> symbols) :
    hostname(hostname), symbols(symbols.begin(), symbols.end()) {
    try {
      asio::io_context io_context;

      asio::ip::tcp::resolver resolver(io_context);
      auto const results = resolver.resolve(hostname, WEBSOCKET_PORT);

      // Create an SSL Context
      asio::ssl::context ssl_context{asio::ssl::context::tlsv12_client};

      // Create an SSL Stream
      web = std::make_unique<ssl_socket>(io_context, ssl_context);
      asio::connect(web->next_layer().lowest_layer(), results.begin(), results.end());

      // Perform SSL Handshake
      web->next_layer().handshake(asio::ssl::stream_base::client);
      web->handshake(hostname, "/v2/marketdata");

      std::cout << std::format("Connected to: [{}] ", hostname) << std::endl;
    } catch (const beast::system_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  void addSymbol(std::string symbol) {
    symbols.insert(symbol);
  }

  void removeSymbol(std::string symbol) {
    symbols.erase(symbol);
  }

  void subscribe() {
    std::stringstream payload;
    payload << "{\"type\":\"subscribe\",\"subscriptions\":"
      << "[{\"name\":\"l2\",\"symbols\":" << json(symbols).dump() << "}]}";

    web->write(asio::buffer(payload.str()));
  }

  void run() {
    try{
      beast::flat_buffer buffer;
      while(true) {
        web->read(buffer);
        std::string buffstr(asio::buffer_cast<const char*>(buffer.data()), buffer.size());
        json json_buffer = json::parse(buffstr);
        // Stream data to the order book stream
        // Ex: orderstream << json_buffer;
        // And the orderstream will redirect json data to the order book
        std::string event = json_buffer["type"].template get<std::string>();
        if (event == "l2_updates") {
          /*
        std::vector<std::vector<std::string>> changes =
          json_buffer["changes"].template get<std::vector<std::vector<std::string>>>();
          for (auto change : changes)
            std::cout << change[0] << "\t" << change[1] << "\t" << change[2] << std::endl;
            */
          std::cout << json_buffer.dump() << std::endl;
        }
        buffer.clear();
      }
    } catch (const beast::system_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  json getMarketData(beast::flat_buffer& buffer) {
    web->read(buffer);
    std::string buffstr(asio::buffer_cast<const char*>(buffer.data()), buffer.size());
    buffer.clear();

    return json::parse(buffstr);
  }
};
