#include <nex/embed_shader.hpp>

using namespace nex;

#include <sstream>

Expected<dg::IShader*> CompileEmbeddedShader(
    dg::IRenderDevice& device,
    const EmbeddedShaderParams& params,
    IVirtualFileSystem& fileLoader) {

    std::stringstream ss;

    if (params.defines) {
        for (auto const& [key, value] : *params.defines) {
            ss << "#define " << key << " " << value << "\n";
        }
    }
    if (params.overrides) {
        for (auto const& [key, value] : *params.overrides) {
            ss << "#define " << key << " " << value << "\n";
        }
    }

    auto fileContents = fileLoader.Find(params.path);
    NEX_EXP_RETURN(fileContents);

    dg::ShaderCreateInfo info;
    info.Desc.ShaderType = params.shaderType;
    info.Desc.UseCombinedTextureSamplers = true;
    info.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;

    dg::IShader* result = nullptr;
    device.CreateShader(info, &result);
    NEX_EXP_RETURN_IF(result == nullptr, ShaderCompilationError{.path = params.path.string()});

    return result;
}