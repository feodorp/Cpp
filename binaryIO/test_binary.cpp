#include "binaryIO.h"
#include <iostream>
#include <cassert>
#include <iomanip>
#include <string_view>


using namespace binIO;

constexpr char testData[] = {
    static_cast<char>(0x20), static_cast<char>(0xA0), static_cast<char>(0x24), // 1
    static_cast<char>(0x29), static_cast<char>(0xC3), static_cast<char>(0x18), // 2
    static_cast<char>(0xCF), static_cast<char>(0x28), static_cast<char>(0x23), // 3
    static_cast<char>(0x9F), static_cast<char>(0x24), static_cast<char>(0x29), // 4
    static_cast<char>(0xC3), static_cast<char>(0x18), static_cast<char>(0xFD), // 5
    static_cast<char>(0xBE), static_cast<char>(0x1F), static_cast<char>(0x85), // 6
    static_cast<char>(0xEB), static_cast<char>(0x51), static_cast<char>(0xB8), // 7
    static_cast<char>(0x1E), static_cast<char>(0x09), static_cast<char>(0x40), // 8
    static_cast<char>(0x7B), static_cast<char>(0x14), static_cast<char>(0x2E), // 9
    static_cast<char>(0x40), static_cast<char>(0x54), static_cast<char>(0x65), // 10
    static_cast<char>(0x73), static_cast<char>(0x74), static_cast<char>(0x74), // 11
    static_cast<char>(0x73), static_cast<char>(0x65), static_cast<char>(0x54), // 12
    static_cast<char>(0x54), static_cast<char>(0x65), static_cast<char>(0x73), // 13
    static_cast<char>(0x74), static_cast<char>(0x00), static_cast<char>(0x74), // 14
    static_cast<char>(0x73), static_cast<char>(0x65), static_cast<char>(0x54), // 15
    static_cast<char>(0x00), static_cast<char>(0x00), static_cast<char>(0x00), // 16
    static_cast<char>(0x00), static_cast<char>(0x00), static_cast<char>(0x00), // 17
    static_cast<char>(0x00), static_cast<char>(0x00), static_cast<char>(0x00), // 18
    static_cast<char>(0x00), static_cast<char>(0x00), static_cast<char>(0x00), // 19
    static_cast<char>(0x00), static_cast<char>(0x00), static_cast<char>(0x00), // 20
    static_cast<char>(0x00), static_cast<char>(0x00), static_cast<char>(0x54), // 21
    static_cast<char>(0x65), static_cast<char>(0x44)                           // 22
};
int main()
{
    if(!std::filesystem::exists("testData.bin") || !std::filesystem::is_regular_file("testData.bin") || std::filesystem::file_size("testData.bin")<=0) {
        std::ofstream ofs("testData.bin", std::ios::binary | std::ios::out);
        ofs.exceptions(std::ios::failbit | std::ios::badbit);
        if(!ofs.is_open())
            throw std::runtime_error("Failed to open temp testData file");
        ofs.write(testData, sizeof(testData));
        ofs.close();
        std::cout << "Binary file created" << std::endl;
    }

    char data[sizeof(testData)];

    std::cout << std::boolalpha;

    auto bf1 = BinaryFile<Append>();
    std::cout << "File size: " << std::filesystem::file_size("testData.bin") << std::endl;
    bf1.open("testData.bin");
    std::cout << "File is open as Append: " << bf1.is_open() << std::endl;
    auto pos = bf1.tell();
    std::cout << "Position: " << pos << std::endl;
    std::cout << "Can read: " << bool(bf1.get_mode() & binIO::Read) << std::endl;
    std::cout << "Can write: " << bool(bf1.get_mode() & (binIO::Write|binIO::Append)) <<std::endl;
    // bf1.read(data, 65);
    // bf1.seek(0);
    // bf1.write(testData,sizeof(testData));
    bf1.close();
    std::cout << std::endl;

    auto bf2 = BinaryFile<Read>();
    std::cout << "File size: " << std::filesystem::file_size("testData.bin") << std::endl;
    bf2.open("testData.bin");
    std::cout << "File is open as Read: " << bf2.is_open() << std::endl;
    pos = bf2.tell();
    std::cout << "Position: " << pos << std::endl;
    std::cout << "Can read: " << bool(bf2.get_mode() & binIO::Read) <<std::endl;
    std::cout << "Can write: " << bool(bf2.get_mode() & (binIO::Write|binIO::Append)) <<std::endl;
    // bf2.read(data, 65);
    // bf2.seek(0);
    // bf2.write(testData,65);
    bf2.close();
    std::cout << std::endl;

    auto bf21 = BinaryFile<Write>();
    std::cout << "File size: " << std::filesystem::file_size("testData.bin") << std::endl;
    bf21.open("testData.bin");
    std::cout << "File is open as Write: " << bf21.is_open() << std::endl;
    pos = bf21.tell();
    std::cout << "Position: " << pos << std::endl;
    std::cout << "Can read: " << bool(bf21.get_mode() & binIO::Read) <<std::endl;
    std::cout << "Can write: " << bool(bf21.get_mode() & (binIO::Write|binIO::Append)) <<std::endl;
    // bf21.read(data, 65);
    // bf21.seek(0);
    // bf21.write(testData,65);
    bf21.close();
    std::cout << std::endl;

    auto bf22 = BinaryFile<Write|Append>();
    std::cout << "File size: " << std::filesystem::file_size("testData.bin") << std::endl;
    bf22.open("testData.bin");
    std::cout << "File is open as Write and Append: " << bf22.is_open() << std::endl;
    pos = bf22.tell();
    std::cout << "Position: " << pos << std::endl;
    std::cout << "Can read: " << bool(bf22.get_mode() & binIO::Read) <<std::endl;
    std::cout << "Can write: " << bool(bf22.get_mode() & (binIO::Write|binIO::Append)) <<std::endl;
    // bf22.read(data, 65);
    // bf22.seek(0);
    // bf22.write(testData,65);
    bf22.close();
    std::cout << std::endl;

    auto bf3 = BinaryFile<Read|Write>();
    std::cout << "File size: " << std::filesystem::file_size("testData.bin") << std::endl;
    bf3.open("testData.bin");
    std::cout << "File is open as Read and Write: " << bf3.is_open() << std::endl;
    pos = bf3.tell();
    std::cout << "Position: " << pos << std::endl;
    std::cout << "Can read: " << bool(bf3.get_mode() & binIO::Read) <<std::endl;
    std::cout << "Can write: " << bool(bf3.get_mode() & (binIO::Write|binIO::Append)) <<std::endl;
    // bf3.read(data, 65);
    // bf3.seek(0);
    // bf3.write(testData,65);
    bf3.close();
    std::cout << std::endl;

    auto bf4 = BinaryFile<Read|Append>();
    std::cout << "File size: " << std::filesystem::file_size("testData.bin") << std::endl;
    bf4.open("testData.bin");
    std::cout << "File is open as Read and Append: " << bf4.is_open() << std::endl;
    pos = bf4.tell();
    std::cout << "Position: " << pos << std::endl;
    std::cout << "Can read: " << bool(bf4.get_mode() & binIO::Read) <<std::endl;
    std::cout << "Can write: " << bool(bf4.get_mode() & (binIO::Write|binIO::Append)) <<std::endl;
    // bf4.read(data, 65);
    // bf4.seek(0);
    // bf4.write(testData,65);
    bf4.close();
    std::cout << std::endl;

    if(!std::filesystem::exists("testData.bin") || !std::filesystem::is_regular_file("testData.bin") || std::filesystem::file_size("testData.bin")<=0) {
        std::ofstream ofs("testData.bin", std::ios::binary | std::ios::out);
        ofs.exceptions(std::ios::failbit | std::ios::badbit);
        if(!ofs.is_open())
            throw std::runtime_error("Failed to open temp testData file");
        ofs.write(testData, sizeof(testData));
        ofs.close();
        std::cout << "Binary file created" << std::endl;
    }

    auto filename = std::string("testData.bin");
    std::cout << "File size before open: " << std::filesystem::file_size("testData.bin") << std::endl;
    auto bf = BinaryFile<Read|Write>(filename);
    std::cout << "File size after open: " << std::filesystem::file_size("testData.bin") << std::endl;

    char buf[65];
    std::byte bufb[65];
    pos = bf.tell();
    std::cout << "Position: " << pos << std::endl;
    bf.read(buf,65);
    std::cout << "Read char array:" << std::endl << buf << std::endl;
    bf.seek(pos);
    std::cout << "Position: " << pos << std::endl;
    bf.read(bufb,65);
    std::cout << "Read std::byte array:" << std::endl << reinterpret_cast<char*>(bufb) << std::endl;

    std::cout << "File data as native endian float array:" << std::endl;
    for (int j=0;j<4;++j)
        std::cout << *reinterpret_cast<const float*>(testData+j*sizeof(float)) << " ";
    std::cout << std::endl;

    float i[4]{0.0};
    bf.seek(pos);
    std::cout << "Position: " << pos << std::endl;
    bf.read<std::endian::big>(&i[0],4);
    std::cout << "Read fixed size array float[4] as big endian:" << std::endl;
    for (int j=0;j<4;++j)
        std::cout << i[j] << " ";
    std::cout << std::endl;

    std::array<float,4> k{0.0};
    bf.seek(pos);
    std::cout << "Position: " << pos << std::endl;
    std::cout << "Read std::array<float,4> as native endian:" << std::endl;
    bf.read(k);
    for (int j=0;j<4;++j)
        std::cout << k[j] << " ";
    std::cout << std::endl;

    float x;
    struct T {float x; float y; void f(){};};
    bf.seek(pos);
    std::cout << "Position: " << pos << std::endl;
    x=bf.read<float, std::endian::big>();
    std::cout << "Read float scalar as big endian:" << std::endl;
    std::cout << x << std::endl;
    bf.seek(pos);
    std::cout << "Position: " << pos << std::endl;
    T z=bf.read<T>();
    std::cout << "Read structure with two float scalars:" << std::endl;
    std::cout << z.x << " " << z.y  << std::endl << std::endl;


    std::cout << "Can write: " << (bf.get_mode() & (binIO::Write|binIO::Append)) <<std::endl;
    bf.seek(pos);
    std::cout << "Position: " << pos << std::endl;
    bf.write(z);
    std::cout << "Write same structure:" << std::endl;
    std::cout << z.x << " " << z.y  << std::endl;
    bf.seek(pos);
    bf.read(z);
    std::cout << "Read again:" << std::endl;
    std::cout << z.x << " " << z.y  << std::endl << std::endl;

}
