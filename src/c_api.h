
#pragma once

#include <stdint.h>
#include "tiny_dnn_c_api.h"

extern "C" {

    struct SerializedData {
        void* data;
        int64_t size;
    };

    typedef void(*callback_t)(void* state, const SerializedData& data, const SerializedData& status);

    TINY_DNN_C_EXPORT void* network_build(const SerializedData& configuration);
    TINY_DNN_C_EXPORT void network_destroy(void* handler);

    TINY_DNN_C_EXPORT void network_train(void* state, void* network, const SerializedData& train_data, callback_t train_callback);
    TINY_DNN_C_EXPORT void network_test(void* state, void* network, const SerializedData& test_data, callback_t test_callback);

}

