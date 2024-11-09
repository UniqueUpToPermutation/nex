#include <nex/im3d.hpp>
#include <nex/embed_shader.hpp>

using namespace nex;
using namespace nex::gfx;

Expected<Im3dShaders> Im3dShaders::Create(dg::IRenderDevice& device, IVirtualFileSystem& fileSystem) {
    Im3dShaders result;

    std::unordered_map<std::string, std::string> vsTrianglesConfig = {
        {"TRIANGLES", "1"},
        {"VERTEX_SHADER", "1"}
    };
    EmbeddedShaderParams vsTrianglesParams{
        .path = "im3d.hlsl",
        .shaderType = dg::SHADER_TYPE_VERTEX,
        .name = "Im3d Triangle VS",
        .overrides = &vsTrianglesConfig,
        .entryPoint = "main"
    };

    std::unordered_map<std::string, std::string> vsPointsConfig = {
        {"POINTS", "1"},
        {"VERTEX_SHADER", "1"}
    };
    EmbeddedShaderParams vsPointsParams{
        .path = "im3d.hlsl",
        .shaderType = dg::SHADER_TYPE_VERTEX,
        .name = "Im3d Points VS",
        .overrides = &vsPointsConfig,
        .entryPoint = "main"
    };

    std::unordered_map<std::string, std::string> vsLinesConfig = {
        {"LINES", "1"},
        {"VERTEX_SHADER", "1"}
    };
    EmbeddedShaderParams vsLinesParams{
        .path = "im3d.hlsl",
        .shaderType = dg::SHADER_TYPE_VERTEX,
        .name = "Im3d Lines VS",
        .overrides = &vsLinesConfig,
        .entryPoint = "main"
    };

    std::unordered_map<std::string, std::string> gsPointsConfig = {
        {"POINTS", "1"},
        {"GEOMETRY_SHADER", "1"}
    };
    EmbeddedShaderParams gsPointsParams{
        .path = "im3d.hlsl",
        .shaderType = dg::SHADER_TYPE_GEOMETRY,
        .name = "Im3d Points GS",
        .overrides = &gsPointsConfig,
        .entryPoint = "main_points"
    };

    std::unordered_map<std::string, std::string> gsLineConfig = {
        {"LINES", "1"},
        {"GEOMETRY_SHADER", "1"}
    };
    EmbeddedShaderParams gsLinesParams{
        .path = "im3d.hlsl",
        .shaderType = dg::SHADER_TYPE_GEOMETRY,
        .name = "Im3d Lines GS",
        .overrides = &gsLineConfig,
        .entryPoint = "main_lines"
    };

    std::unordered_map<std::string, std::string> psTriangleConfig = {
        {"TRIANGLES", "1"},
        {"PIXEL_SHADER", "1"}
    };
    EmbeddedShaderParams psTriangleParams{
        .path = "im3d.hlsl",
        .shaderType = dg::SHADER_TYPE_PIXEL,
        .name = "Im3d Triangles PS",
        .overrides = &psTriangleConfig,
        .entryPoint = "main_tris"
    };

    std::unordered_map<std::string, std::string> psLinesConfig = {
        {"LINES", "1"},
        {"PIXEL_SHADER", "1"}
    };
    EmbeddedShaderParams psLinesParams{
        .path = "im3d.hlsl",
        .shaderType = dg::SHADER_TYPE_PIXEL,
        .name = "Im3d Lines PS",
        .overrides = &psLinesConfig,
        .entryPoint = "main_lines"
    };

    std::unordered_map<std::string, std::string> psPointConfig = {
        {"POINTS", "1"},
        {"PIXEL_SHADER", "1"}
    };
    EmbeddedShaderParams psPointParams{
        .path = "im3d.hlsl",
        .shaderType = dg::SHADER_TYPE_PIXEL,
        .name = "Im3d Lines PS",
        .overrides = &psLinesConfig,
        .entryPoint = "main_points"
    };

    Error err;
    result.trianglesVS = NEX_EXP_UNWRAP(
        CompileEmbeddedShader(device, vsTrianglesParams, fileSystem), err);
    result.pointsVS = NEX_EXP_UNWRAP(
        CompileEmbeddedShader(device, vsPointsParams, fileSystem), err);
    result.linesVS = NEX_EXP_UNWRAP(
        CompileEmbeddedShader(device, vsLinesParams, fileSystem), err);
    result.pointsGS = NEX_EXP_UNWRAP(
        CompileEmbeddedShader(device, gsPointsParams, fileSystem), err);
    result.linesGS = NEX_EXP_UNWRAP(
        CompileEmbeddedShader(device, gsLinesParams, fileSystem), err);
    result.trianglesPS = NEX_EXP_UNWRAP(
        CompileEmbeddedShader(device, psTriangleParams, fileSystem), err);
    result.linesPS = NEX_EXP_UNWRAP(
        CompileEmbeddedShader(device, psLinesParams, fileSystem), err);
    result.pointsPS = NEX_EXP_UNWRAP(
        CompileEmbeddedShader(device, psPointParams, fileSystem), err);

    return result;
}

