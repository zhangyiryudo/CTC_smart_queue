#pragma once

#include <array>
#include <atomic>
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

class SmartQueueLockFree {
public:
    SmartQueueLockFree();
    ~SmartQueueLockFree();
    SmartQueueLockFree(const SmartQueueLockFree&) = delete;
    SmartQueueLockFree& operator=(const SmartQueueLockFree&) = delete;
    SmartQueueLockFree(SmartQueueLockFree&&) = delete;
    SmartQueueLockFree& operator=(SmartQueueLockFree&&) = delete;

    void push(Quote quote);
    std::optional<Quote> tryPop();
    std::size_t size() const;
    bool empty() const;

private:
    struct Node {
        Quote quote;
        std::atomic<Node*> next;
        std::atomic<bool> active;

        explicit Node(Quote q)
            : quote(std::move(q)), next(nullptr), active(true) {}
    };

    static constexpr std::size_t kSymbolCount = static_cast<std::size_t>(Symbol::INVALID);

    static std::size_t symbolIndex(Symbol symbol) {
        return static_cast<std::size_t>(symbol);
    }

    std::atomic<Node*> head_;
    std::atomic<Node*> tail_;
    std::array<std::atomic<Node*>, kSymbolCount> latest_;
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