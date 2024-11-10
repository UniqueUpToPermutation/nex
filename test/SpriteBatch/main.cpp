#include <iostream>

#include <nex/core.hpp>
#include <nex/glfw_env.hpp>
#include <nex/gfx.hpp>
#include <nex/sprite.hpp>
#include <nex/embed_shader.hpp>
#include <nex/camera.hpp>
#include <nex/gfx_texture.hpp>

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

    auto textureBuffer = texture::prefabs::CheckerBoard(128, 128, 8, 8);
    auto gpuTexture = gfx::UploadToGpu(*gfx.device, *gfx.context, textureBuffer);
    textureBuffer = {};

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

        auto backbufferWidth = static_cast<float>(gfx.swapChain->GetDesc().Width);
        auto backbufferHeight = static_cast<float>(gfx.swapChain->GetDesc().Height);

        auto camera = Camera{
            .projection = OrthoProjection{
                .width = backbufferWidth,
                .height = backbufferHeight
            },
        };
        globalsBuffer.Write(*gfx.context, hlsl::SceneGlobals{
            .mCamera = camera.AsCameraAttribs(backbufferWidth, backbufferHeight)
        });

        auto* pRTV = gfx.swapChain->GetCurrentBackBufferRTV();
        auto* pDSV = gfx.swapChain->GetDepthBufferDSV();
        gfx.context->SetRenderTargets(1, &pRTV, pDSV, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        gfx.context->ClearRenderTarget(pRTV, ClearColor, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        gfx.context->ClearDepthStencil(pDSV, dg::CLEAR_DEPTH_FLAG, 1.f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        spriteBatch.Begin(*gfx.context, spriteReqs);
        spriteBatch.Draw(*gpuTexture, glm::vec2(0.0, 0.0), glm::vec2(64.0, 64.0), 1.0);
        spriteBatch.Draw(*gpuTexture, glm::vec2(128.0, 128.0), glm::vec2(64.0, 64.0), 0.5);
        spriteBatch.Draw(*gpuTexture, glm::vec2(256.0, 256.0), glm::vec2(64.0, 64.0), 2.0);
        spriteBatch.End();

        gfx.swapChain->Present();
    }

    return err;
}

int main() {
    auto result = Run();
    return result.IsOk() ? 0 : -1;
}