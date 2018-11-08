
#pragma once

#include "c_api.h"


template <typename T>
struct Serialized : public SerializedData
{
    Serialized(const T& t)
    {
        size = t.ByteSizeLong();
        data = malloc(size);
        t.SerializeToArray(data, size);
    }
    
    ~Serialized()
    {
        free(data);
    }

    static T parse(const SerializedData& data) {
        T t; t.ParseFromArray(data.data, data.size);
        return t;
    }
    
};
