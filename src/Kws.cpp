//
// Created by quangbd on 03/02/2021.
//

#include "Kws.h"
#include <vector>
#include "m_lstm.h"
#include "m_ds_cnn.h"
#include "logging.h"
#include "wav_writer.h"

Kws::Kws(const char *storage_wav_path, uint16_t total_sample, uint8_t count_threshold,
         float min_duration_between_wakeup, float min_time_buffer) {
    ds_cnn = Model(reinterpret_cast<const char *>(ds_cnn3_tflite), ds_cnn3_tflite_len);
    lstm = Model(reinterpret_cast<const char *>(lstm1_tflite), lstm1_tflite_len);

    this->total_sample = total_sample;
    this->count_threshold = count_threshold;
    this->min_duration_between_wakeup = min_duration_between_wakeup;
    this->min_time_buffer = min_time_buffer;
    this->input_buffer_queue = (float *) malloc(total_sample * sizeof(float));
    this->storage_wav_path = storage_wav_path;
}

Kws &Kws::get_instance(const char *storage_wav_path, uint16_t total_sample, uint8_t count_threshold,
                       float min_duration_between_wakeup, float min_time_buffer) {
    static Kws kws(storage_wav_path, total_sample, count_threshold, min_duration_between_wakeup, min_time_buffer);
    return kws;
}

int Kws::wakeup(const short *short_input_buffer, int length) {
    auto *float_input_buffer = (float *) malloc(length * sizeof(float));
    for (int i = 0; i < length; i++) {
        float float_input = ((float) short_input_buffer[i] / SHRT_MAX);
        float_input_buffer[i] = float_input;
    }

    // Copy to buffer queue
    memcpy(&input_buffer_queue[0], &input_buffer_queue[length],
           (total_sample - length) * sizeof(float));
    memcpy(&input_buffer_queue[total_sample - length], float_input_buffer, length * sizeof(float));
    free(float_input_buffer);

    long current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    if (is_new_command) {
        is_new_command = false;
        wakeup_queue_timestamps.clear();
        wakeup_queue_scores.clear();
        return 0;
    }
    if (!wakeup_queue_timestamps.empty()
        && ((float) (current_timestamp - wakeup_queue_timestamps[0]) > min_time_buffer)) {
        wakeup_queue_timestamps.pop_front();
        wakeup_queue_scores.pop_front();
    }

    // ds_cnn predict
    std::vector<float> d_output = ds_cnn.predict(input_buffer_queue);
    float re_positive = d_output[0];
    float re_negative = d_output[1];

    if ((re_positive > re_negative) &&
        ((float) (current_timestamp - previous_wakeup_time) > min_duration_between_wakeup)) {
        wakeup_queue_scores.push_back(re_positive - re_negative);
        wakeup_queue_timestamps.push_back(current_timestamp);
        auto queue_size = wakeup_queue_scores.size();
        LOG_INFO("d_output: neg %f - pos %f - size %d - is_nc %d",
                 re_negative, re_positive, queue_size, is_new_command);
        if (!is_new_command && (queue_size > count_threshold)) {
            std::vector<float> l_output = lstm.predict(input_buffer_queue);
            float lre_positive = l_output[0];
            float lre_negative = l_output[1];
            auto result_score = lre_positive - lre_negative;
            LOG_INFO("l_output: %f - %f", lre_positive, lre_negative);
            if (result_score > 0.4) {
                is_new_command = true;
                if (storage_wav_path != nullptr) {
                    write_frames(storage_wav_path, input_buffer_queue, total_sample);
                }
                previous_wakeup_time = current_timestamp;
                is_new_command = true;

                LOG_INFO("Clean audio buffer");
                memset(&input_buffer_queue[0], 0, total_sample * sizeof(float));
                return (int) (result_score * 100);
            }
        }
    }
    return 0;
}

void Kws::close() {
    ds_cnn.close();
    lstm.close();
}


