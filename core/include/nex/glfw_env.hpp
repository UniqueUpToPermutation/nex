#pragma once

#include <string>
#include <memory>

#include <nex/error.hpp>

#include <glm/glm.hpp>

struct GLFWwindow;

namespace nex {
    struct GlfwEnvironmentParams {
        std::string windowTitle = "Nex";
    };

    class GlfwEnvironment {
    private:
        static GlfwEnvironment* _gSingleton;

        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> _window;
        Error _error;
        bool _isTimerInitialized = false;
        double _lastTime = 0.0;
        double _deltaTime = 0.0;
        GlfwEnvironmentParams _params;
        bool _surfaceDirty = false;

        static void ErrorHandler(int error, const char* desc);
        static void ResizeHandler(GLFWwindow* window, int width, int height);

    public:
        using Params = GlfwEnvironmentParams;

        GlfwEnvironment(Params params = Params{});
        GlfwEnvironment(GlfwEnvironment const&) = delete;
        GlfwEnvironment(GlfwEnvironment&&);
        GlfwEnvironment& operator=(GlfwEnvironment const&) = delete;
        GlfwEnvironment& operator=(GlfwEnvironment&&);

        ~GlfwEnvironment();

        static Expected<GlfwEnvironment> Create(Params params = {});
        Error Destroy();

        glm::vec2 GetViewport() const;

        bool ShouldClose() const;
        Error MessagePump();

        double GetTime() const;
        double GetDeltaTime() const;

        bool IsSurfaceDirty() const;
        void ResetSurfaceDirty();

        glm::vec2 GetContentScale() const;
        GLFWwindow* GetWindow() const;
    };
}