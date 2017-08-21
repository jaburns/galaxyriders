#pragma once

#include <iostream>

namespace Debug
{
    template<typename none = void>
    static void log() {
        std::cout << std::endl;
    }

    template<typename T, typename ...Ts>
    static void log(T t, Ts... ts) {
        std::cout << t << "\t";
        log(ts...);
    }
}
