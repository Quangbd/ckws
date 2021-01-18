#include "Kws.h"
#include "ds_cnn.h"
#include "logging.h"
#include "wav_data.h"

#ifdef __ANDROID__
#include <jni.h>
#else
#endif

extern "C" {
void init(const char *storage_wav_path = nullptr) {
    Kws::get_instance(reinterpret_cast<const char *>(ds_cnn1_tflite), ds_cnn1_tflite_len, storage_wav_path);
    LOG_DEBUG("Init model have done");
}

int wakeup(const short *input_buffer, size_t length) {
    Kws &kws = Kws::get_instance(nullptr, 0);
    return kws.wakeup(input_buffer, length);
}

void close() {
    Kws &kws = Kws::get_instance(nullptr, 0);
    kws.close();
}


#ifdef __ANDROID__
JNIEXPORT jboolean JNICALL
Java_hino_konoha_hokages_Minato_loadNavtiveChakra(JNIEnv *env, __unused jobject thiz,
                                                  jstring output_wav_dir) {
    Kws::get_instance(reinterpret_cast<const char *>(ds_cnn1_tflite), ds_cnn1_tflite_len, nullptr);
    LOG_INFO("Init model have done");
    return true;
}

extern "C"
JNIEXPORT jint JNICALL
Java_hino_konoha_hokages_Minato_teleportation(JNIEnv *env, __unused jobject thiz,
                                              jshortArray input_buffer, jint length) {
    short *short_input_buffer = env->GetShortArrayElements(input_buffer, nullptr);
    Kws &kws = Kws::get_instance(nullptr, 0);
    return kws.wakeup(short_input_buffer, length);
}
#else
#endif
}

int main() {
    Kws &kws = Kws::get_instance(reinterpret_cast<const char *>(ds_cnn1_tflite), ds_cnn1_tflite_len,
                                 "/Users/quangbd/Desktop");
    short input_buffer[16000] = WAVE_DATA;

    long start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    uint8_t count = 0;
    for (int i = 0; i < 16000; i += 640) {
        short sub_input_buffer[640];
        memcpy(&sub_input_buffer[0], &input_buffer[i], 640 * sizeof(short));
        kws.wakeup(sub_input_buffer, 640);
        count += 1;
    }

    long end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    LOG_DEBUG("Infer duration: %lu - time: %d", end - start, count);
    kws.close();
}