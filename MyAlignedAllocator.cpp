
#include "MyAlignedAllocator.hpp"
#include <iostream>
#include <vector>

struct TradeData {
    alignas(CACHE_LINE_SIZE) int volume;
    double price;
};
                
int main() {
    // Vector whose internal storage is cache‑line aligned
    std::vector<TradeData, MyAlignedAllocator<TradeData>> trades;

    trades.reserve(1024);
    trades.push_back({100, 150.25});

    for(const auto& trade : trades) {
        std::cout<<"Volume: "<<trade.volume<<", Price: "<<trade.price<<'\n';
    }

    return 0;
}
                             