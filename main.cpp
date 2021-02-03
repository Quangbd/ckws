#include "Model.h"
#include "m_lstm.h"
#include "m_ds_cnn.h"
#include "logging.h"
#include "wav_data.h"


#ifdef __ANDROID__
#include <jni.h>
#else
#endif

//extern "C" {
//#ifdef __ANDROID__
//JNIEXPORT jboolean JNICALL
//Java_hino_konoha_hokages_Minato_loadNavtiveChakra(JNIEnv *env, __unused jobject thiz,
//                                                  jstring output_wav_dir) {
//    Kws::get_instance(reinterpret_cast<const char *>(lstm1_tflite), lstm1_tflite_len, nullptr);
//    LOG_INFO("Init model have done");
//    return true;
//}
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_hino_konoha_hokages_Minato_teleportation(JNIEnv *env, __unused jobject thiz,
//                                              jshortArray input_buffer, jint length) {
//    short *short_input_buffer = env->GetShortArrayElements(input_buffer, nullptr);
//    Kws &kws = Kws::get_instance(nullptr, 0);
//    int result = kws.wakeup(short_input_buffer, length);
//    env->ReleaseShortArrayElements(input_buffer, short_input_buffer, 0);
//    return result;
//}
//#else
//
//void c_kws_init(const char *storage_wav_path = nullptr) {
//    Model::get_instance(reinterpret_cast<const char *>(lstm1_tflite), lstm1_tflite_len, storage_wav_path);
//    LOG_DEBUG("Init model have done");
//}
//
//int c_kws_wakeup(const short *input_buffer, size_t length) {
//    Model &kws = Model::get_instance(nullptr, 0);
//    return kws.wakeup(input_buffer, length);
//}
//
//void c_kws_close() {
//    Model &kws = Model::get_instance(nullptr, 0);
//    kws.close();
//}
//#endif
//}
//
//#ifdef __ANDROID__
//#else

int main() {
#define TOTAL_SAMPLE 16000
    Model model = Model(reinterpret_cast<const char *>(lstm1_tflite), lstm1_tflite_len);
    short short_input_buffer[TOTAL_SAMPLE] = WAVE_DATA;
    auto *float_input_buffer = (float *) malloc(TOTAL_SAMPLE * sizeof(float));
    for (int i = 0; i < TOTAL_SAMPLE; i++) {
        float float_input = ((float) short_input_buffer[i] / SHRT_MAX);
        float_input_buffer[i] = float_input;
    }
    auto result = model.predict(float_input_buffer);
    LOG_INFO("%f - %f", result[0], result[1]);
    model.close();
    free(float_input_buffer);
}

//#endif