
#pragma once

#include <stdint.h>
#include "tiny_dnn_c_api.h"

extern "C" {
    typedef void(*callback_t)(void* state, const void* data, const void* status);

    TINY_DNN_C_EXPORT void say_hello();
    
    TINY_DNN_C_EXPORT void* network_build(const void* configuration);
    TINY_DNN_C_EXPORT void network_destroy(void* handler);

    TINY_DNN_C_EXPORT void network_train(void* state, void* network, const void* train_data, callback_t train_callback);
    TINY_DNN_C_EXPORT void network_test(void* state, void* network, const void* test_data, callback_t test_callback);

}

