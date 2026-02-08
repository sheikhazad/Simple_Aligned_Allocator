
================================================================================
README — MyAlignedAllocator
================================================================================

MyAlignedAllocator is an STL‑compatible allocator that uses C++17’s aligned
operator new/delete to guarantee cache‑line alignment for all allocated memory.
This improves cache locality and helps prevent false sharing in multi‑threaded
systems.

The allocator is stateless, so the rebind constructor is intentionally empty.
It includes the required value_type, rebind, and rebind constructor so containers
such as std::map, std::unordered_map, and std::list can allocate their internal
node types correctly.

--------------------------------------------------------------------------------
value_type and rebind
--------------------------------------------------------------------------------

Both value_type and rebind are required by the C++ Allocator concept.  
STL containers rely on these definitions to allocate elements and internal
structures.

--------------------------------------------------------------------------------
Why `using value_type = T;` is required
--------------------------------------------------------------------------------

Even if value_type is never referenced directly in user code, STL containers
depend on it. Every standard container (vector, map, unordered_map, deque, list,
etc.) expects an allocator to define:

    using value_type = T;

STL uses value_type for:
 - determining the type being allocated
 - constructing elements
 - destroying elements
 - performing allocator rebinding
 - allocating internal node types (e.g., std::pair<const Key, T> in std::map)

Without value_type, an allocator does not satisfy the Allocator concept, and
many STL containers will fail to compile.

--------------------------------------------------------------------------------
How STL containers use value_type
--------------------------------------------------------------------------------

STL code frequently accesses:

    typename Alloc::value_type
    Alloc::rebind<U>::other
    allocator_traits<Alloc>::value_type

allocator_traits extracts:
 - value_type
 - pointer
 - size_type
 - rebind
 - allocate
 - deallocate

This is why value_type must be present.

--------------------------------------------------------------------------------
Why STL containers need rebind
--------------------------------------------------------------------------------

STL containers often allocate internal types that differ from T.

Examples:

std::map<Key, Value> allocates:
 - std::pair<const Key, Value>
 - tree nodes
 - sentinel nodes

std::unordered_map<Key, Value> allocates:
 - buckets
 - node objects
 - internal bookkeeping structures

std::list<T> allocates:
 - node objects containing T

std::deque<T> allocates:
 - block pointers
 - internal buffers

To support this, the STL must be able to request:

    "An allocator for type U, even though the original allocator was defined for T."

This is exactly what rebind provides:

    template<typename U>
    struct rebind {
        using other = MyAlignedAllocator<U, Alignment>;
    };

--------------------------------------------------------------------------------
What happens if rebind is removed?
--------------------------------------------------------------------------------

Some containers may still compile, but many will not.

Container                  Works?   Reason
-------------------------  -------  -------------------------------------------
std::vector<T>            ✔️        Only allocates T
std::map<Key, Value>      ❌        Requires allocators for node types
std::unordered_map<K, V>  ❌        Requires allocators for buckets + nodes
std::list<T>              ❌        Allocates node objects
std::deque<T>             ❌        Allocates internal block structures

Without rebind, the allocator becomes non‑conforming and not STL‑compatible.

--------------------------------------------------------------------------------
Summary
--------------------------------------------------------------------------------

MyAlignedAllocator:

 - is STL‑compatible
 - uses C++17 aligned allocation
 - is stateless
 - prevents false sharing when used with aligned data structures
 - includes the required value_type, rebind, and rebind constructor
 - supports all standard containers, including node‑based ones

This design ensures correct behavior across the entire STL and provides
predictable, cache‑friendly memory allocation suitable for high‑performance
systems.

================================================================================
END OF README
================================================================================
