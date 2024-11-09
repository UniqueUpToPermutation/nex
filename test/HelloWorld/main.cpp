#include <iostream>

#include <nex/core.hpp>
#include <nex/glfw_env.hpp>

using namespace nex;

Error Run() {
    Error err;

    auto env = NEX_ERR_UNWRAP(GlfwEnvironment::Create(), err);

    while (!env.ShouldClose()) {
        env.MessagePump();
    }

    return err;
}

int main() {
    auto result = Run();
    return result.IsOk() ? 0 : -1;
}