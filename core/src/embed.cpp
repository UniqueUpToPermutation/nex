#include <nex/embed.hpp>

using namespace nex;

Expected<std::string_view> nex::IVirtualFileSystem::Find(const std::filesystem::path& source) const {
    auto result = FindCStr(source);
    NEX_EXP_RETURN(result.error());
    return std::string_view{*result};
}

Expected<const char*> nex::EmbeddedFileLoader::FindCStr(const std::filesystem::path& source) const {
    auto searchSource = std::filesystem::relative(source, "");
    
    // Search internal shaders first
    auto it = _internalShaders.find(searchSource);
    NEX_EXP_RETURN_IF(it == _internalShaders.end(), InvalidPathError{source});
    return it->second;
}

void nex::EmbeddedFileLoader::Add(const embedded_file_loader_t& factory) {
    factory(_internalShaders);
}

nex::EmbeddedFileLoader::EmbeddedFileLoader(const embedded_file_loader_t& factory) {
    Add(factory);
}
