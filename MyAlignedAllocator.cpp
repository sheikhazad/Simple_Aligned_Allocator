
#include "MyAlignedAllocator.hpp"
#include <iostream>
#include <vector>

//struct alignas(CACHE_LINE_SIZE) TradeData ==> Same as align first member of struct.
struct TradeData {
    /*alignas(CACHE_LINE_SIZE) is still needed even when using a cache‑aligned allocator
      MyAlignedAllocator guarantees that the start of the allocated block is aligned to a cache line.
      But it does NOT guarantee that each element inside the block is cache‑line aligned.
    */
    //Same effect as struct alignas(CACHE_LINE_SIZE) TradeData
    alignas(CACHE_LINE_SIZE) int volume; 
    //aligning one member is enough to align the entire struct.
    //There is no benefit to aligning rest of the struct members
    //alignas(64) double price; // unnecessary
    double price;
};
                
int main() {
    // Vector whose internal storage is cache‑line aligned
    std::vector<TradeData, MyAlignedAllocator<TradeData>> trades;

    trades.reserve(1024);
    trades.emplace_back({100, 150.25});

    for(const auto& trade : trades) {
        std::cout<<"Volume: "<<trade.volume<<", Price: "<<trade.price<<'\n';
    }

    return 0;
}
                             
