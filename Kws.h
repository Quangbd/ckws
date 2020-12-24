//
// Created by quangbd on 22/12/2020.
//

#ifndef CKWS_KWS_H
#define CKWS_KWS_H

#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <numeric>
#include "constant.h"

extern "C" {
#include "includes/tensorflow-lite/c/c_api.h"
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
    float input_buffer_queue[TOTAL_SAMPLE]{};
    uint8_t is_infer;

    // Function
    Kws(const char *model_buffer, size_t model_size);

public:
    static Kws &get_instance(const char *model_buffer, size_t model_size);

    bool wakeup(const short *short_input_buffer, int length);

    void close();
};


#endif //CKWS_KWS_H
