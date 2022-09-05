/*
 * This file is part of the Cpp utils distribution (https://github.com/feodorp/Cpp)
 * Copyright (C) 2022 Feodor Pisnitchenko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _binaryIO_h
#define _binaryIO_h

#include <type_traits>
#include <algorithm>
#include <filesystem>
#include <cstddef>
#include <fstream>
#include <memory>
#include "bytes.h"

// check if system is Big/Little endian
static_assert(std::endian::native == std::endian::big || std::endian::native == std::endian::little);

namespace binIO {

     /** Table from C++ fstream header
       *
       *  +---------------------------------------------------------+
       *  |   Read   |   Write  | Truncate |  Append  |   stdio     |
       *  +---------------------------------------------------------+
       *  |          |    +     |         |           |      w      |
       *  |          |    +     |         |      +    |      a      |
       *  |          |          |         |      +    |      a      |
       *  |          |    +     |   +     |           |      w      |
       *  |    +     |          |         |           |      r      |
       *  |    +     |    +     |         |           |      r+     |
       *  |    +     |    +     |   +     |           |      w+     |
       *  |    +     |    +     |         |      +    |      a+     |
       *  |    +     |          |         |      +    |      a+     |
       *  |---------------------------------------------------------|
       *
       */

    using FileMode = std::ios::openmode;
    constexpr FileMode Read = std::ios::in;                     // read mode
    constexpr FileMode Write = std::ios::out;                   // write mode (override or criate new file)
    constexpr FileMode Append = std::ios::app;                  // append flag (append at the end of file)
    constexpr FileMode Truncate = std::ios::trunc;              // truncate flag (append at the end of file)

    template<FileMode _Mode>
    class BinaryFile
    {
        private:
            static constexpr FileMode mode_ = _Mode;
            std::filesystem::path path_;
            std::unique_ptr<std::fstream> file_;

        public:
            BinaryFile() : path_(), file_(std::make_unique<std::fstream>()) {}
            explicit BinaryFile(const char* path) : BinaryFile(std::filesystem::path(path)) {}
            explicit BinaryFile(const std::string& path) : BinaryFile(std::filesystem::path(path)) {}
            explicit BinaryFile(const std::filesystem::path& path)
            : path_(), file_(std::make_unique<std::fstream>())
            {
                open(path);
            }

            BinaryFile(const BinaryFile&) = delete;

            BinaryFile(BinaryFile&& other) noexcept :  path_(std::move(other.path_)), file_(std::move(other.file_)) {}

            BinaryFile& operator=(const BinaryFile&) = delete;
            inline BinaryFile& operator=(BinaryFile&& other) noexcept
            {
                path_ = std::move(other.path_);
                file_ = std::move(other.file_);
                return *this;
            }

            ~BinaryFile()
            {
                if(file_->is_open())
                    file_->close();
            }

            // Check if file is open
            [[nodiscard]] inline bool is_open() const { return file_->is_open(); }

            // Open file
            inline void open(const char* path) { open(std::filesystem::path(path)); }
            inline void open(const std::string& path) { open(std::filesystem::path(path)); }
            inline void open(const std::filesystem::path& path)
            {
                if (is_open())
                    throw std::runtime_error("BinaryFile has already been opened. Close it before open again.");

                if constexpr (!(mode_ & (Write | Append)))
                    if(!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
                        throw std::runtime_error("Invalid path: " + path.generic_string());

                path_ = path;
                file_->exceptions(std::fstream::failbit | std::fstream::badbit);
                file_->open(path.generic_string(), mode_|std::ios::binary);
            }

            // Close file
            inline void close()
            {
                file_->close();
                path_.clear();
            }

            [[nodiscard]] constexpr FileMode get_mode() { return mode_ ; }

            // Return the current head position.
            [[nodiscard]] inline std::streampos tell() { return file_->rdbuf()->pubseekoff(0, std::ios::cur); }

            // Move the read head to absolute "position"
            inline std::streampos seek(std::streampos position) { return file_->rdbuf()->pubseekpos(position); }
            // Move the read head to "offset" from base position "dir"
            inline std::streampos seek(std::streamoff offset, std::ios_base::seekdir dir=std::ios::beg) { return file_->rdbuf()->pubseekoff(offset, dir); }

            // Read "size" bytes to char* "buffer" (primary function):
            inline void read(char* buffer, std::streamsize size)
            {
                static_assert(!!(mode_ & Read), "BynaryFile wasn't set with Read flag");
                file_->read(buffer, size);
            }
            // Read "size" bytes to std::byte* "buffer":
            inline void read(std::byte* buffer, std::streamsize size) { read(reinterpret_cast<char*>(buffer), size); }
            // Read element of arithmetic type:
            template<std::endian en = std::endian::native, class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
            inline void read(T &x)
            {
                read(reinterpret_cast<char*>(&x), sizeof(T));
                if constexpr (en != std::endian::native)
                    x = reverseBytes(x);
            }
            // Read element of non-arithmetic type:
            template<std::endian en = std::endian::native, class T, std::enable_if_t<!std::is_arithmetic_v<T>, bool> = true>
            inline void read(T &x)
            {
                static_assert(en == std::endian::native, "Non arithmetic types can only be read as native endian");
                static_assert(std::is_trivially_copyable_v<T>,
                        "Only trivially copyable and trivially constructible classes can be read");
                read(reinterpret_cast<char*>(&x), sizeof(T));
            }
            // templated read function (returning type T element):
            template<class T, std::endian en = std::endian::native>
            [[nodiscard]] inline T read() {T x; read<en>(x); return x; }
            // Read N element to fixed size array x[N] of type T
            template<std::endian en = std::endian::native, class T, std::size_t N>
            inline void read(T (&x)[N])
            {
                static_assert(std::is_arithmetic_v<T>, "BinaryFile can read only arithmetic type arrays.");
                read(reinterpret_cast<char*>(x),sizeof(T)*N);
                if constexpr (en != std::endian::native) {
                    // for-range loop
                    for (T& y : x) {
                        y = reverseBytes(y);
                    }
                }
            }
            // Read elements to std::array<T,N>
            template<std::endian en = std::endian::native, class T, std::size_t N>
            inline void read(std::array<T,N> &x)
            {
                read<en>(reinterpret_cast<T(&)[N]>(*x.data()));
            }
            // Read n elements to pointer array *x of type T
            template<std::endian en = std::endian::native, class T, std::enable_if_t<!std::is_same_v<T,char>, bool> = true>
            inline void read(T* x, std::size_t n)
            {
                static_assert(std::is_arithmetic_v<T>, "BinaryFile can read only arithmetic type arrays.");
                read(reinterpret_cast<char*>(x),sizeof(T)*n);
                if constexpr (en != std::endian::native) {
                    //can't use for-range loop with pointers
                    for (std::size_t i=0; i<n; ++i) {
                        x[i] = reverseBytes(x[i]);
                    }
                }
            }

            // Write "size" bytes to char* "buffer" (primary function):
            inline void write(const char* buffer, std::streamsize size) const
            {
                static_assert(!!(mode_ & (Write | Append)), "BynaryFile wasn't set with Write or Append flags");
                file_->write(buffer, size);
            }
            // Write "size" bytes to std::byte* "buffer":
            inline void write(const std::byte* buffer, std::streamsize size) const { write(reinterpret_cast<const char*>(buffer), size); }
            // Write element of arithmetic type:
            template<std::endian en = std::endian::native, class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
            inline void write(const T &x) const
            {
                if constexpr (en != std::endian::native) {
                    T y = reverseBytes(x);
                    write(reinterpret_cast<const char*>(&y), sizeof(T));
                } else {
                    write(reinterpret_cast<const char*>(&x), sizeof(T));
                }
            }
            // Write element of non-arithmetic type:
            template<std::endian en = std::endian::native, class T, std::enable_if_t<!std::is_arithmetic_v<T>, bool> = true>
            inline void write(const T &x) const
            {
                static_assert(en == std::endian::native, "Non arithmetic types can only be written as native endian");
                static_assert(std::is_trivially_copyable_v<T>,
                        "Only trivially copyable classes can be written");
                write(reinterpret_cast<const char*>(&x), sizeof(T));
            }
            // Write N element to fixed size array x[N] of type T
            template<std::endian en = std::endian::native, class T, std::size_t N>
            inline void write(const T (&x)[N]) const
            {
                static_assert(std::is_arithmetic_v<T>, "BinaryFile can write only arithmetic type arrays.");
                if constexpr (en != std::endian::native) {
                    // for-range loop
                    for (T& y : x) {
                        y = reverseBytes(y);
                        write(reinterpret_cast<char*>(y),sizeof(T));
                    }
                }else {
                    write(reinterpret_cast<const char*>(x),sizeof(T)*N);
                }
            }
            // Write elements to std::array<T,N>
            template<std::endian en = std::endian::native, class T, std::size_t N>
            inline void write(const std::array<T,N> &x) const
            {
                write<en>(reinterpret_cast<const T(&)[N]>(*x.data()));
            }
            // Write n elements to pointer array *x of type T
            template<std::endian en = std::endian::native, class T, std::enable_if_t<!std::is_same_v<T,char>, bool> = true>
            inline void write(const T* x, std::size_t n)
            {
                static_assert(std::is_arithmetic_v<T>, "BinaryFile can write only arithmetic type arrays.");
                if constexpr (en != std::endian::native) {
                    //can't use for-range loop with pointers
                    T y;
                    for (std::size_t i=0; i<n; ++i) {
                        y = reverseBytes(x[i]);
                        write(reinterpret_cast<const char*>(y),sizeof(T));
                    }
                } else {
                    write(reinterpret_cast<const char*>(x),sizeof(T)*n);
                }
            }

    };
}

#endif
