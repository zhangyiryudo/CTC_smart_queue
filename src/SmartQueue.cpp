#include "SmartQueue.h"

void SmartQueue::push(Quote quote) {
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

std::optional<Quote> SmartQueue::tryPop() {
    std::scoped_lock lock(mutex_);
    if (queue_.empty()) {
        return std::nullopt;
    }

    Quote quote = std::move(queue_.front());
    index_.erase(quote.symbol);
    queue_.pop_front();
    return quote;
}

std::size_t SmartQueue::size() const {
    std::scoped_lock lock(mutex_);
    return queue_.size();
}

bool SmartQueue::empty() const {
    return size() == 0;
}