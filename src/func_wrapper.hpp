#pragma once

#include <functional>


inline void wrapper(void* state, const void* data, const void* status)
{
    typedef std::function<void(const void*, const void*)> FuncWrapper;
    FuncWrapper& w = *reinterpret_cast<FuncWrapper*>(state);
    w(data, status);
}

inline std::pair<void(*)(void*, const void*, const void*), void*> make_wrapper(std::function<void (const void*, const void*)>& func)
{
    void* state = reinterpret_cast<void*>(&func);
    return std::make_pair(&wrapper, state);
}



// credit: https://stackoverflow.com/questions/18169180/whats-the-best-way-to-wrap-a-c-callback-with-a-c11-interface
// impl with variadics fails in VS :/ (bug reported to MSVC)
/*
template <typename Result, typename... Args>
Result wrapper(void* state, Args... args)
{
    using FuncWrapper = std::function<Result (Args...)>;
    FuncWrapper& w = *reinterpret_cast<FuncWrapper*>(state);
    return w(args...);
}

template <typename Result, typename... Args>
auto make_wrapper(std::function<Result(Args...)>& func)
-> std::pair<Result (*)(Args...), void*>
{
    void* state = reinterpret_cast<void*>(&func);
    return std::make_pair(&wrapper<Result, Args...>, state);
}
*/
