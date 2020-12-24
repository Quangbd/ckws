#include "Kws.h"
#include "ds_cnn.h"

extern "C" {
void init() {
    Kws::get_instance(reinterpret_cast<const char *>(ds_cnn1_tflite), ds_cnn1_tflite_len);
}

bool wake_up(const short *input_buffer, size_t length) {
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
    short test[16000] = {};
    kws.wakeup(test, 16000);
    kws.close();
}