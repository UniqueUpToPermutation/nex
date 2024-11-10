#pragma once

#include <nex/vertex_format.hpp>

#include <glm/glm.hpp>

#include <filesystem>
#include <span>

namespace nex {
    glm::uvec1 ColorToBytes(glm::vec1 x);
    glm::uvec2 ColorToBytes(glm::vec2 x);
    glm::uvec3 ColorToBytes(glm::vec3 x);
    glm::uvec4 ColorToBytes(glm::vec4 x);

    template <typename T>
    auto SpanOf(T& obj) {
        return std::span{&obj[0], static_cast<size_t>(obj.length())};
    }

    namespace texture {
        enum class Dimension {
            Buffer,
            Texture1D,
            Texture1DArray,
            Texture2D,
            Texture2DArray,
            Texture3D,
            TextureCube,
            TextureCubeArray
        };

        inline bool IsArray(Dimension dim) {
            if (dim == Dimension::Texture1D ||
                dim == Dimension::Texture2D ||
                dim == Dimension::Texture3D) {
                return false;
            } else {
                return true;
            }
        }

        struct Format {
            uint32_t channels;
            ValueType valueType;
            bool isNormalized;
            bool isLinear;

            static Format UNKNOWN();
            static Format RGBA32_FLOAT();
            static Format RGBA32_UINT();
            static Format RGBA32_SINT();
            static Format RGB32_FLOAT();
            static Format RGB32_UINT();
            static Format RGB32_SINT();
            static Format R32_FLOAT();
            static Format R32_SINT();
            static Format R32_UINT();
            static Format RG32_FLOAT();
            static Format RG32_UINT();
            static Format RG32_SINT();
            static Format RGBA8_UINT();
            static Format RGBA8_UNORM();
            static Format RGBA8_SINT();
            static Format RGBA8_SNORM();
            static Format SRGBA8_UNORM();
            static Format R8_SINT();
            static Format R8_UINT();
            static Format R8_SNORM();
            static Format R8_UNORM();
            static Format RG8_SINT();
            static Format RG8_UINT();
            static Format RG8_SNORM();
            static Format RG8_UNORM();
            static Format RGB8_SINT();
            static Format RGB8_UINT();
            static Format RGB8_SNORM();
            static Format RGB8_UNORM();
            static Format R16_SINT();
            static Format R16_UINT();
            static Format R16_SNORM();
            static Format R16_UNORM();
            static Format RG16_SINT();
            static Format RG16_UINT();
            static Format RG16_SNORM();
            static Format RG16_UNORM();
            static Format RGBA16_SINT();
            static Format RGBA16_UINT();
            static Format RGBA16_SNORM();
            static Format RGBA16_UNORM();

            inline uint32_t GetPixelByteSize() const {
                return GetSize(valueType) * channels;
            }
        };

        struct LoadParams {
            bool isSRGB = false;
            bool generateMips = true;
        };

        uint32_t MipCount(
            const uint32_t width, 
            const uint32_t height);

        uint32_t MipCount(
            const uint32_t width, 
            const uint32_t height, 
            const uint32_t depth);

        struct SubResDataDesc {
            uint64_t depthStride;
            uint64_t srcOffset;
            uint64_t stride;
            uint64_t length;
            uint32_t mip;
            uint32_t slice;
        };

        struct Desc {
            Dimension type;
            uint32_t width = 1;
            uint32_t height = 1;
            uint32_t arraySizeOrDepth = 1;
            Format format;
            uint32_t mipLevels = 1;
            uint32_t sampleCount = 1;

            inline uint32_t GetPixelByteSize() const {
                return format.GetPixelByteSize();
            }

            uint32_t GetByteSize() const;
            std::vector<SubResDataDesc> GetSubresourceDescs() const;

            uint32_t GetMipCount() const {
                if (mipLevels == 0) {
                    return MipCount(width, height, arraySizeOrDepth);
                } else 
                    return mipLevels;
            }

            uint32_t GetDepth() const {
                if (IsArray(type)) {
                    return 1u;
                } else {
                    return arraySizeOrDepth;
                }
            }

            uint32_t GetArraySize() const {
                if (IsArray(type)) {
                    return arraySizeOrDepth;
                } else {
                    return 1u;
                }
            }
        };

        struct Buffer {
            Desc desc;
            std::vector<uint8_t> data;

            void GenerateMips();
            static Buffer Alloc(const Desc& desc);

            inline Desc GetDesc() const {
                return desc;
            }
        };

        namespace prefabs {
            Buffer SolidColor(
                uint32_t width,
                uint32_t height,
                glm::vec4 color);
            Buffer CheckerBoard(
                uint32_t width,
                uint32_t height,
                uint32_t widthSubdivisions,
                uint32_t heightSubdivisions,
                glm::vec4 color1 = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
                glm::vec4 color2 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
            );
        };

        Buffer LoadFromBytes_RGBA8_UNORM(
            const LoadParams& params, 
            const std::vector<uint8_t>& image,
            uint32_t width, uint32_t height);
    }

    using TextureBuffer = texture::Buffer;
    using TextureDesc = texture::Desc;
    using TextureLoadParams = texture::LoadParams;
    using TextureFormat = texture::Format;
}