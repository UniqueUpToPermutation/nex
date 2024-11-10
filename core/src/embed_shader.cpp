#include <nex/embed_shader.hpp>

#include <sstream>

using namespace nex;
using namespace nex::gfx;

// Defined in generated shader_rc.cpp
namespace nex {
	void MakeShaderMap(nex::file_map_t& map);
}

Expected<dg::IShader*> gfx::CompileEmbeddedShader(
    dg::IRenderDevice& device,
    const EmbeddedShaderParams& params,
    IVirtualFileSystem& fileLoader) {
    Error err;

    std::vector<dg::ShaderMacro> macros;
    macros.reserve(
        params.defines ? params.defines->size() : 0 + 
        params.overrides ? params.overrides->size() : 0);

    if (params.defines) {
        for (auto const& [key, value] : *params.defines) {
            macros.push_back(dg::ShaderMacro{
                key.c_str(),
                value.c_str()
            });
        }
    }
    if (params.overrides) {
        for (auto const& [key, value] : *params.overrides) {
            macros.push_back(dg::ShaderMacro{
                key.c_str(),
                value.c_str()
            });
        }
    }

    auto fileContents = NEX_EXP_UNWRAP(fileLoader.FindCStr(params.path), err);

    dg::ShaderCreateInfo info;
    info.Desc.Name = params.name.c_str();
    info.Desc.ShaderType = params.shaderType;
    info.Desc.UseCombinedTextureSamplers = true;
    info.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;
    info.Source = fileContents;
    if (!macros.empty()) {
        info.Macros = dg::ShaderMacroArray{macros.data(), static_cast<dg::Uint32>(macros.size())};
    }
    info.EntryPoint = params.entryPoint.c_str();
    
    dg::IShader* result = nullptr;
    device.CreateShader(info, &result);
    NEX_EXP_RETURN_IF(result == nullptr, ShaderCompilationError{.path = params.path.string()});

    return result;
}

EmbeddedFileLoader gfx::GetEmbeddedShaders() {
    return EmbeddedFileLoader(&MakeShaderMap);
}