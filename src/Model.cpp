//
// Created by quangbd on 22/12/2020.
//

#include "Model.h"
#include <algorithm>

Model::Model(const char *model_buffer, size_t model_size) {
    model = TfLiteModelCreate(model_buffer, model_size);
    options = TfLiteInterpreterOptionsCreate();

    // Create the interpreter.
    interpreter = TfLiteInterpreterCreate(model, options);
    TfLiteInterpreterAllocateTensors(interpreter);
}

std::vector<float> Model::predict(const float *input_buffer) {
    TfLiteTensor *input_tensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
    const TfLiteTensor *output_tensor = TfLiteInterpreterGetOutputTensor(interpreter,0);
    TfLiteTensorCopyFromBuffer(input_tensor,&input_buffer[0], input_tensor->bytes);
    TfLiteInterpreterInvoke(interpreter);
    std::vector<float> output((output_tensor->bytes) / sizeof(float));
    TfLiteTensorCopyToBuffer(output_tensor, &output[0], output_tensor->bytes);
    return output;
}

void Model::close() {
    TfLiteInterpreterDelete(interpreter);
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);
}

Model::Model() = default;




