#include <iostream>

#include <nex/core.hpp>
#include <nex/glfw_env.hpp>
#include <nex/gfx.hpp>
#include <nex/buffer.hpp>
#include <nex/embed_shader.hpp>
#include <nex/im3d.hpp>
#include <nex/camera.hpp>

using namespace nex;

Error Run() {
    Error err;

    GlfwEnvironmentParams envParams{
        .windowTitle = "Nex (Im3d Test)"
    };
    auto env = NEX_ERR_UNWRAP(GlfwEnvironment::Create(envParams), err);
    auto gfx = NEX_ERR_UNWRAP(gfx::GfxInit(env.GetWindow()), err);

    auto rtvFormat = gfx.swapChain->GetCurrentBackBufferRTV()->GetDesc().Format;
    auto psvFormat = gfx.swapChain->GetDepthBufferDSV()->GetDesc().Format;

    auto shaderSrc = gfx::GetEmbeddedShaders();
    auto globalsBuffer = NEX_ERR_UNWRAP(gfx::DynamicUniformBuffer<hlsl::SceneGlobals>::Create(*gfx.device), err);

    auto im3dShaders = NEX_ERR_UNWRAP(gfx::Im3dShaders::Create(*gfx.device, shaderSrc), err);
    auto im3dPipeline = NEX_ERR_UNWRAP(
        gfx::Im3dPipeline::Create(*gfx.device, globalsBuffer, rtvFormat, psvFormat, 1, im3dShaders, true), err);
    auto im3dModule = NEX_ERR_UNWRAP(gfx::Im3dModule::Create(*gfx.device), err);
    
    Camera camera;

    while (!env.ShouldClose()) {
        env.MessagePump();

        Im3d::NewFrame();

        Im3d::PushDrawState();
        Im3d::SetSize(10.0f);

        Im3d::BeginTriangles();
        Im3d::Vertex(-0.75f, -0.75f, 0.0f, Im3d::Color_Magenta);
        Im3d::Vertex(-0.5f, -0.25f, 0.0f, Im3d::Color_Yellow);
        Im3d::Vertex(-0.25f, -0.75f, 0.0f, Im3d::Color_Cyan);
        Im3d::End();

        Im3d::BeginLineLoop();
        Im3d::Vertex(0.75f, -0.75f, 0.0f, Im3d::Color_Red);
        Im3d::Vertex(0.5f, -0.25f, 0.0f, Im3d::Color_Green);
        Im3d::Vertex(0.25f, -0.75f, 0.0f, Im3d::Color_Blue);
        Im3d::End();

        Im3d::BeginPoints();
        Im3d::Vertex(-0.25f, 0.25f, 0.0f, Im3d::Color_Purple);
        Im3d::Vertex(0.0f, 0.75f, 0.0f, Im3d::Color_Brown);
        Im3d::Vertex(0.25f, 0.25f, 0.0f, Im3d::Color_Gold);
        Im3d::End();

        Im3d::PopDrawState();

        Im3d::EndFrame();
        
        auto* pRTV = gfx.swapChain->GetCurrentBackBufferRTV();
        auto* pDSV = gfx.swapChain->GetDepthBufferDSV();
        gfx.context->SetRenderTargets(1, &pRTV, pDSV, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        gfx.context->ClearRenderTarget(pRTV, ClearColor, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        gfx.context->ClearDepthStencil(pDSV, dg::CLEAR_DEPTH_FLAG, 1.f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        hlsl::SceneGlobals globals{
            .mCamera = camera.AsCameraAttribs(
                gfx.swapChain->GetDesc().Width, gfx.swapChain->GetDesc().Height)
        };
        globalsBuffer.Write(*gfx.context, globals);
        
        im3dModule.Draw(*gfx.context, im3dPipeline, Im3d::GetContext());

        gfx.swapChain->Present();
    }

    return err;
}

int main() {
    auto result = Run();
    return result.IsOk() ? 0 : -1;
}