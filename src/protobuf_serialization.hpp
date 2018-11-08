

template <typename T>
struct Serialized
{
    void* data;
    int64_t size;
    
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
