#pragma once

#include <nex/texture.hpp>

#include <filesystem>

namespace nex {
    Expected<texture::Buffer> LoadPng(
        std::filesystem::path const& path, 
        texture::LoadParams const& params);
}