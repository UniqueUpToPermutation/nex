#include <iostream>

#include <nex/core.hpp>
#include <nex/glfw_env.hpp>
#include <nex/gfx.hpp>

using namespace nex;

static const char* VSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

void main(in  uint    VertId : SV_VertexID,
          out PSInput PSIn) 
{
    float4 Pos[3];
    Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
    Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
    Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

    float3 Col[3];
    Col[0] = float3(1.0, 0.0, 0.0); // red
    Col[1] = float3(0.0, 1.0, 0.0); // green
    Col[2] = float3(0.0, 0.0, 1.0); // blue

    PSIn.Pos   = Pos[VertId];
    PSIn.Color = Col[VertId];
}
)";

static const char* PSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

struct PSOutput
{ 
    float4 Color : SV_TARGET; 
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn.Color.rgb, 1.0);
}
)";


Error Run() {
    Error err;

    auto env = NEX_ERR_UNWRAP(GlfwEnvironment::Create(), err);
    auto gfx = NEX_ERR_UNWRAP(gfx::GfxInit(env.GetWindow()), err);

    dg::GraphicsPipelineStateCreateInfo PSOCreateInfo;

    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";

    PSOCreateInfo.PSODesc.PipelineType = dg::PIPELINE_TYPE_GRAPHICS;
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = gfx.swapChain->GetDesc().ColorBufferFormat;
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = gfx.swapChain->GetDesc().DepthBufferFormat;
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = dg::CULL_MODE_NONE;
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = false;

    dg::ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.Desc.UseCombinedTextureSamplers = true;

    dg::RefCntAutoPtr<dg::IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Triangle vertex shader";
        ShaderCI.Source          = VSSource;
        gfx.device->CreateShader(ShaderCI, &pVS);
    }
    NEX_ERR_RETURN_IF(pVS == nullptr, RuntimeError{"Failed to create vertex shader!"});

    dg::RefCntAutoPtr<dg::IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Triangle pixel shader";
        ShaderCI.Source          = PSSource;
        gfx.device->CreateShader(ShaderCI, &pPS);
    }
    NEX_ERR_RETURN_IF(pPS == nullptr, RuntimeError{"Failed to create fragment shader!"});

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    
    dg::RefCntAutoPtr<dg::IPipelineState> pPSO;
    gfx.device->CreateGraphicsPipelineState(PSOCreateInfo, &pPSO);
    NEX_ERR_RETURN_IF(pPSO == nullptr, RuntimeError{"Failed to create graphics pipeline!"});

    while (!env.ShouldClose()) {
        env.MessagePump();

        auto* pRTV = gfx.swapChain->GetCurrentBackBufferRTV();
        auto* pDSV = gfx.swapChain->GetDepthBufferDSV();
        gfx.context->SetRenderTargets(1, &pRTV, pDSV, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        gfx.context->ClearRenderTarget(pRTV, ClearColor, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        gfx.context->ClearDepthStencil(pDSV, dg::CLEAR_DEPTH_FLAG, 1.f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        gfx.context->SetPipelineState(pPSO);
        dg::DrawAttribs drawAttrs;
        drawAttrs.NumVertices = 3; 
        gfx.context->Draw(drawAttrs);

        gfx.swapChain->Present();
    }

    return err;
}

int main() {
    auto result = Run();
    return result.IsOk() ? 0 : -1;
}