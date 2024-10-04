#include <iostream>

#include <nex/core.hpp>
#include <nex/glfw_env.hpp>

using namespace nex;

Error Run() {
    auto env = GlfwEnvironment::Create();
    NEX_ERR_RETURN(env);

    while (!env->ShouldClose()) {
        env->MessagePump();
    }

    return {};
}

int main() {
    auto result = Run();
    return result.IsOk() ? 0 : -1;
}