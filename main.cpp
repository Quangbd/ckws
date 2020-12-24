#include "Kws.h"
#include "ds_cnn.h"
#include <cstring>
#include <iostream>
#include "logging.h"

extern "C" {
void init() {
    Kws::get_instance(reinterpret_cast<const char *>(ds_cnn1_tflite), ds_cnn1_tflite_len);
    LOG_INFO("Init model have done");
}

bool wakeup(const short *input_buffer, size_t length) {
    Kws &kws = Kws::get_instance(nullptr, 0);
    // release
    return kws.wakeup(input_buffer, length);
}

void close() {
    Kws &kws = Kws::get_instance(nullptr, 0);
    kws.close();
}
}

int main() {
    Kws &kws = Kws::get_instance(reinterpret_cast<const char *>(ds_cnn1_tflite), ds_cnn1_tflite_len);
    short input_buffer[16000] = WAVE_DATA;

    long start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    for (int i = 0; i < 16000; i += 640) {
        short sub_input_buffer[640];
        memcpy(&sub_input_buffer[0], &input_buffer[i], 640 * sizeof(short));
        kws.wakeup(sub_input_buffer, 640);
    }

    long end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "infer " << end - start << std::endl;
    kws.close();
}