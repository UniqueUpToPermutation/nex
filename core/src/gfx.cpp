#include <nex/gfx.hpp>

#include <EngineFactoryVk.h>
#include <LinuxNativeWindow.h>

#if PLATFORM_LINUX
#    define GLFW_EXPOSE_NATIVE_X11 1
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

using namespace nex;
using namespace dg;

Expected<GfxInitResult> nex::GfxInit(GLFWwindow* window) {
    auto* pFactoryVk = dg::GetEngineFactoryVk();

#if PLATFORM_LINUX
    LinuxNativeWindow nativeWindow;
    nativeWindow.WindowId = glfwGetX11Window(window);
    nativeWindow.pDisplay = glfwGetX11Display();
#endif

    GfxInitResult result;
    EngineVkCreateInfo engineCI;
    SwapChainDesc scDesc;

    pFactoryVk->CreateDeviceAndContextsVk(engineCI, result.device.RawDblPtr(), result.context.RawDblPtr());
    NEX_EXP_RETURN_IF(!result.device || !result.context, 
        RuntimeError{"Unable to create graphics device and context!"});

    pFactoryVk->CreateSwapChainVk(result.device, result.context, scDesc, 
        nativeWindow, result.swapChain.RawDblPtr());
    NEX_EXP_RETURN_IF(!result.swapChain, RuntimeError{"Unable to create swapchain!"});

    return result;
}