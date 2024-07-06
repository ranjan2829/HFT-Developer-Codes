#include "Matching_Engine.h"

namespace Exchange {
MatchingEngine::MatchingEngine(ClientRequestLFQueue *client_requests,
                               ClientResponseLFQueue *client_responses_,
                               MEMarketUpdateLFQueue *market_updates)
    : incoming_requests_(client_requests),
      outgoing_ogw_responses_(client_responses_),
      outgoing_md_updates_(market_updates),
      logger_("exchange_matching_engine.log") {
  for (size_t i = 0; i < ticker_order_book_.size(); ++i) {
    ticker_order_book_[i] = new MEOrderBook(i, &logger_, this);
  }
}
MatchingEngine::~MatchingEngine() {
  run_ = false;
  using namespace std::literals::chrono_literals;
  std::this_thread::sleep_for(1s);
  incoming_requests_ = nullptr;
  outgoing_ogw_responses_ = nullptr;
  outgoing_md_updates_ = nullptr;

  for (auto &order_book : ticket_order_book_) {
    delete order_book;
    order_book = nullptr;
  }
}
auto MatchingEngine::start() -> void {
  run_ = true;
  ASSERT(Common::createAndStartThread(-1, "Exchange/MatchingEngine",
                                      [this]() { run(); }) != nullptr,
         "Failed to start MatchingEngine thread.");
}
auto MatchingEngine::stop() -> void { run_ = false; }
} // namespace Exchange
