//
// Created by quangbd on 22/12/2020.
//

#include "Kws.h"
#include "logging.h"
#include <climits>
#include <algorithm>

Kws::Kws(const char *model_buffer, size_t model_size, const char *storage_wav_path,
         uint16_t total_sample, uint8_t count_threshold, float max_score_threshold,
         float avg_score_threshold, float min_duration_between_wakeup, float min_time_buffer) {
    model = TfLiteModelCreate(model_buffer, model_size);
    options = TfLiteInterpreterOptionsCreate();

    // Create the interpreter.
    interpreter = TfLiteInterpreterCreate(model, options);
    TfLiteInterpreterAllocateTensors(interpreter);

    this->total_sample = total_sample;
    this->count_threshold = count_threshold;
    this->max_score_threshold = max_score_threshold;
    this->avg_score_threshold = avg_score_threshold;
    this->min_duration_between_wakeup = min_duration_between_wakeup;
    this->min_time_buffer = min_time_buffer;
    this->input_buffer_queue = (float *) malloc(total_sample * sizeof(float));
    this->storage_wav_path = storage_wav_path;
}

Kws &Kws::get_instance(const char *model_buffer, size_t model_size, const char *storage_wav_path,
                       uint16_t total_sample, uint8_t count_threshold, float max_score_threshold,
                       float avg_score_threshold, float min_duration_between_wakeup, float min_time_buffer) {
    static Kws kws(model_buffer, model_size, storage_wav_path, total_sample, count_threshold, max_score_threshold,
                   avg_score_threshold, min_duration_between_wakeup, min_time_buffer);
    return kws;
}

int Kws::wakeup(const short *short_input_buffer, int length) {
    auto *float_input_buffer = (float *) malloc(length * sizeof(float));
//    std::transform(short_input_buffer, short_input_buffer + length, float_input_buffer,
//                   [](short s) { return (float) s / (float) SHRT_MAX; });
    for (int i = 0; i < length; i++) {
        float float_input = ((float) short_input_buffer[i] / SHRT_MAX);
        float_input_buffer[i] = float_input;
    }

    // Copy to buffer queue
    memcpy(&input_buffer_queue[0], &input_buffer_queue[length],
           (total_sample - length) * sizeof(float));
    memcpy(&input_buffer_queue[total_sample - length], float_input_buffer, length * sizeof(float));
    free(float_input_buffer);

    TfLiteTensor *input_tensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
    const TfLiteTensor *output_tensor = TfLiteInterpreterGetOutputTensor(interpreter,
                                                                         0);
    TfLiteTensorCopyFromBuffer(input_tensor,
                               &input_buffer_queue[0], input_tensor->bytes);
    TfLiteInterpreterInvoke(interpreter);
    std::vector<float> output((output_tensor->bytes) / sizeof(float));
    TfLiteTensorCopyToBuffer(output_tensor, &output[0], output_tensor->bytes);

    long current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    if (is_new_command) {
        is_new_command = false;
        wakeup_queue_timestamps.clear();
        wakeup_queue_scores.clear();
        return 0;
    } else if (!wakeup_queue_timestamps.empty()
               && ((float) (current_timestamp - wakeup_queue_timestamps[0]) > min_time_buffer)) {
        wakeup_queue_timestamps.pop_front();
        wakeup_queue_scores.pop_front();
    }

    if ((output[1] > output[0]) && ((float) (current_timestamp - previous_wakeup_time) > min_duration_between_wakeup)) {
        LOG_DEBUG("Score N_%f - P_%f - %lu", output[0], output[1], wakeup_queue_scores.size());
        wakeup_queue_scores.push_back(output[1] - output[0]);
        wakeup_queue_timestamps.push_back(current_timestamp);
        auto queue_size = wakeup_queue_scores.size();
        if (!is_new_command && (wakeup_queue_scores.size() > count_threshold)) {
            auto avg_score = std::accumulate(wakeup_queue_scores.begin(),
                                             wakeup_queue_scores.end(), 0.0) / queue_size;
            auto max_score = std::max_element(wakeup_queue_scores.begin(), wakeup_queue_scores.end())[0];
            if ((max_score > max_score_threshold) && (avg_score > avg_score_threshold)) {
                LOG_DEBUG("Wakeup core - %f", avg_score);
                if (storage_wav_path != nullptr) {
                    write_frames(storage_wav_path, input_buffer_queue, total_sample);
                }
                previous_wakeup_time = current_timestamp;
                is_new_command = true;
                return (int) (avg_score * 100);
            }
        }
    }
    return 0;
}

void Kws::close() {
    TfLiteInterpreterDelete(interpreter);
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);
    free(input_buffer_queue);
}



