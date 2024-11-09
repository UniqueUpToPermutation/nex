#pragma once

#include <DeviceContext.h>
#include <SwapChain.h>
#include <EngineFactory.h>
#include <RenderDevice.h>
#include <RefCntAutoPtr.hpp>

namespace dg = Diligent;

#include <nex/glfw_env.hpp>

namespace nex::gfx {

    struct GfxInitResult {
        dg::RefCntAutoPtr<dg::IRenderDevice> device;
        dg::RefCntAutoPtr<dg::IDeviceContext> context;
        dg::RefCntAutoPtr<dg::ISwapChain> swapChain;
    };

    Expected<GfxInitResult> GfxInit(GLFWwindow* window);
}