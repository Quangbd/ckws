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
    float *input_buffer_queue;
    bool is_new_command = false;
    long previous_wakeup_time{};

    // Config threshold
    uint8_t count_threshold;
    float min_duration_between_wakeup;
    float min_time_buffer;

    // Constructor
    Kws(const char *storage_wav_path, uint16_t total_sample, uint8_t count_threshold,
        float min_duration_between_wakeup, float min_time_buffer);

public:
    static Kws &get_instance(const char *storage_wav_path = nullptr, uint16_t total_sample = 16000,
                             uint8_t count_threshold = 3, float min_duration_between_wakeup = 1.5,
                             float min_time_buffer = 1);

    int wakeup(const short *short_input_buffer, int length);

    void close();
};


#endif //CKWS_KWS_H
