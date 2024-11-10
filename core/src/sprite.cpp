#include <nex/sprite.hpp>
#include <nex/embed_shader.hpp>

using namespace nex;
using namespace nex::gfx;

using namespace dg;

Expected<SpriteShaders> SpriteShaders::Create(
    dg::IRenderDevice& device, 
    IVirtualFileSystem& system) {
    std::unordered_map<std::string, std::string> vsConfig = {
        {"VERTEX_SHADER", "1"}
    };
    EmbeddedShaderParams vsParams{
        .path = "sprite.hlsl",
        .shaderType = dg::SHADER_TYPE_VERTEX,
        .name = "Sprite VS",
        .overrides = &vsConfig,
        .entryPoint = "main"
    };

    std::unordered_map<std::string, std::string> gsConfig = {
        {"GEOMETRY_SHADER", "1"}
    };
    EmbeddedShaderParams gsParams{
        .path = "sprite.hlsl",
        .shaderType = dg::SHADER_TYPE_GEOMETRY,
        .name = "Sprite GS",
        .overrides = &gsConfig,
        .entryPoint = "main"
    };

    std::unordered_map<std::string, std::string> psConfig = {
        {"PIXEL_SHADER", "1"}
    };
    EmbeddedShaderParams psParams{
        .path = "sprite.hlsl",
        .shaderType = dg::SHADER_TYPE_PIXEL,
        .name = "Sprite PS",
        .overrides = &psConfig,
        .entryPoint = "main"
    };

    Error err;
    SpriteShaders shaders;
    shaders.vs = NEX_EXP_UNWRAP(CompileEmbeddedShader(device, vsParams, system), err);
    shaders.gs = NEX_EXP_UNWRAP(CompileEmbeddedShader(device, gsParams, system), err);
    shaders.ps = NEX_EXP_UNWRAP(CompileEmbeddedShader(device, psParams, system), err);
    return shaders;
}

