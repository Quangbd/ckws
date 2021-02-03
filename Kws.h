//
// Created by quangbd on 03/02/2021.
//

#ifndef CKWS_KWS_H
#define CKWS_KWS_H

#include "Model.h"
#include <deque>


class Kws {
private:
    Model ds_cnn;
    Model lstm;

    const char *storage_wav_path;
    std::deque<float> wakeup_queue_scores;
    std::deque<long> wakeup_queue_timestamps;
    uint16_t total_sample;
    float *input_buffer_queue{};

    // Constructor
    Kws(const char *storage_wav_path, uint16_t total_sample);

public:
    static Kws &get_instance(const char *storage_wav_path, uint16_t total_sample = 16000);

    int wakeup(const short *short_input_buffer, int length);

    void close();
};


#endif //CKWS_KWS_H
