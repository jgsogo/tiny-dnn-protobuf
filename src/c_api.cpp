
#include "c_api.h"

#include <iostream>
#include "tiny_cnn/tiny_cnn.h"

#include "messages/configuration.pb.h"
#include "messages/train_data.pb.h"
#include "messages/test_data.pb.h"
#include "messages/status.pb.h"
#include "protobuf_serialization.hpp"

using network_t=tiny_cnn::network<tiny_cnn::sequential>;

void* network_build(const void* configuration_in) {
    std::cout << "[C++] network_build()" << std::endl;
    network_t* network = nullptr;
    try {
        tiny_dnn::Configuration config = Serialized<tiny_dnn::Configuration>::parse(configuration_in);
        network = new network_t();
        // Build the actual network
        
    }
    catch(std::exception& e) {
        std::cerr << "[C++] Error build network: " << e.what() << std::endl;
    }
    return static_cast<void*>(network);
}

void network_destroy(void* handler) {
    std::cout << "[C++] network_destroy()" << std::endl;
    network_t* network = static_cast<network_t*>(handler);
    try {
        delete network;
    }
    catch(...) {}
}

void network_train(void* state, void* network_in, const void* train_data_in, callback_t train_callback) {
    std::cout << "[C++] network_train()" << std::endl;
    network_t* network = static_cast<network_t*>(network_in);
    tiny_dnn::TrainData config = Serialized<tiny_dnn::TrainData>::parse(train_data_in);
}

void network_test(void* state, void* network_in, const void* test_data_in, callback_t test_callback) {
    std::cout << "[C++] network_test()" << std::endl;
    network_t* network = static_cast<network_t*>(network_in);
    tiny_dnn::TestData config = Serialized<tiny_dnn::TestData>::parse(test_data_in);
}
