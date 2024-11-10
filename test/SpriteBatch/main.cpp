#include <iostream>

#include <nex/core.hpp>
#include <nex/glfw_env.hpp>
#include <nex/gfx.hpp>
#include <nex/sprite.hpp>
#include <nex/embed_shader.hpp>
#include <nex/camera.hpp>

#include <imgui.h>

using namespace nex;

Error Run() {
    Error err;

    auto env = NEX_ERR_UNWRAP(GlfwEnvironment::Create(), err);
    auto gfx = NEX_ERR_UNWRAP(gfx::GfxInit(env.GetWindow()), err);

    auto shaderSrc = gfx::GetEmbeddedShaders();
    auto shaders = NEX_ERR_UNWRAP(gfx::SpriteShaders::Create(*gfx.device, shaderSrc), err);

    auto rtvFormat = gfx.swapChain->GetCurrentBackBufferRTV()->GetDesc().Format;
    auto psvFormat = gfx.swapChain->GetDepthBufferDSV()->GetDesc().Format;

    auto globalsCi = gfx::DynamicBufferCI::UniformBuffer();
    auto globalsBuffer = gfx::DynamicBuffer<hlsl::SceneGlobals>::Create(*gfx.device, globalsCi);

    auto camera = Camera();

    gfx::SpriteBatchPipelineCI ci{
        .shaders = shaders,
        .globals = globalsBuffer,
        .backbufferFormat = rtvFormat,
        .depthbufferFormat = psvFormat,
    };
    auto spriteReqs = NEX_ERR_UNWRAP(gfx::SpriteBatchRequirements::Create(*gfx.device, ci), err);

    auto spriteBatch = gfx::SpriteBatch::Create(*gfx.device);

    while (!env.ShouldClose()) {
        env.MessagePump();

        globalsBuffer.Write(*gfx.context, hlsl::SceneGlobals{
            .mCamera = camera.AsCameraAttribs(
                gfx.swapChain->GetDesc().Width, gfx.swapChain->GetDesc().Height)
        });

        auto* pRTV = gfx.swapChain->GetCurrentBackBufferRTV();
        auto* pDSV = gfx.swapChain->GetDepthBufferDSV();
        gfx.context->SetRenderTargets(1, &pRTV, pDSV, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        gfx.context->ClearRenderTarget(pRTV, ClearColor, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        gfx.context->ClearDepthStencil(pDSV, dg::CLEAR_DEPTH_FLAG, 1.f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        spriteBatch.Begin(*gfx.context, spriteReqs);
        spriteBatch.End();

        gfx.swapChain->Present();
    }

    return err;
}

int main() {
    auto result = Run();
    return result.IsOk() ? 0 : -1;
}