Expected<Im3dPipeline> Im3dPipeline::Create(
    dg::IRenderDevice& device,
    DynamicUniformBuffer<hlsl::SceneGlobals>& globals,
    dg::TEXTURE_FORMAT backbufferColorFormat,
    dg::TEXTURE_FORMAT backbufferDepthFormat,
    uint samples,
    Im3dShaders& shaders,
    bool bDepthEnable) {

    Im3dPipeline result;
    result.shaders = shaders;

    dg::GraphicsPipelineStateCreateInfo PSOCreateInfo;
    dg::PipelineStateDesc&              PSODesc          = PSOCreateInfo.PSODesc;
    dg::GraphicsPipelineDesc&           GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;

    PSODesc.Name         = "Im3d Triangle Pipeline";
    PSODesc.PipelineType = dg::PIPELINE_TYPE_GRAPHICS;

    GraphicsPipeline.NumRenderTargets             = 1;
    GraphicsPipeline.RTVFormats[0]                = backbufferColorFormat;
    GraphicsPipeline.PrimitiveTopology            = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    GraphicsPipeline.RasterizerDesc.CullMode      = dg::CULL_MODE_BACK;
    GraphicsPipeline.DepthStencilDesc.DepthEnable = bDepthEnable;
    GraphicsPipeline.DSVFormat 					  = backbufferDepthFormat;

    dg::RenderTargetBlendDesc blendState;
    blendState.BlendEnable = true;
    blendState.BlendOp = dg::BLEND_OPERATION_ADD;
    blendState.BlendOpAlpha = dg::BLEND_OPERATION_ADD;
    blendState.DestBlend = dg::BLEND_FACTOR_INV_SRC_ALPHA;
    blendState.SrcBlend = dg::BLEND_FACTOR_SRC_ALPHA;
    blendState.DestBlendAlpha = dg::BLEND_FACTOR_ONE;
    blendState.SrcBlendAlpha = dg::BLEND_FACTOR_ONE;

    GraphicsPipeline.BlendDesc.RenderTargets[0] = blendState;

    // Number of MSAA samples
    GraphicsPipeline.SmplDesc.Count = samples;

    size_t stride = sizeof(Im3d::VertexData);
    size_t position_offset = offsetof(Im3d::VertexData, m_positionSize);
    size_t color_offset = offsetof(Im3d::VertexData, m_color);

    std::vector<dg::LayoutElement> layoutElements = {
        // Position
        dg::LayoutElement(0, 0, 4, dg::VT_FLOAT32, false, 
            position_offset, stride, 
            dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX),
        // Color
        dg::LayoutElement(1, 0, 4, dg::VT_UINT8, true, 
            color_offset, stride, 
            dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX),
    };

    GraphicsPipeline.InputLayout.NumElements = layoutElements.size();
    GraphicsPipeline.InputLayout.LayoutElements = &layoutElements[0];

    PSOCreateInfo.pVS = shaders.trianglesVS.RawPtr();
    PSOCreateInfo.pGS = nullptr;
    PSOCreateInfo.pPS = shaders.trianglesPS.RawPtr();

    PSODesc.ResourceLayout.DefaultVariableType = dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    dg::IPipelineState* pipelineStateTris = nullptr;
    device.CreateGraphicsPipelineState(PSOCreateInfo, &pipelineStateTris);
    result.pipelineStateTriangles.Attach(pipelineStateTris);

    auto contextData = result.pipelineStateTriangles->GetStaticVariableByName(
        dg::SHADER_TYPE_VERTEX, "cbContextData");
    contextData->Set(globals.Get());

    // Line Pipeline
    GraphicsPipeline.PrimitiveTopology = dg::PRIMITIVE_TOPOLOGY_LINE_LIST;
    PSOCreateInfo.pVS = shaders.linesVS;
    PSOCreateInfo.pGS = shaders.linesGS;
    PSOCreateInfo.pPS = shaders.linesPS;
    PSODesc.Name = "Im3d Lines Pipeline";

    dg::IPipelineState* pipelineStateLines = nullptr;
    device.CreateGraphicsPipelineState(PSOCreateInfo, &pipelineStateLines);
    result.pipelineStateLines.Attach(pipelineStateLines);

    contextData = pipelineStateLines->GetStaticVariableByName(
        dg::SHADER_TYPE_VERTEX, "cbContextData");
    contextData->Set(globals.Get());

    contextData = pipelineStateLines->GetStaticVariableByName(
        dg::SHADER_TYPE_GEOMETRY, "cbContextData");
    contextData->Set(globals.Get());				

    // Point Pipeline
    GraphicsPipeline.PrimitiveTopology = dg::PRIMITIVE_TOPOLOGY_POINT_LIST;
    PSOCreateInfo.pVS = shaders.pointsVS;
    PSOCreateInfo.pGS = shaders.pointsGS;
    PSOCreateInfo.pPS = shaders.pointsPS;
    PSODesc.Name = "Im3d Points Pipeline";

    dg::IPipelineState* pipelineStateVertices = nullptr;
    device.CreateGraphicsPipelineState(PSOCreateInfo, &pipelineStateVertices);
    result.pipelineStateVertices.Attach(pipelineStateVertices);

    contextData = pipelineStateVertices->GetStaticVariableByName(
        dg::SHADER_TYPE_VERTEX, "cbContextData");
    contextData->Set(globals.Get());

    contextData = pipelineStateVertices->GetStaticVariableByName(
        dg::SHADER_TYPE_GEOMETRY, "cbContextData");
    contextData->Set(globals.Get());

    dg::IShaderResourceBinding* vertBinding = nullptr;
    pipelineStateVertices->CreateShaderResourceBinding(&vertBinding, true);
    result.vertexSRB.Attach(vertBinding);

    dg::IShaderResourceBinding* lineBinding = nullptr;
    pipelineStateLines->CreateShaderResourceBinding(&lineBinding, true);
    result.linesSRB.Attach(lineBinding);

    dg::IShaderResourceBinding* triangleBinding = nullptr;
    result.pipelineStateTriangles->CreateShaderResourceBinding(&triangleBinding, true);
    result.triangleSRB.Attach(triangleBinding);

    return result;
}

