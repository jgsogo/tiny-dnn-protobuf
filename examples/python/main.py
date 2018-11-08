

import ctypes
from configuration_pb2 import Configuration
from train_data_pb2 import TrainData
from test_data_pb2 import TestData


class Serialized(ctypes.Structure):
    _fields_ = [
        ("data", ctypes.c_void_p),
        ("size", ctypes.c_longlong),
    ]

    @classmethod
    def build(cls, proto):
        s = proto.SerializeToString()
        serialized = Serialized()
        serialized.data = ctypes.cast(s, ctypes.c_void_p)
        serialized.size = len(s)
        return serialized

    def parse(self, proto_class):
        proto = proto_class()
        proto.ParseFromString(ctypes.string_at(self.data, self.size))
        return proto


if __name__ == '__main__':
    print("Run TINY_DNN behind a C interface")

    path_to_dll = r"/Users/jgsogo/dev/projects/tiny-dnn-protobuf/build/lib/libtiny_dnn_c.dylib"
    lib = ctypes.CDLL(path_to_dll)

    # say hello
    lib.say_hello()

    # Configure network
    configuration = Configuration()
    configuration.id = "Id of configuration!"
    layer = configuration.layers.add()
    layer.type = "fully-connected"
    layer.activation = "tan_h"
    layer.n_inputs = 2
    layer.n_output = 3

    layer = configuration.layers.add()
    layer.type = "fully-connected"
    layer.activation = "tan_h"
    layer.n_inputs = 3
    layer.n_output = 1

    network_build = lib.network_build
    network_build.argtypes = [ctypes.POINTER(Serialized)]
    network_build.restype = ctypes.c_void_p
    h = network_build(Serialized.build(configuration))

    # Train the network
    train_data = TrainData()
    train_data.batch_size = 1
    train_data.epochs = 1000
    for item in [[0, 0], [0, 1], [1, 0], [1, 1]]:
        input_item = train_data.input_data.add()
        input_item.values.extend(item)
    for item in [[0], [1], [1], [0]]:
        desired_item = train_data.desired_data.add()
        desired_item.values.extend(item)

    def callback(a, b, c):
        print("Done training!")

    CMPFUNC = ctypes.CFUNCTYPE(ctypes.c_voidp, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p)
    network_train = lib.network_train
    network_train.restype = None
    network_train.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(Serialized), CMPFUNC]
    network_train(None, h, Serialized.build(train_data), CMPFUNC(callback))

    # Test the network
    test_data = TestData()
    for item in [[0, 0], [0, 1], [1, 0], [1, 1]]:
        input_item = test_data.test_data.add()
        input_item.values.extend(item)

    def callback(_, data, status):
        ss = ctypes.cast(data, ctypes.POINTER(Serialized))
        ss = ss[0]
        proto = ss.parse(proto_class=TestData)
        print(proto)
        print("Done test!")

    network_test = lib.network_test
    network_test.restype = None
    network_test.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(Serialized), CMPFUNC]
    network_test(None, h, Serialized.build(test_data), CMPFUNC(callback))

    # Destroy network
    network_destroy = lib.network_destroy
    network_destroy.restype = None
    network_destroy.argtypes = [ctypes.c_void_p]
    network_destroy(h)

