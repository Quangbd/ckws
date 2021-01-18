//
// Created by quangbd on 22/12/2020.
//

#ifndef CKWS_KWS_H
#define CKWS_KWS_H

#include <vector>
#include <deque>
#include <chrono>
#include <numeric>
#include <cstring>
#include "wav_writer.h"

extern "C" {
#ifdef __ANDROID__
#include "includes/tensorflow-lite-android/c/c_api.h"
#else
#include "includes/tensorflow-lite-pc/c/c_api.h"
#endif
}

class Kws {
private:

    // Variable
    TfLiteInterpreter *interpreter;
    TfLiteModel *model;
    TfLiteInterpreterOptions *options;

    std::deque<float> wakeup_queue_scores;
    std::deque<long> wakeup_queue_timestamps;
    bool is_new_command = false;
    long previous_wakeup_time{};

    uint16_t total_sample;
    float *input_buffer_queue;

    // Config threshold
    uint8_t count_threshold;
    float avg_score_threshold;
    float max_score_threshold;
    float min_duration_between_wakeup;
    float min_time_buffer;
    const char *storage_wav_path;

    // Function
    Kws(const char *model_buffer, size_t model_size, const char *storage_wav_path,
        uint16_t total_sample, uint8_t count_threshold, float max_score_threshold,
        float avg_score_threshold, float min_duration_between_wakeup, float min_time_buffer);

public:
    static Kws &get_instance(const char *model_buffer, size_t model_size, const char *storage_wav_path = nullptr,
                             uint16_t total_sample = 16000,
                             uint8_t count_threshold = 5, float max_score_threshold = 0.7,
                             float avg_score_threshold = 0.5, float min_duration_between_wakeup = 1.5,
                             float min_time_buffer = 1);

    int wakeup(const short *short_input_buffer, int length);

    void close();
};


#endif //CKWS_KWS_H
