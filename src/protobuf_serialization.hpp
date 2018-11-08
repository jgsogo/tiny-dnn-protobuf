
#pragma once

#include "c_api.h"


template <typename T>
struct ProtoSerialized : public SerializedData
{
    ProtoSerialized(const T& t)
    {
        size = t.ByteSizeLong();
        data = malloc(size);
        t.SerializeToArray(data, size);
    }
    
    ~ProtoSerialized()
    {
        free(data);
    }

    static T parse(const SerializedData& data) {
        T t; t.ParseFromArray(data.data, data.size);
        return t;
    }
    
};
