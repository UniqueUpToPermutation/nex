#include <iostream>

#include <nex/core.hpp>
#include <nex/glfw_env.hpp>
#include <nex/gfx.hpp>
#include <nex/imgui.hpp>
#include <nex/imgui_glfw.hpp>

#include <imgui.h>

using namespace nex;

Error Run() {
    Error err;

    auto env = NEX_ERR_UNWRAP(GlfwEnvironment::Create(), err);
    auto gfx = NEX_ERR_UNWRAP(gfx::GfxInit(env.GetWindow()), err);

    dg::ImGuiDiligentCreateInfo renderCI;
    renderCI.BackBufferFmt = gfx.swapChain->GetDesc().ColorBufferFormat;
    renderCI.DepthBufferFmt = gfx.swapChain->GetDesc().DepthBufferFormat;
    renderCI.pDevice = gfx.device;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.FontGlobalScale = 2.0f;

    ImGui_ImplGlfw_InitForOther(env.GetWindow(), true);

    dg::ImGuiDiligentRenderer renderer(std::cref(renderCI));
    renderer.CreateDeviceObjects();
    renderer.CreateFontsTexture();

    bool isDemoWndOpen = true;

    while (!env.ShouldClose()) {
        env.MessagePump();

        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ImGui::ShowDemoWindow(&isDemoWndOpen);
        ImGui::EndFrame();

        ImGui::Render();

        auto* pRTV = gfx.swapChain->GetCurrentBackBufferRTV();
        auto* pDSV = gfx.swapChain->GetDepthBufferDSV();
        gfx.context->SetRenderTargets(1, &pRTV, pDSV, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        gfx.context->ClearRenderTarget(pRTV, ClearColor, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        gfx.context->ClearDepthStencil(pDSV, dg::CLEAR_DEPTH_FLAG, 1.f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        renderer.NewFrame(
            gfx.swapChain->GetDesc().Width, 
            gfx.swapChain->GetDesc().Height, 
            gfx.swapChain->GetDesc().PreTransform);
        renderer.RenderDrawData(gfx.context, ImGui::GetDrawData());
        renderer.EndFrame();

        gfx.swapChain->Present();
    }

    ImGui_ImplGlfw_Shutdown();

    return err;
}

int main() {
    auto result = Run();
    return result.IsOk() ? 0 : -1;
}