
#include "MyAlignedAllocator.hpp"
#include <iostream>
#include <vector>
#include <memory>//std::construct_at, std::destroy_at

    

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

    //Usage: 1
    // Vector whose internal storage is cache‑line aligned
    std::vector<TradeData, MyAlignedAllocator<TradeData>> trades;

    trades.reserve(1024);
    trades.emplace_back({100, 150.25});

    for(const auto& trade : trades) {
        std::cout<<"Volume: "<<trade.volume<<", Price: "<<trade.price<<'\n';
    }

    //Usage: 2
    MyAlignedAllocator<Order> alloc;
    // 1. Allocate raw memory
    Order* p = alloc.allocate(1);
    // 2. Construct Order in that raw memory
    std::construct_at(p, 100, 50);

    // Use the object
    // p->...

    // 3. Destroy the Order object
    std::destroy_at(p);
    // 4. Release the raw memory
    alloc.deallocate(p, 1);

    /*
                     MyAlignedAllocator<Order>
                          │
                          ▼
                 allocate(1)
                          │
                          ▼
             ┌──────────────────────┐
             │ 64-byte aligned      │
             │ RAW MEMORY           │
             │ Order doesn't exist  │
             └──────────────────────┘
                          │
                          │ construct_at()
                          ▼
             ┌──────────────────────┐
             │ Order object         │
             │ price = 100          │
             │ quantity = 50        │
             └──────────────────────┘
                          │
                          │ destroy_at()
                          ▼
             ┌──────────────────────┐
             │ RAW MEMORY           │
             │ Order doesn't exist  │
             └──────────────────────┘
                          │
                          │ deallocate()
                          ▼
                       memory
                       returned
    */

    return 0;
}

                      
