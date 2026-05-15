#include "SmartQueue.h"

SmartQueueLockFree::SmartQueueLockFree() {
    Quote dummyQuote{Symbol::INVALID, 0ll};
    Node* dummy = new Node(std::move(dummyQuote));
    dummy->active.store(false, std::memory_order_relaxed);
    head_.store(dummy, std::memory_order_relaxed);
    tail_.store(dummy, std::memory_order_relaxed);

    for (auto& slot : latest_) {
        slot.store(nullptr, std::memory_order_relaxed);
    }
}

SmartQueueLockFree::~SmartQueueLockFree() {
    Node* node = head_.load(std::memory_order_relaxed);
    while (node != nullptr) {
        Node* next = node->next.load(std::memory_order_relaxed);
        delete node;
        node = next;
    }
}

void SmartQueueLockFree::push(Quote quote) {
    if (quote.symbol == Symbol::INVALID) {
        return;
    }

    const std::size_t index = symbolIndex(quote.symbol);
    Node* node = new Node(std::move(quote));
    Node* previousLatest = latest_[index].exchange(node, std::memory_order_acq_rel);
    if (previousLatest != nullptr) {
        previousLatest->active.store(false, std::memory_order_release);
    }

    Node* previousTail = tail_.exchange(node, std::memory_order_acq_rel);
    previousTail->next.store(node, std::memory_order_release);
}

std::optional<Quote> SmartQueueLockFree::tryPop() {
    while (true) {
        Node* first = head_.load(std::memory_order_acquire);
        Node* next = first->next.load(std::memory_order_acquire);
        if (next == nullptr) {
            return std::nullopt;
        }

        if (!head_.compare_exchange_weak(first, next, std::memory_order_acq_rel, std::memory_order_acquire)) {
            continue;
        }

        delete first;
        const std::size_t index = symbolIndex(next->quote.symbol);
        Node* expected = next;

        if (latest_[index].compare_exchange_strong(expected, nullptr, std::memory_order_acq_rel, std::memory_order_acquire)) {
            if (next->active.load(std::memory_order_acquire)) {
                Quote quote = std::move(next->quote);
                return quote;
            }
            // Another update may have already replaced this node, fall through to continue.
        }
    }
}

std::size_t SmartQueueLockFree::size() const {
    std::size_t count = 0;
    for (const auto& slot : latest_) {
        Node* node = slot.load(std::memory_order_acquire);
        if (node != nullptr && node->active.load(std::memory_order_acquire)) {
            ++count;
        }
    }
    return count;
}

bool SmartQueueLockFree::empty() const {
    return size() == 0;
}