Expected<Im3dModule> Im3dModule::Create(dg::IRenderDevice& device, uint bufferSize) {
    dg::BufferDesc CBDesc;
    CBDesc.Name 			= "Im3d Geometry Buffer";
    CBDesc.Size 			= sizeof(Im3d::VertexData) * bufferSize;
    CBDesc.Usage 			= dg::USAGE_DYNAMIC;
    CBDesc.BindFlags 		= dg::BIND_VERTEX_BUFFER;
    CBDesc.CPUAccessFlags	= dg::CPU_ACCESS_WRITE;

    dg::IBuffer* geoBuf = nullptr;
    device.CreateBuffer(CBDesc, nullptr, &geoBuf);
    NEX_EXP_RETURN_IF(geoBuf == nullptr, RuntimeError{"Failed to create Im3d Buffer!"});

    Im3dModule module;
    module._geometryBuffer.Attach(geoBuf);
    return module;
}

uint Im3dModule::GetBufferCount() const {
    return _geometryBuffer->GetDesc().Size / sizeof(Im3d::VertexData);
}

void Im3dModule::Draw(dg::IDeviceContext& deviceContext,
    Im3dPipeline& pipeline,
    Im3d::Context& im3dContext) {

    uint bufferCount = GetBufferCount();
    uint drawListCount = im3dContext.getDrawListCount();

    dg::RefCntAutoPtr<dg::IPipelineState> currentPipelineState;

    dg::Uint64 offsets[] = {0};
    dg::IBuffer* vBuffers[] = { _geometryBuffer.RawPtr()};

    deviceContext.SetVertexBuffers(0, 1, 
        vBuffers, offsets, 
        dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
        dg::SET_VERTEX_BUFFERS_FLAG_RESET);

    for (uint i = 0; i < drawListCount; ++i) {
        auto drawList = &im3dContext.getDrawLists()[i];

        switch (drawList->m_primType) {
            case Im3d::DrawPrimitiveType::DrawPrimitive_Triangles:
                if (currentPipelineState != pipeline.pipelineStateTriangles) {
                    currentPipelineState = pipeline.pipelineStateTriangles;
                    deviceContext.SetPipelineState(currentPipelineState);
                    deviceContext.CommitShaderResources(pipeline.triangleSRB.RawPtr(), 
                        dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                }
                break;
            case Im3d::DrawPrimitive_Lines:
                if (currentPipelineState != pipeline.pipelineStateLines) {
                    currentPipelineState = pipeline.pipelineStateLines;
                    deviceContext.SetPipelineState(currentPipelineState);
                    deviceContext.CommitShaderResources(pipeline.linesSRB.RawPtr(), 
                        dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                }
                break;
            case Im3d::DrawPrimitive_Points:
                if (currentPipelineState != pipeline.pipelineStateVertices) {
                    currentPipelineState = pipeline.pipelineStateVertices;
                    deviceContext.SetPipelineState(currentPipelineState);
                    deviceContext.CommitShaderResources(pipeline.vertexSRB.RawPtr(), 
                        dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                }
                break;
            default:
                break;
        }

        uint currentIndx = 0;
        while (currentIndx < drawList->m_vertexCount) {
            uint vertsToRender = std::min(bufferCount, drawList->m_vertexCount - currentIndx);

            {
                dg::MapHelper<Im3d::VertexData> vertexMap(&deviceContext, _geometryBuffer, 
                    dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
                std::memcpy(vertexMap, &drawList->m_vertexData[currentIndx],
                    sizeof(Im3d::VertexData) * vertsToRender);
            }

            dg::DrawAttribs drawAttribs;
            drawAttribs.NumVertices = vertsToRender;
            drawAttribs.Flags = dg::DRAW_FLAG_VERIFY_ALL;

            deviceContext.Draw(drawAttribs);

            currentIndx += vertsToRender;
        }
    }
}