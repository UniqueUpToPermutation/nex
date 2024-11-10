#include <nex/texture.hpp>
#include <nex/mip_generator.hpp>

#include <filesystem>

#include <cmath>
#include <cstring>
#include <span>

using namespace nex;
using namespace nex::texture;

glm::uvec1 nex::ColorToBytes(glm::vec1 x) {
    return glm::clamp(x, glm::vec1(0.0f), glm::vec1(1.0f)) * 255.0f;
}
glm::uvec2 nex::ColorToBytes(glm::vec2 x) {
    return glm::clamp(x, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)) * 255.0f;
}
glm::uvec3 nex::ColorToBytes(glm::vec3 x) {
    return glm::clamp(x, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 255.0f;
}
glm::uvec4 nex::ColorToBytes(glm::vec4 x) {
    return glm::clamp(x, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) * 255.0f;
}

Format Format::UNKNOWN() {
    return Format{
        0, ValueType::UNDEFINED, false, false
    };
}

Format Format::RGBA32_FLOAT() {
    return Format{
        4, ValueType::FLOAT32, false, true
    };
}
Format Format::RGBA32_UINT() {
    return Format{
        4, ValueType::UINT32, false, true
    };
}
Format Format::RGBA32_SINT() {
    return Format{
        4, ValueType::INT32, false, true
    };
}
Format Format::RGB32_FLOAT() {
    return Format{
        3, ValueType::FLOAT32, false, true
    };
}
Format Format::RGB32_UINT() {
    return Format{
        3, ValueType::UINT32, false, true
    };
}
Format Format::RGB32_SINT() {
    return Format{
        3, ValueType::INT32, false, true
    };
}
Format Format::R32_FLOAT() {
    return Format{
        1, ValueType::FLOAT32, false, true
    };
}
Format Format::R32_SINT() {
    return Format{
        1, ValueType::UINT32, false, true
    };
}
Format Format::R32_UINT() {
    return Format{
        2, ValueType::INT32, false, true
    };
}
Format Format::RG32_FLOAT() {
    return Format{
        2, ValueType::FLOAT32, false, true
    };
}
Format Format::RG32_UINT() {
    return Format{
        2, ValueType::UINT32, false, true
    };
}
Format Format::RG32_SINT() {
    return Format{
        2, ValueType::INT32, false, true
    };
}
Format Format::RGBA8_UINT() {
    return Format{
        4, ValueType::UINT8, false, true
    };
}
Format Format::RGBA8_UNORM() {
    return Format{
        4, ValueType::UINT8, true, true
    };
}
Format Format::RGBA8_SINT() {
    return Format{
        4, ValueType::INT8, false, true
    };
}
Format Format::RGBA8_SNORM() {
    return Format{
        4, ValueType::INT8, true, true
    };
}
Format Format::SRGBA8_UNORM() {
    return Format{
        4, ValueType::UINT8, true, false
    };
}
Format Format::R8_SINT() {
    return Format{
        1, ValueType::INT8, false, true 
    };
}
Format Format::R8_UINT() {
    return Format{
        1, ValueType::UINT8, false, true 
    };
}
Format Format::R8_SNORM() {
    return Format{
        1, ValueType::INT8, true, true 
    };
}
Format Format::R8_UNORM() {
    return Format{
        1, ValueType::UINT8, true, true 	
    };
}
Format Format::RG8_SINT() {
    return Format{
        2, ValueType::INT8, false, true  
    };
}
Format Format::RG8_UINT() {
    return Format{
        2, ValueType::UINT8, false, true 
    };
}
Format Format::RG8_SNORM() {
    return Format{
        2, ValueType::INT8, true, true 
    };
}
Format Format::RG8_UNORM() {
    return Format{
        2, ValueType::UINT8, true, true 	
    };
}
Format Format::RGB8_SINT() {
    return Format{
        3, ValueType::INT8, false, true  
    };
}
Format Format::RGB8_UINT() {
    return Format{
        3, ValueType::UINT8, false, true 
    };
}
Format Format::RGB8_SNORM() {
    return Format{
        3, ValueType::INT8, true, true 
    };
}
Format Format::RGB8_UNORM() {
    return Format{
        3, ValueType::UINT8, true, true 	
    };
}
Format Format::R16_SINT() {
    return Format{
        1, ValueType::INT16, false, true 
    };
}
Format Format::R16_UINT() {
    return Format{
        1, ValueType::UINT16, false, true 
    };
}
Format Format::R16_SNORM() {
    return Format{
        1, ValueType::INT16, true, true 
    };
}
Format Format::R16_UNORM() {
    return Format{
        1, ValueType::UINT16, true, true 
    };
}
Format Format::RG16_SINT() {
    return Format{
        2, ValueType::INT16, false, true 
    };
}
Format Format::RG16_UINT() {
    return Format{
        2, ValueType::UINT16, false, true 
    };
}
Format Format::RG16_SNORM() {
    return Format{
        2, ValueType::INT16, true, true 
    };
}
Format Format::RG16_UNORM() {
    return Format{
        2, ValueType::UINT16, true, true 
    };
}
Format Format::RGBA16_SINT() {
    return Format{
        4, ValueType::INT16, false, true 
    };
}
Format Format::RGBA16_UINT() {
    return Format{
        4, ValueType::UINT16, false, true 
    };
}
Format Format::RGBA16_SNORM() {
    return Format{
        4, ValueType::INT16, true, true 
    };
}
Format Format::RGBA16_UNORM() {
    return Format{
        4, ValueType::UINT16, true, true 
    };
}

uint texture::MipCount(
    const uint width, 
    const uint height) {
    return (uint)(1 + std::floor(std::log2(std::max(width, height))));
}

uint texture::MipCount(
    const uint width, 
    const uint height, 
    const uint depth) {
    return (uint)(1 + std::floor(
        std::log2(std::max(width, std::max(height, depth)))));
}

std::vector<SubResDataDesc> Desc::GetSubresourceDescs() const {

    std::vector<SubResDataDesc> descs;

    auto pixelSize = GetPixelByteSize();
    size_t mip_count = GetMipCount();

    // Compute subresources and sizes
    size_t currentOffset = 0;
    for (size_t iarray = 0; iarray < arraySizeOrDepth; ++iarray) {
        for (size_t imip = 0; imip < mip_count; ++imip) {
            size_t mip_width = width;
            size_t mip_height = height;
            size_t mip_depth = arraySizeOrDepth;

            mip_width = std::max<size_t>(mip_width >> imip, 1u);
            mip_height = std::max<size_t>(mip_height >> imip, 1u);
            mip_depth = std::max<size_t>(mip_depth >> imip, 1u);

            size_t increment = mip_width * mip_height * 
                mip_depth * pixelSize * sampleCount;

            SubResDataDesc subDesc;
            subDesc.srcOffset = currentOffset;
            subDesc.depthStride = mip_width * mip_height * pixelSize * sampleCount;
            subDesc.stride = mip_width * pixelSize * sampleCount;
            subDesc.length = increment;
            subDesc.mip = imip;
            subDesc.slice = iarray;
            descs.emplace_back(subDesc);

            currentOffset += increment;
        }
    }

    return descs;
}

uint32_t Desc::GetByteSize() const {
    auto pixelSize = GetPixelByteSize();
    size_t mip_count = GetMipCount();

    // Compute subresources and sizes
    size_t currentOffset = 0;
    for (size_t iarray = 0; iarray < arraySizeOrDepth; ++iarray) {
        for (size_t imip = 0; imip < mip_count; ++imip) {
            size_t mip_width = width;
            size_t mip_height = height;
            size_t mip_depth = arraySizeOrDepth;

            mip_width = std::max<size_t>(mip_width >> imip, 1u);
            mip_height = std::max<size_t>(mip_height >> imip, 1u);
            mip_depth = std::max<size_t>(mip_depth >> imip, 1u);

            size_t increment = mip_width * mip_height * 
                mip_depth * pixelSize * sampleCount;
            currentOffset += increment;
        }
    }

    return (uint32_t)currentOffset;
}

Buffer Buffer::Alloc(const Desc& desc) {
    size_t sz = desc.GetByteSize();

    Buffer result;
    result.desc = desc;
    result.data.resize(sz);

    return result;
}

void Buffer::GenerateMips() {
    size_t mipCount = desc.mipLevels;
    bool isSRGB = !desc.format.isLinear;
    size_t pixelSize = desc.GetPixelByteSize();
    uint componentCount = desc.format.channels * desc.sampleCount;
    auto valueType = desc.format.valueType;

    auto subresources = desc.GetSubresourceDescs();

    size_t arrayLength = 1;

    switch (desc.type) {
        case Dimension::Texture2D:
            arrayLength = 1;
            break;
        case Dimension::Texture2DArray:
        case Dimension::TextureCube:
        case Dimension::TextureCubeArray:
            arrayLength = desc.arraySizeOrDepth;
            break;
        default:
            throw std::runtime_error("GenerateMips does not support this resource dimension type!");
    }

    uint iSubresource = 0;
    for (size_t arrayIndex = 0; arrayIndex < arrayLength; ++arrayIndex) {
        auto& baseSubDesc = subresources[iSubresource];
        auto last_mip_data = &data[baseSubDesc.srcOffset];
        ++iSubresource;

        for (size_t i = 1; i < mipCount; ++i) {
            auto& subDesc = subresources[iSubresource];
            auto new_mip_data = &data[subDesc.srcOffset];

            uint fineWidth = std::max<uint>(1u, desc.width >> (i - 1));
            uint fineHeight = std::max<uint>(1u, desc.height >> (i - 1));
            uint coarseWidth = std::max<uint>(1u, desc.width >> i);
            uint coarseHeight = std::max<uint>(1u, desc.height >> i);

            uint fineStride = (uint)(fineWidth * pixelSize);
            uint coarseStride = (uint)(coarseWidth * pixelSize);

            mip_generator_2d_t mip_gen;

            switch (valueType) {
                case ValueType::UINT8:
                    mip_gen = &ComputeCoarseMip2D<uint8_t>;
                    break;
                case ValueType::UINT16:
                    mip_gen = &ComputeCoarseMip2D<uint16_t>;
                    break;
                case ValueType::UINT32:
                    mip_gen = &ComputeCoarseMip2D<uint32_t>;
                    break;
                case ValueType::FLOAT32:
                    mip_gen = &ComputeCoarseMip2D<float>;
                    break;
                default:
                    throw std::runtime_error("Mip generation for texture type is not supported!");
            }

            mip_gen(componentCount, isSRGB, last_mip_data, fineStride, 
                fineWidth, fineHeight, new_mip_data, 
                coarseStride, coarseWidth, coarseHeight);

            last_mip_data = new_mip_data;
            ++iSubresource;
        }
    }
}

Buffer texture::LoadFromBytes_RGBA8_UNORM(
    const TextureLoadParams& params, 
    const std::vector<uint8_t>& image,
    uint32_t width, uint32_t height) {
    
    std::vector<uint8_t> rawData;
    std::vector<SubResDataDesc> subDatas;
    rawData.resize(image.size() * 2);

    size_t currentIndx = 0;

    Format format;

    if (params.isSRGB) {
        format = Format::SRGBA8_UNORM();
    } else {
        format = Format::RGBA8_UNORM();
    }

    Desc desc;
    desc.width = width;
    desc.height = height;
    desc.mipLevels = params.generateMips ? ::MipCount(width, height) : 1;
    desc.format = format;
    desc.type = Dimension::Texture2D;
    desc.arraySizeOrDepth = 1;

    auto result = Buffer::Alloc(desc);
    auto subresources = desc.GetSubresourceDescs();

    std::memcpy(&result.data[0], &image[0], subresources[0].length);

    if (params.generateMips)
        result.GenerateMips();

    return result;
}

Buffer prefabs::SolidColor(
    uint width,
    uint height,
    glm::vec4 color) {

    Desc desc;
    desc.type = Dimension::Texture2D;
    desc.width = width;
    desc.height = height;
    desc.format = Format::SRGBA8_UNORM();

    auto data = Buffer::Alloc(desc);
    auto ucolor = ColorToBytes(color);
    auto src = SpanOf(ucolor);

    for (uint y = 0; y < height; ++y) {
        for (uint x = 0; x < width; ++x) {
            auto dest = std::span{&data.data[(x + y * width) * 4], 4};
            std::copy(src.begin(), src.end(), dest.begin());
        }
    }

    return Buffer(std::move(data));
}

Buffer prefabs::CheckerBoard(
        uint32_t width,
        uint32_t height,
        uint32_t widthSubdivisions,
        uint32_t heightSubdivisions,
        glm::vec4 color1,
        glm::vec4 color2
    ) {
    Desc desc;
    desc.type = Dimension::Texture2D;
    desc.width = width;
    desc.height = height;
    desc.format = Format::SRGBA8_UNORM();

    auto data = Buffer::Alloc(desc);

    auto ucolor1 = ColorToBytes(color1);
    auto ucolor2 = ColorToBytes(color2);

    auto src1 = SpanOf(ucolor1);
    auto src2 = SpanOf(ucolor2);

    uint32_t xSubdivSize = width / widthSubdivisions;
    uint32_t ySubdivSize = height / heightSubdivisions;

    for (uint y = 0; y < height; ++y) {
        for (uint x = 0; x < width; ++x) {
            auto dest = std::span{&data.data[(x + y * width) * 4], 4};
            
            auto tileX = x / xSubdivSize;
            auto tileY = y / ySubdivSize;

            if ((tileX + tileY) % 2 == 0) {
                std::copy(src1.begin(), src1.end(), dest.begin());
            } else {
                std::copy(src2.begin(), src2.end(), dest.begin());
            }
        }
    }

    return data;
}