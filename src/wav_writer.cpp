//
// Created by quangbd on 25/12/2020.
//

#include "wav_writer.h"
#include <fstream>
#include <chrono>

void write_uint32(std::ostream &os, uint32_t i) {
    union {
        char buf[4];
        uint32_t i;
    } u{};
    u.i = i;
    os.write(u.buf, 4);
}

void write_uint16(std::ostream &os, uint16_t i) {
    union {
        char buf[2];
        uint16_t i;
    } u{};
    u.i = i;
    os.write(u.buf, 2);
}

void write_frames(const std::string &folder_path, float *data, int length) {
    std::string sym = "/";
    std::string name_format = ".wav";
    long current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    std::string file_name_path = folder_path + sym + std::to_string(current_timestamp) + name_format;
    std::ofstream f_out(file_name_path.c_str(), std::ios::out | std::ios::binary);
    if (!f_out) {
        throw std::runtime_error("Cannot open file");
    }
    auto data_size = length * sizeof(float);

    f_out << "RIFF";
    write_uint32(f_out, data_size + 50);
    f_out << "WAVE";
    f_out << "fmt ";
    write_uint32(f_out, 18); // chunk size
    write_uint16(f_out, 3); // format
    write_uint16(f_out, 1); // channels
    write_uint32(f_out, 16000); // sample rate
    write_uint32(f_out, 16000 * 4); // bytes per rate
    write_uint16(f_out, 4); // number block align
    write_uint16(f_out, 32); // bits per sample
    write_uint16(f_out, 0); // size extension
    f_out << "fact";
    write_uint32(f_out, 4);
    write_uint32(f_out, length);
    f_out << "data";
    write_uint32(f_out, data_size);
    f_out.write(reinterpret_cast<const char *>(data), data_size);

    f_out.close();
}
