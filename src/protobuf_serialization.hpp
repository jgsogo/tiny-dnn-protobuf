
#pragma once

#include "c_api.h"

template <typename T>
T parse(const SerializedData& data) {
    T t; t.ParseFromArray(data.data, data.size);
    return t;
}


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

    operator const void*() const
    {
        return static_cast<const void*>(this);
    }
    
    static T parse(const void* handler)
    {
        const Serialized* serialized = static_cast<const Serialized*>(handler);
        T t; t.ParseFromArray(serialized->data, serialized->size);
        return t;
    }
};
