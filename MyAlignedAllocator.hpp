//AlignedAllocator - Simpler version
//MyAlignedAllocator.hpp
#pragma once

#include <new>
#include <cstddef> // for std::size_t
#include <limits>


// Default to 64‑byte alignment (cache line size)
constexpr std::size_t CACHE_LINE_SIZE = 64;

/**
 * A simple, STL‑compatible aligned allocator.
 * Uses C++17 aligned new/delete to guarantee that all allocated memory
 * starts at a cache‑line boundary. This helps avoid false sharing and
 * improves cache locality in multi‑threaded systems.
 */
template<typename T, std::size_t Alignment = CACHE_LINE_SIZE>
struct MyAlignedAllocator {

    // Required typedefs for STL allocator compatibility, see README for details
    using value_type = T;

    //Optional: Not mandatory technically but nice to have: 
    //All instances of this allocator are considered equal — so memory can be shared between them safely
    //Always equal because allocator is stateless
    //This helps containers optimize away allocator comparisons and simplifies move operations.
    using is_always_equal = std::true_type;  //Optional

    //Following are not required as allocator_traits<MyAllocator<T>> will generate if missing
    //using pointer = T*;                // Pointer to allocated memory
    //using size_type = std::size_t;     // Type for size parameters

    // Default constructor — allocator is stateless
    //Can not be made explicit as STL containers need to implicitly construct allocators when copy/move container, rebind
    MyAlignedAllocator() noexcept = default;

    //1. Rebind Constructor: STL rebind constructor — allows constructing MyAlignedAllocator<U> from MyAlignedAllocator<T>
    // C++ containers e.g. std::vector<T> sometimes internally need to allocate a different type — not just T, but maybe T*, or std::pair<const T, U>, or void.
    // Allow rebinding allocator to another type (required by STL)
    // Constructor that takes an allocator of a different type U and the same alignment
    //Empty body since MyAlignedAllocator is stateless and doesn't need to do anything special when rebinding
    template<typename U>
    MyAlignedAllocator(const MyAlignedAllocator<U, Alignment>& ) noexcept {}

    /**
     * Allocate memory for n objects of type T, aligned to Alignment bytes.
     */
    T* allocate(std::size_t n) {
        // Overflow check: ensure n * sizeof(T) doesn't wrap
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_alloc{};

        // Allocate using C++17 aligned operator new
        // std::align_val_t : A tag type(not function) used to tell operator new what alignment to use
        void* p = ::operator new(n * sizeof(T), std::align_val_t(Alignment));
        return static_cast<T*>(p);
    }

    /**
     * Deallocate memory previously allocated with allocate().
     * Must use the matching aligned delete.
     *Even if the allocator does not need size, the parameter must be present so that the allocator matches what STL containers expect.
     */
    void deallocate(T* p, std::size_t) noexcept {
        //operator delete with alignment does not require the size. It only needs the pointer and the alignment.
        ::operator delete(p, std::align_val_t(Alignment));
    }

    //2. Rebind Allocator: Rebind allocator to another type U (required by STL containers)
    //For e.g. If we already have: 
    // MyAlignedAllocator<int, 64> myAllocator;
    //Then myAllocator::rebind<double>::other is:
    //MyAlignedAllocator<double, 64>
    //Same alignment, but now allocates for double instead of int.
    template<typename U>
    struct rebind {
        using other = MyAlignedAllocator<U, Alignment>;
    };
};

/*STL needs these operators:
    All instances of this allocator are interchangeable and always equal.
    This is correct because the allocator is stateless.
    There is no internal memory pool, no pointer, no counter, no ownership.
    Two allocators with the same template parameters behave identically.
    Allocators compare equal if they have the same alignment*/
template<typename T, std::size_t A>
bool operator==(const MyAlignedAllocator<T, A>&,
                const MyAlignedAllocator<T, A>&) noexcept { return true; }

template<typename T, std::size_t A>
bool operator!=(const MyAlignedAllocator<T, A>&,
                const MyAlignedAllocator<T, A>&) noexcept { return false; }


