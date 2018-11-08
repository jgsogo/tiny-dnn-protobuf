#pragma once

#include <functional>

// credit: https://stackoverflow.com/questions/18169180/whats-the-best-way-to-wrap-a-c-callback-with-a-c11-interface
// impl with variadics fails in VS :/ (bug reported to MSVC)

template <typename Result, typename... Args>
Result wrapper(void* state, Args... args)
{
    using FuncWrapper = std::function<Result (Args...)>;
    FuncWrapper& w = *reinterpret_cast<FuncWrapper*>(state);
    return w(args...);
}

template <typename Result, typename... Args>
auto make_wrapper(std::function<Result (Args...)>& func)
-> std::pair<Result (*)(void*, Args...), void*>
{
    void* state = reinterpret_cast<void*>(&func);
    return std::make_pair(&wrapper<Result, Args...>, state);
}

