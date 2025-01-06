#include "marketfeed.hpp"

using namespace std;

int main() {
  MarketFeed feed("api.gemini.com");
  feed.subscribe();
  feed.run();
  return 0;
}
