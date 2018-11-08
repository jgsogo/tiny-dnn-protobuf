
#include "c_api.h"

#include <iostream>
#include "tiny_cnn/tiny_cnn.h"

#include "messages/configuration.pb.h"
#include "messages/train_data.pb.h"
#include "messages/test_data.pb.h"
#include "messages/status.pb.h"
#include "protobuf_serialization.hpp"

using network_t=tiny_cnn::network<tiny_cnn::sequential>;

namespace {

    using network_layer_builder_t=std::map<std::tuple<std::string, std::string>, std::function<network_t& (network_t&, int32_t, int32_t)>>;
 
    template <template <typename> class layer, typename activation>
    network_t& func_helper(network_t& n, int32_t n_inputs, int32_t n_outputs) {
        n << layer<activation>(n_inputs, n_outputs);
        return n;
    }
    
    network_layer_builder_t populate_network_layer_builder() {
        using namespace tiny_cnn::layers;
        using namespace tiny_cnn::activation;
        network_layer_builder_t ret;
        // fully-connected
        ret[{"fully-connected", "identity"}] = func_helper<fc, identity>;
        ret[{"fully-connected", "sigmoid"}] = func_helper<fc, sigmoid>;
        ret[{"fully-connected", "relu"}] = func_helper<fc, relu>;
        ret[{"fully-connected", "leaky_relu"}] = func_helper<fc, leaky_relu>;
        ret[{"fully-connected", "elu"}] = func_helper<fc, elu>;
        ret[{"fully-connected", "softmax"}] = func_helper<fc, softmax>;
        ret[{"fully-connected", "tan_h"}] = func_helper<fc, tan_h>;
        ret[{"fully-connected", "tan_hp1m2"}] = func_helper<fc, tan_hp1m2>;

        // ....
        return ret;
    }
    
    network_layer_builder_t network_layer_builder = populate_network_layer_builder();
    
    network_t& add_layer(network_t& network, const tiny_dnn::Configuration::Layer& layer)
    {
        auto& layer_type = layer.type();
        auto& activation = layer.activation();
        auto it = network_layer_builder.find({layer_type, activation});
        if (it != network_layer_builder.end()) {
            it->second(network, layer.n_inputs(), layer.n_output());
            return network;
        }
        else {
            std::ostringstream os; os << "Unknown pair layer-type='" << layer_type << "', activation='" << activation << "'.";
            throw std::runtime_error(os.str());
        }
    }
}
    
/*
 template <class T>
 using conv = tiny_cnn::convolutional_layer<T>;
 
 template <class T>
 using max_pool = tiny_cnn::max_pooling_layer<T>;
 
 template <class T>
 using ave_pool = tiny_cnn::average_pooling_layer<T>;
 
 template <class T>
 using fc = tiny_cnn::fully_connected_layer<T>;
 
 template <class T>
 using dense = tiny_cnn::fully_connected_layer<T>;
 
 using add = tiny_cnn::elementwise_add_layer;
 
 using dropout = tiny_cnn::dropout_layer;
 
 using input = tiny_cnn::input_layer;
 
 template <class T>
 using lrn = tiny_cnn::lrn_layer<T>;
 
 using input = tiny_cnn::input_layer;
 
 using concat = tiny_cnn::concat_layer;
 
 template <class T>
 using deconv = tiny_cnn::deconvolutional_layer<T>;
 
 template <class T>
 using max_unpooling = tiny_cnn::max_unpooling_layer<T>;
 
 template <class T>
 using average_unpooling = tiny_cnn::average_unpooling_layer<T>;
 
 using batch_norm = tiny_cnn::batch_normalization_layer;
 
 using slice = tiny_cnn::slice_layer;
 
 using power = tiny_cnn::power_layer;

 */
    

void* network_build(const void* configuration_in) {
    std::cout << "[C++] network_build()" << std::endl;
    network_t* network = nullptr;
    try {
        tiny_dnn::Configuration config = Serialized<tiny_dnn::Configuration>::parse(configuration_in);
        network = new network_t();
        // Build the actual network
        for (auto& layer: config.layers()) {
            add_layer(*network, layer);
        }
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
    tiny_dnn::TrainData train_data = Serialized<tiny_dnn::TrainData>::parse(train_data_in);
    
    std::vector<tiny_cnn::vec_t> input_data;
    std::transform(train_data.input_data().begin(), train_data.input_data().end(),
                   std::back_inserter(input_data),
                   [](auto& item){
                       return tiny_cnn::vec_t(item.values().begin(), item.values().end());
                   });
    
    std::vector<tiny_cnn::vec_t> desired_out;
    std::transform(train_data.desired_data().begin(), train_data.desired_data().end(),
                   std::back_inserter(desired_out),
                   [](auto& item){
                       return tiny_cnn::vec_t(item.values().begin(), item.values().end());
                   });
    
    // TODO: Get optimization algorithm and mse from protobuf
    tiny_cnn::gradient_descent opt;
    network->fit<tiny_cnn::mse>(opt, input_data, desired_out, train_data.batch_size(), train_data.epochs());
    
    double loss = network->get_loss<tiny_cnn::mse>(input_data, desired_out);
    train_data.set_loss(loss);
    
    tiny_dnn::Status status;
    status.set_ok(true);
    train_callback(state, Serialized<tiny_dnn::TrainData>(train_data), Serialized<tiny_dnn::Status>(status));
}

void network_test(void* state, void* network_in, const void* test_data_in, callback_t test_callback) {
    std::cout << "[C++] network_test()" << std::endl;
    network_t* network = static_cast<network_t*>(network_in);
    tiny_dnn::TestData test_data = Serialized<tiny_dnn::TestData>::parse(test_data_in);
    
    std::vector<tiny_cnn::vec_t> inputs;
    std::transform(test_data.test_data().begin(), test_data.test_data().end(),
                   std::back_inserter(inputs),
                   [](auto& item){
                       return tiny_cnn::vec_t(item.values().begin(), item.values().end());
                   });
    for (auto& item: inputs) {
        auto result = network->predict(item);
        auto test_result = test_data.add_test_data();
        for (auto& it: result) {
            test_result->add_values(it);
        }
    }
    
    tiny_dnn::Status status;
    status.set_ok(true);
    test_callback(state, Serialized<tiny_dnn::TestData>(test_data), Serialized<tiny_dnn::Status>(status));
}
