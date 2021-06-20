//
// Created by quangbd on 22/12/2020.
//

#ifndef CKWS_MODEL_H
#define CKWS_MODEL_H

#include <vector>
#include <numeric>
#include <cstring>

extern "C" {
#include "tensorflow-lite/c/c_api.h"
}

class Model {
private:

    // Variable
    TfLiteInterpreter *interpreter{};
    TfLiteModel *model{};
    TfLiteInterpreterOptions *options{};

public:

    // Constructor
    Model(const char *model_buffer, size_t model_size);
    Model();

    std::vector<float> predict(const float *input_buffer);

    void close();
};


#endif //CKWS_MODEL_H
