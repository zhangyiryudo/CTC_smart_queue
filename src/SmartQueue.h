#pragma once

#include <list>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <variant>

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
    void push(Quote quote);
    std::optional<Quote> tryPop();
    std::size_t size() const;
    bool empty() const;

private:
    mutable std::mutex mutex_;
    std::list<Quote> queue_;
    std::unordered_map<Symbol, std::list<Quote>::iterator> index_;
};