Expected<dg::RefCntAutoPtr<dg::IPipelineState>> gfx::CreateSpriteBatchPipeline(
    dg::IRenderDevice& device,
    SpriteBatchPipelineCI const& ci) {
    dg::SamplerDesc SamDesc
    {
        ci.filterType, ci.filterType, ci.filterType, 
        dg::TEXTURE_ADDRESS_CLAMP, dg::TEXTURE_ADDRESS_CLAMP, dg::TEXTURE_ADDRESS_CLAMP
    };

    dg::GraphicsPipelineStateCreateInfo PSOCreateInfo;
    dg::PipelineStateDesc&              PSODesc          = PSOCreateInfo.PSODesc;
    dg::GraphicsPipelineDesc&           GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;

    PSODesc.Name         = "Sprite Batch Pipeline";
    PSODesc.PipelineType = dg::PIPELINE_TYPE_GRAPHICS;

    GraphicsPipeline.NumRenderTargets             = 1;
    GraphicsPipeline.RTVFormats[0]                = ci.backbufferFormat;
    GraphicsPipeline.PrimitiveTopology            = dg::PRIMITIVE_TOPOLOGY_POINT_LIST;
    GraphicsPipeline.RasterizerDesc.CullMode      = dg::CULL_MODE_BACK;
    GraphicsPipeline.DepthStencilDesc.DepthEnable = false;
    GraphicsPipeline.DSVFormat 					  = ci.depthbufferFormat;

    dg::RenderTargetBlendDesc blendState;
    blendState.BlendEnable =    true;
    blendState.BlendOp =        dg::BLEND_OPERATION_ADD;
    blendState.BlendOpAlpha =   dg::BLEND_OPERATION_ADD;
    blendState.DestBlend =      dg::BLEND_FACTOR_INV_SRC_ALPHA;
    blendState.SrcBlend =       dg::BLEND_FACTOR_SRC_ALPHA;
    blendState.DestBlendAlpha = dg::BLEND_FACTOR_ONE;
    blendState.SrcBlendAlpha =  dg::BLEND_FACTOR_ONE;

    GraphicsPipeline.BlendDesc.RenderTargets[0] = blendState;

    GraphicsPipeline.SmplDesc.Count = ci.samples;

    uint stride = sizeof(hlsl::SpriteBatchVSInput);

    std::vector<dg::LayoutElement> layoutElements = {
        dg::LayoutElement(0, 0, 4, dg::VT_FLOAT32, false, 
            dg::LAYOUT_ELEMENT_AUTO_OFFSET, stride, dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX),
        dg::LayoutElement(1, 0, 4, dg::VT_FLOAT32, false, 
            dg::LAYOUT_ELEMENT_AUTO_OFFSET, stride, dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX),
        dg::LayoutElement(2, 0, 2, dg::VT_FLOAT32, false, 
            dg::LAYOUT_ELEMENT_AUTO_OFFSET, stride, dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX),
        dg::LayoutElement(3, 0, 2, dg::VT_FLOAT32, false, 
            dg::LAYOUT_ELEMENT_AUTO_OFFSET, stride, dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX),
        dg::LayoutElement(4, 0, 2, dg::VT_FLOAT32, false, 
            dg::LAYOUT_ELEMENT_AUTO_OFFSET, stride, dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX),
        dg::LayoutElement(5, 0, 2, dg::VT_FLOAT32, false, 
            dg::LAYOUT_ELEMENT_AUTO_OFFSET, stride, dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX),
    };

    GraphicsPipeline.InputLayout.NumElements = layoutElements.size();
    GraphicsPipeline.InputLayout.LayoutElements = &layoutElements[0];

    PSOCreateInfo.pVS = ci.shaders.vs;
    PSOCreateInfo.pGS = ci.shaders.gs;
    PSOCreateInfo.pPS = ci.shaders.ps;

    PSODesc.ResourceLayout.DefaultVariableType = dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    dg::ShaderResourceVariableDesc Vars[] = 
    {
        {dg::SHADER_TYPE_PIXEL, "mTexture", dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };
    PSODesc.ResourceLayout.NumVariables = _countof(Vars);
    PSODesc.ResourceLayout.Variables    = Vars;

    dg::ImmutableSamplerDesc ImtblSamplers[] =
    {
        {dg::SHADER_TYPE_PIXEL, "mTexture_sampler", SamDesc}
    };
    PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);
    PSODesc.ResourceLayout.ImmutableSamplers    = ImtblSamplers;

    dg::RefCntAutoPtr<dg::IPipelineState> pso;
    device.CreateGraphicsPipelineState(PSOCreateInfo, &pso);
    NEX_EXP_RETURN_IF(pso == nullptr, RuntimeError{"Failed to create pipeline state!"});

    auto globalVar = pso->GetStaticVariableByName(dg::SHADER_TYPE_VERTEX, "cbContextData");
    NEX_EXP_RETURN_IF(globalVar == nullptr, RuntimeError{"cbContextData not defined in shader!"});

    globalVar->Set(ci.globals.Get());
    return pso;
}

SpriteBatchBindings gfx::CreateSpriteBatchBindings(dg::IPipelineState& pso) {
    dg::RefCntAutoPtr<dg::IShaderResourceBinding> srb;
    pso.CreateShaderResourceBinding(&srb, true);

    auto textureVariable = srb->GetVariableByName(dg::SHADER_TYPE_PIXEL, "mTexture");
    return SpriteBatchBindings{
        .textureVariable = textureVariable,
        .shaderBinding = srb,
    };
}

Expected<SpriteBatchRequirements> SpriteBatchRequirements::Create(
    dg::IRenderDevice& device, SpriteBatchPipelineCI const& ci) {
    Error err;
    auto pso = NEX_EXP_UNWRAP(CreateSpriteBatchPipeline(device, ci), err);
    return SpriteBatchRequirements{
        .pso = pso,
        .bindings = CreateSpriteBatchBindings(*pso)
    };
}

SpriteBatch SpriteBatch::Create(dg::IRenderDevice& device, SpriteBatchCI const& ci) {
    return SpriteBatch(
        DynamicBuffer<hlsl::SpriteBatchVSInput>::Create(
            device, DynamicBufferCI::VertexBuffer(ci.batchSize))
    );
}

