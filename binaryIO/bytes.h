#ifndef _BYTES_H
#define _BYTES_H

#if __cplusplus >= 202002L
#include <bit>
#endif

#if __cplusplus <= 202002L

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap_16(x) BSWAP_16(x)
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_32(x) swap16(x)
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif

#else

#include <byteswap.h>

#endif

namespace std {
    template<typename T>
    constexpr inline
    std::enable_if_t<std::is_integral_v<T>, T>
    byteswap(T value) noexcept {
        if constexpr (sizeof(T) == 1)
            return value;
        if constexpr (sizeof(T) == 2)
            return bswap_16(value);
        if constexpr (sizeof(T) == 4)
            return bswap_32(value);
        if constexpr (sizeof(T) == 8)
            return bswap_64(value);
        }
}
#endif

#include <cstring>
#include <type_traits>

#if __cplusplus < 202002L

namespace std {
    template <class To, class From>
    constexpr inline
    std::enable_if_t<
        sizeof(To) == sizeof(From) &&
        std::is_trivially_copyable_v<From> &&
        std::is_trivially_copyable_v<To>,To>
    bit_cast(const From& source) noexcept {
        static_assert(std::is_trivially_constructible_v<To>,
            "destination type must be trivially constructible");

        To dest;
        std::memcpy(&dest, &source, sizeof(dest));
        return dest;
    }
}

#endif

template <class T>
constexpr inline
std::enable_if_t<sizeof(T) == 1 && std::is_arithmetic_v<T>, T>
reverseBytes(const T x) noexcept { return x; }

template <class T>
constexpr inline
std::enable_if_t<sizeof(T) == 2 && std::is_arithmetic_v<T>, T>
reverseBytes(const T x) noexcept { return std::bit_cast<T>(std::byteswap(std::bit_cast<uint16_t>(x))); }

template <class T>
constexpr inline
std::enable_if_t<sizeof(T) == 4 && std::is_arithmetic_v<T>, T>
reverseBytes(const T x) noexcept { return std::bit_cast<T>(std::byteswap(std::bit_cast<uint32_t>(x))); }

template <class T>
constexpr inline
std::enable_if_t<sizeof(T) == 8 && std::is_arithmetic_v<T>, T>
reverseBytes(const T x) noexcept { return std::bit_cast<T>(std::byteswap(std::bit_cast<uint64_t>(x))); }

template <class T>
constexpr inline
std::enable_if_t<!std::is_arithmetic_v<T> ||
    !(sizeof(T) == 1 ||
      sizeof(T) == 2 ||
      sizeof(T) == 4 ||
      sizeof(T) == 8), T>
reverseBytes(const T x) noexcept {  []<bool flag=false>() {static_assert(flag,"only arithmetic type with 1, 2, 4 or 8 bytes can be reversed");}(); return x; }

#endif
