#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class Symbol {
    AAPL,
    MSFT,
    GOOGL,
    AMZN,
    TSLA,
    INVALID,
};

using PriceValue = std::variant<long long, double>;

struct Quote {
    Symbol symbol;
    PriceValue ask;
};

class SmartQueue {
public:
    void push(Quote quote) {
        std::scoped_lock lock(mutex_);
        if (quote.symbol == Symbol::INVALID) {
            return;
        }

        auto it = index_.find(quote.symbol);
        if (it != index_.end()) {
            queue_.erase(it->second);
            index_.erase(it);
        }

        queue_.push_back(std::move(quote));
        index_[queue_.back().symbol] = std::prev(queue_.end());
    }

    std::optional<Quote> tryPop() {
        std::scoped_lock lock(mutex_);
        if (queue_.empty()) {
            return std::nullopt;
        }

        Quote quote = std::move(queue_.front());
        index_.erase(quote.symbol);
        queue_.pop_front();
        return quote;
    }

    std::size_t size() const {
        std::scoped_lock lock(mutex_);
        return queue_.size();
    }

    bool empty() const {
        return size() == 0;
    }

private:
    mutable std::mutex mutex_;
    std::list<Quote> queue_;
    std::unordered_map<Symbol, std::list<Quote>::iterator> index_;
};

static std::string toString(Symbol symbol) {
    switch (symbol) {
        case Symbol::AAPL: return "AAPL";
        case Symbol::MSFT: return "MSFT";
        case Symbol::GOOGL: return "GOOGL";
        case Symbol::AMZN: return "AMZN";
        case Symbol::TSLA: return "TSLA";
        default: return "INVALID";
    }
}

static std::string toString(const PriceValue& value) {
    return std::visit([](auto&& v) {
        return std::to_string(v);
    }, value);
}

int main() {
    SmartQueue queue;

    std::vector<Quote> updates = {
        {Symbol::AAPL, 175.24},
        {Symbol::MSFT, 329.10},
        {Symbol::AAPL, -10ll},       // negative price example
        {Symbol::GOOGL, 2951.55},
        {Symbol::MSFT, 332ll},       // integer price example
        {Symbol::AMZN, 139.82},
        {Symbol::AAPL, 176.50},
        {Symbol::TSLA, -888888888888ll}, // large negative price example
    };

    for (auto& update : updates) {
        queue.push(update);
    }

    std::cout << "Final queue state (latest update per symbol):\n";
    while (auto quote = queue.tryPop()) {
        std::cout << "  " << toString(quote->symbol)
                  << " ask=" << toString(quote->ask)
                  << '\n';
    }

    return 0;
}
