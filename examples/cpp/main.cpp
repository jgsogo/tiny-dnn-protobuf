
#include <iostream>

#include "messages/configuration.pb.h"
#include "messages/train_data.pb.h"
#include "messages/test_data.pb.h"
#include "messages/status.pb.h"

#include "../../src/c_api.h"
#include "../../src/protobuf_serialization.hpp"
#include "../../src/func_wrapper.hpp"

int main() {
    std::cout << "TinyDNN with C interface (using protobufs)\n";

    tiny_dnn::Configuration configuration;
    configuration.set_id("Network");
    
    auto layer1 = configuration.add_layers();
    layer1->set_type("fully-connected");
    layer1->set_activation("tan_h");
    layer1->set_n_inputs(2);
    layer1->set_n_output(3);
    
    auto layer2 = configuration.add_layers();
    layer2->set_type("fully-connected");
    layer2->set_activation("tan_h");
    layer2->set_n_inputs(3);
    layer2->set_n_output(1);
    
    // Create the network
    void* network = network_build(Serialized(configuration));
    
    // Train it
    {
        tiny_dnn::TrainData train_data;
        train_data.set_batch_size(1);
        train_data.set_epochs(1000);
        for (auto item: std::vector<std::vector<int>>({ {0,0}, {0,1}, {1,0}, {1,1} })) {
            auto input_item = train_data.add_input_data();
            for (auto& it: item) {
                input_item->add_values(it);
            }
        }
        for (auto item: std::vector<std::vector<int>>({ {0},   {1}, {1},     {0} })) {
            auto input_item = train_data.add_desired_data();
            for (auto& it: item) {
                input_item->add_values(it);
            }
        }
        std::function<void(const SerializedData&, const SerializedData&)> callback_train =
        [](const SerializedData& data, const SerializedData& status_in) {
            tiny_dnn::Status status = Serialized<tiny_dnn::Status>::parse(status_in);
            if (status.ok()) {
                tiny_dnn::TrainData train_data = Serialized<tiny_dnn::TrainData>::parse(data);
            }
            else {
                std::cerr << "ERROR: " << status.error_message() << std::endl;
            }
        };
        auto [callback, state] = make_wrapper(callback_train);
        network_train(state, network, Serialized(train_data), callback);
    }
    
    // Test it
    {
        tiny_dnn::TestData test_data;
        for (auto item: std::vector<std::vector<int>>({ {0,0}, {0,1}, {1,0}, {1,1} })) {
            auto input_item = test_data.add_test_data();
            for (auto& it: item) {
                input_item->add_values(it);
            }
        }
        std::function<void(const SerializedData&, const SerializedData&)> callback_test = [](const SerializedData& data, const SerializedData& status_in) {
            tiny_dnn::Status status = Serialized<tiny_dnn::Status>::parse(status_in);
            if (status.ok()) {
                tiny_dnn::TestData test_data = Serialized<tiny_dnn::TestData>::parse(data);
                std::cout << test_data.DebugString() << std::endl;
            }
            else {
                std::cerr << "ERROR: " << status.error_message() << std::endl;
            }
        };
        auto [callback, state] = make_wrapper(callback_test);
        network_test(state, network, Serialized(test_data), callback);
    }
    network_destroy(network);
}
