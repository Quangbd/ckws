//
// Created by quangbd on 25/12/2020.
//

#ifndef CKWS_WAV_WRITER_H
#define CKWS_WAV_WRITER_H

#include <iostream>
#include <cmath>

void write_uint32(std::ostream &os, uint32_t i);

void write_uint16(std::ostream &os, uint16_t i);

void write_frames(const std::string &folder_path, float *data, int length);


#endif //CKWS_WAV_WRITER_H
