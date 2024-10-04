#include <nex/glfw_env.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>

using namespace nex;

GlfwEnvironment* GlfwEnvironment::_gSingleton = nullptr;

void GlfwEnvironment::ErrorHandler(int error, const char* desc) {
    NEX_ERR_SET(_gSingleton->_error, (GlfwError{error, desc}));
}

void GlfwEnvironment::ResizeHandler(GLFWwindow* window, int width, int height) {
    if (window == _gSingleton->GetWindow()) {
        _gSingleton->_surfaceDirty = true;
    }
}

GlfwEnvironment::GlfwEnvironment(Params params) : 
    _params(params),
    _window(nullptr, &glfwDestroyWindow) {
    glfwSetTime(0);
}

GlfwEnvironment::GlfwEnvironment(GlfwEnvironment&& other) : GlfwEnvironment() {
    *this = std::move(other);
}

GlfwEnvironment& GlfwEnvironment::operator=(GlfwEnvironment&& other) {
    if (&other == _gSingleton) {
        _gSingleton = this;
    }
    _window = std::move(other._window);
    _error = std::move(other._error);
    _params = std::move(other._params);

    return *this;
}

GlfwEnvironment::~GlfwEnvironment() {
    if (this == _gSingleton) {
        Destroy();
        _gSingleton = nullptr;
    }
}

Expected<GlfwEnvironment> GlfwEnvironment::Create(Params params) {
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    GlfwEnvironment result(params);
    result._gSingleton = &result;

    PLOG_INFO << "Initializing GLFW...";
    glfwSetErrorCallback(GlfwEnvironment::ErrorHandler);
    
    glfwInit();
    NEX_EXP_RETURN(result._error);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto window = glfwCreateWindow(1920, 1080, result._params.windowTitle.c_str(), nullptr, nullptr);      
    NEX_EXP_RETURN(result._error);

    glfwSetFramebufferSizeCallback(window, GlfwEnvironment::ResizeHandler);
    result._window.reset(window);

    return Expected<GlfwEnvironment>(std::move(result));
}

Error GlfwEnvironment::Destroy() {
    PLOG_INFO << "Shutting Down GLFW...";

    _error = {};
    _window.reset();

    glfwTerminate();
    NEX_ERR_RETURN(_error);
    return {};
}

glm::vec2 GlfwEnvironment::GetViewport() const {
    int x, y;
    glfwGetFramebufferSize(_window.get(), &x, &y);
    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

bool GlfwEnvironment::ShouldClose() const {
    if (_window)
        return glfwWindowShouldClose(_window.get());
    else 
        return true;
}

Error GlfwEnvironment::MessagePump() {
    // Update timer
    if (!_isTimerInitialized) {
        glfwSetTime(0);
    }
    _isTimerInitialized = true;

    auto newTime = glfwGetTime();
    _deltaTime = newTime - _lastTime;
    _lastTime = newTime;

    // Update glfw
    _error = {};
    glfwPollEvents();
    NEX_ERR_RETURN(_error);
    return {};
}

double GlfwEnvironment::GetTime() const {
    return _lastTime;
}
double GlfwEnvironment::GetDeltaTime() const {
    return _deltaTime;
}

bool GlfwEnvironment::IsSurfaceDirty() const {
    return _surfaceDirty;
}
void GlfwEnvironment::ResetSurfaceDirty() {
    _surfaceDirty = false;
}

GLFWwindow* GlfwEnvironment::GetWindow() {
    return _window.get();
}
