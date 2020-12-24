//
// Created by quangbd on 22/12/2020.
//

#include "Kws.h"
#include <string>
#include "logging.h"


Kws::Kws(const char *model_buffer, size_t model_size) {
    model = TfLiteModelCreate(model_buffer, model_size);
    options = TfLiteInterpreterOptionsCreate();

    // Create the interpreter.
    interpreter = TfLiteInterpreterCreate(model, options);
    TfLiteInterpreterAllocateTensors(interpreter);

    total_sample = TOTAL_SAMPLE;
    is_infer = 0;
}

Kws &Kws::get_instance(const char *model_buffer, size_t model_size) {
    static Kws kws(model_buffer, model_size);
    return kws;
}

bool Kws::wakeup(const short *short_input_buffer, int length) {
    auto *float_input_buffer = (float *) malloc(length * sizeof(float));
    for (int i = 0; i < length; i++) {
        float float_input = ((float) short_input_buffer[i] / MAX_SHORT);
        float_input_buffer[i] = float_input;
    }

    // Copy to buffer queue
    memcpy(&input_buffer_queue[0], &input_buffer_queue[length],
           (total_sample - length) * sizeof(float));
    memcpy(&input_buffer_queue[total_sample - length], float_input_buffer, length * sizeof(float));
    free(float_input_buffer);

//    is_infer++;
//    if (is_infer != TIME_FOR_INFER) {
//        return false;
//    } else {
//        is_infer = 0;
//    }

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
        return false;
    } else if (!wakeup_queue_timestamps.empty()
               && ((float) (current_timestamp - wakeup_queue_timestamps[0]) > MIN_TIME_BUFFER)) {
        wakeup_queue_timestamps.pop_front();
        wakeup_queue_scores.pop_front();
    }

    if ((output[1] > output[0]) && ((float) (current_timestamp - previous_wakeup_time) > MIN_TIME_BETWEEN_WAKEUP)) {
        LOG_INFO("Score N_%f - P_%f - %lu", output[0], output[1], wakeup_queue_scores.size());
        wakeup_queue_scores.push_back(output[1]);
        wakeup_queue_timestamps.push_back(current_timestamp);
        auto queue_size = wakeup_queue_timestamps.size();
        auto avg_score = std::accumulate(wakeup_queue_scores.begin(),
                                         wakeup_queue_scores.end(), 0.0) / queue_size;
        if (!is_new_command && (wakeup_queue_scores.size() > LIMIT_FREQUENTLY) && (avg_score > LIMIT_AVG_SCORE)) {
            LOG_INFO("Wakeup core - %f", avg_score);
            previous_wakeup_time = current_timestamp;
            is_new_command = true;
            return true;
        }
    }
    return false;
}

void Kws::close() {
    TfLiteInterpreterDelete(interpreter);
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);
}


