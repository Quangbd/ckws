//
// Created by quangbd on 03/02/2021.
//

#include "Kws.h"
#include <vector>
#include "m_lstm.h"
#include "m_ds_cnn.h"

Kws::Kws(const char *storage_wav_path, uint16_t total_sample) {
    ds_cnn = Model(reinterpret_cast<const char *>(ds_cnn1_tflite), ds_cnn1_tflite_len);
    lstm = Model(reinterpret_cast<const char *>(lstm1_tflite), lstm1_tflite_len);

    this->storage_wav_path = storage_wav_path;
    this->total_sample = total_sample;
}

Kws &Kws::get_instance(const char *storage_wav_path, uint16_t total_sample) {
    static Kws kws(storage_wav_path, total_sample);
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

    // ds_cnn predict
    std::vector<float> d_output = ds_cnn.predict(input_buffer_queue);
    if (d_output[1] > d_output[0]) {

    }

    return 0;
}

void Kws::close() {
    ds_cnn.close();
    lstm.close();
}