void SpriteBatch::Begin(
    dg::IDeviceContext& context, 
    dg::IPipelineState& pso,
    dg::IShaderResourceBinding& srb,
    dg::IShaderResourceVariable& textureVariable) {
    context.SetPipelineState(&pso);

    dg::IBuffer* buffers[] = { buffer.Get() };
    dg::Uint64 offsets[] = { 0 };
    
    context.SetVertexBuffers(0, 1, buffers, offsets, 
        dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, 
        dg::SET_VERTEX_BUFFERS_FLAG_RESET);

    map = buffer.Map(context);

    groups.clear();
    currentContext = &context;

    this->pso = &pso;
    this->srb = &srb;
    this->textureVariable = &textureVariable;
}

void SpriteBatch::Begin(
    dg::IDeviceContext& context,
    SpriteBatchRequirements& requirements) {
    Begin(context,
        *requirements.pso,
        *requirements.bindings.shaderBinding,
        *requirements.bindings.textureVariable);
}

void SpriteBatch::Flush(bool shouldRemap) {
    if (!groups.empty()) {
        map = {};

        for (auto const& group : groups) {
            textureVariable->Set(group.texture.GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE));
            currentContext->CommitShaderResources(srb, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            dg::DrawAttribs attribs;
            attribs.Flags = dg::DRAW_FLAG_VERIFY_ALL;
            attribs.StartVertexLocation = group.start;
            attribs.NumVertices = group.size;

            currentContext->Draw(attribs);
        }

        if (shouldRemap) {
            map = buffer.Map(*currentContext);
        }
    }
}

void SpriteBatch::End() {
    Flush(false);
}

uint SpriteBatch::GetWriteIndex() const {
    if (groups.empty()) {
        return 0;
    } else {
        auto const& back = groups.back();
        return back.start + back.size;
    }
}

void SpriteBatch::Draw(
    dg::ITexture& texture,
    std::span<hlsl::SpriteBatchVSInput const> arr) {

    AssureGroup(texture);
    uint indexInArr = 0;

    // Repeatedly write into the buffer that we allocated and flush as needed.
    while (true) {
        uint remainingInArr = arr.size() - indexInArr;
        uint writeIndex = GetWriteIndex();

        if (remainingInArr == 0) {
            break;
        }

        uint numberToWrite = std::min<uint>(remainingInArr, buffer.Count() - writeIndex);
        std::memcpy(&map[writeIndex], &arr[indexInArr], sizeof(hlsl::SpriteBatchVSInput) * numberToWrite);

        indexInArr += numberToWrite;
        groups.back().size += numberToWrite;

        if (GetWriteIndex() == buffer.Count()) {
            Flush(true);
            AssureGroup(texture);
        }
    }
}

void SpriteBatch::AssureGroup(dg::ITexture& texture) {
    if (groups.empty() || &groups.back().texture != &texture) {
        groups.push_back(Group{
            .texture = texture,
            .start = GetWriteIndex(),
            .size = 0
        });
    }
}

void SpriteBatch::Draw(
    dg::ITexture& texture, 
    const glm::vec2& pos, 
    const glm::vec2& size, 
    const SpriteRect& rect, 
	const glm::vec2& origin, 
    const float rotation, 
	const glm::vec4& color) {

    AssureGroup(texture);

    auto writeIndex = GetWriteIndex();

    hlsl::SpriteBatchVSInput* instance = &map[writeIndex];
    auto& desc = texture.GetDesc();
    glm::vec2 dim2d(desc.Width, desc.Height);
    
    glm::vec2 uvtop_unscaled(rect.pos.x - 0.5f, rect.pos.y - 0.5f);
    auto uvbottom_unscaled = uvtop_unscaled + rect.size;

    instance->mPos.x = pos.x;
    instance->mPos.y = pos.y;
    instance->mPos.z = 0.0f;
    instance->mPos.w = rotation;

    instance->mColor = color;
    instance->mOrigin = origin;
    instance->mSize = size;
    instance->mUVTop = uvtop_unscaled / dim2d;
    instance->mUVBottom = uvbottom_unscaled / dim2d;

    writeIndex += 1;
    groups.back().size += 1;

    if (writeIndex == buffer.Count()) {
        Flush(true);
    }
}