#include <nex/loaders/png.hpp>
#include <nex/loaders/lodepng.hpp>

using namespace nex;

Expected<texture::Buffer> nex::LoadPng(
    std::filesystem::path const& path,
    texture::LoadParams const& params) {
    texture::Buffer data;
    std::vector<uint8_t> image;
    uint32_t width, height;
    uint32_t error = lodepng::decode(image, width, height, path.string());

    //if there's an error, display it
    if (error) {
        NEX_EXP_RETURN_IF(!std::filesystem::exists(path), InvalidPathError{path});
        return MakeUnexpected(NEX_ERR_MAKE(RuntimeError{lodepng_error_text(error)}));
    }

    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
    //State state contains extra information about the PNG such as text chunks, ...
    return texture::LoadFromBytes_RGBA8_UNORM(params, image, width, height);
}