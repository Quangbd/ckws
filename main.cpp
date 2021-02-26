#include "Kws.h"
#include "logging.h"
#include "wav_data.h"


#ifdef __ANDROID__
#include <jni.h>
#else
#endif

extern "C" {
#ifdef __ANDROID__
JNIEXPORT jboolean JNICALL
Java_hino_konoha_hokages_Minato_loadNavtiveChakra(JNIEnv *env, __unused jobject thiz,
                                                  jstring output_wav_dir) {
    Kws::get_instance(nullptr);
    LOG_INFO("Init model have done");
    return true;
}

extern "C"
JNIEXPORT jint JNICALL
Java_hino_konoha_hokages_Minato_teleportation(JNIEnv *env, __unused jobject thiz,
                                              jshortArray input_buffer, jint length) {
    short *short_input_buffer = env->GetShortArrayElements(input_buffer, nullptr);
    Kws &kws = Kws::get_instance(nullptr);
    int result = kws.wakeup(short_input_buffer, length);
    env->ReleaseShortArrayElements(input_buffer, short_input_buffer, 0);
    return result;
}
#else

void c_kws_init(const char *storage_wav_path = nullptr) {
    Kws::get_instance(storage_wav_path);
    LOG_DEBUG("Init model have done");
}

int c_kws_wakeup(const short *input_buffer, size_t length) {
    Kws &kws = Kws::get_instance(nullptr, 0);
    return kws.wakeup(input_buffer, length);
}

void c_kws_close() {
    Kws &kws = Kws::get_instance(nullptr, 0);
    kws.close();
}
#endif
}

#ifdef __ANDROID__
#else

int main() {
#define TOTAL_SAMPLE 16000
#define STEP 640

    Kws kws = Kws::get_instance(nullptr, TOTAL_SAMPLE);

    long start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    short input_buffer[TOTAL_SAMPLE] = WAVE_DATA;
    uint8_t count = 0;
    for (int i = 0; i < TOTAL_SAMPLE; i += STEP) {
        short sub_input_buffer[STEP];
        memcpy(&sub_input_buffer[0], &input_buffer[i], STEP * sizeof(short));
        kws.wakeup(sub_input_buffer, STEP);
        count += 1;
    }

    long end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    LOG_DEBUG("Infer duration: %lu - time: %d", end - start, count);
    kws.close();
}

#endif