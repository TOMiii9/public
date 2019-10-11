#pragma once

#include <atomic>
#include <stdint.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include "nmmintrin.h" // for SSE4.2

union sse_f32x4 {
    __m128 a4;
    float  a[4];
};

using f32 = float;
using f64 = double;
using i32 = int;
using u32 = unsigned int;
using i64 = int64_t;
using u64 = uint64_t;
using i8  = char;
using u8  = unsigned char;

template <class T>
class AlignedAllocator {
  public:
    using value_type = T;

    AlignedAllocator() noexcept {
    }

    template <class U>
    AlignedAllocator(AlignedAllocator<U> const &) noexcept {
    }

    value_type *allocate(std::size_t n) {
        constexpr auto item_size  = sizeof(value_type);
        const auto     total_size = n * item_size;

        return static_cast<value_type *>(_aligned_malloc(total_size, alignof(T)));
    }

    void deallocate(value_type *p, std::size_t) noexcept {
        _aligned_free(p);
    }
};

template <class T, class U>
bool operator==(const AlignedAllocator<T> &lhs, const AlignedAllocator<U> &rhs) {
    return true;
}

template <class T, class U>
bool operator!=(const AlignedAllocator<T> &lhs, const AlignedAllocator<U> &rhs) {
    return false;
}

template <typename ItemType>
using Array_Of = std::vector<ItemType, AlignedAllocator<ItemType>>;

template <typename ItemType>
using list_of = std::list<ItemType, AlignedAllocator<ItemType>>;

template <typename KeyType, typename ItemType>
// using map_of = std::map<KeyType, ItemType, std::less<KeyType>, AlignedAllocator<std::pair<KeyType, ItemType>>>;
// TODO: FIX THIS LATER
using Map_Of = std::map<KeyType, ItemType>;

using string_array = Array_Of<std::string>;

using atomic_i32 = std::atomic<i32>;

using atomic_bool = std::atomic<bool>;

using string = std::string;

// this cost me 3 fucking hours
constexpr auto FUCKING_FL_EPSILON = 0.00001;

#define MATH_PI 3.14159265358979323846f

template <typename T1, typename T2>
constexpr auto is_eq(T1 a, T2 b) {
    return (std::abs(float(a) - float(b)) < FUCKING_FL_EPSILON);
}
