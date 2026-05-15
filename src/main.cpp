#include <iostream>
#include <string>
#include <vector>

#include "SmartQueue.h"

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
