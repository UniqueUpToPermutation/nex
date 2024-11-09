#pragma once

#include <concepts>

namespace nex {
    template <typename F>
    requires std::invocable<F&>
    struct DeferedExecution{
        F invoker;
        ~DeferedExecution() {
            invoker();
        }
    };
}

#define NEX_DEFER(x) DeferedExecution a{ [&] { x; } }