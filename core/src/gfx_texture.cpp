#include <nex/gfx_texture.hpp>

using namespace nex;
using namespace nex::gfx;
using namespace nex::texture;

using namespace Diligent;

RESOURCE_DIMENSION gfx::ToDiligent(texture::Dimension dim) {
    switch (dim) {
    case texture::Dimension::Buffer:
        return RESOURCE_DIM_BUFFER;
    case texture::Dimension::Texture1D:
        return RESOURCE_DIM_TEX_1D;
    case texture::Dimension::Texture1DArray:
        return RESOURCE_DIM_TEX_1D_ARRAY;
    case texture::Dimension::Texture2D:
        return RESOURCE_DIM_TEX_2D;
    case texture::Dimension::Texture2DArray:
        return RESOURCE_DIM_TEX_2D_ARRAY;
    case texture::Dimension::Texture3D:
        return RESOURCE_DIM_TEX_3D;
    case texture::Dimension::TextureCube:
        return RESOURCE_DIM_TEX_CUBE;
    case texture::Dimension::TextureCubeArray:
        return RESOURCE_DIM_TEX_CUBE_ARRAY;
    default:
        throw std::runtime_error("Unrecognized ResourceDimension type!");
    }
}

VALUE_TYPE gfx::ToDiligent(ValueType valueType) {
    switch (valueType) {
    case ValueType::INT8:
        return VT_INT8;
    case ValueType::INT16:
        return VT_UINT16;
    case ValueType::INT32:
        return VT_UINT32;
    case ValueType::UINT8:
        return VT_UINT8;
    case ValueType::UINT16:
        return VT_UINT16;
    case ValueType::UINT32:
        return VT_UINT32;
    case ValueType::FLOAT16:
        return VT_FLOAT16;
    case ValueType::FLOAT32:
        return VT_FLOAT32;
    default:
        throw std::runtime_error("Unrecognized value type!");
    }
}

TEXTURE_FORMAT gfx::ToDiligent(const TextureFormat& format) {
    if (format.channels == 0) {
        return TEX_FORMAT_UNKNOWN;
    }

    switch (format.valueType) {
    case ValueType::FLOAT32:
        switch (format.channels) {
        case 1:
            return TEX_FORMAT_R32_FLOAT;
        case 2:
            return TEX_FORMAT_RG32_FLOAT;
        case 3:
            return TEX_FORMAT_RGB32_FLOAT;
        case 4:
            return TEX_FORMAT_RGBA32_FLOAT;
        default:
            throw std::runtime_error("Invalid format!");
        }
    case ValueType::FLOAT16:
        switch (format.channels) {
        case 1:
            return TEX_FORMAT_R16_FLOAT;
        case 2:
            return TEX_FORMAT_RG16_FLOAT;
        case 4:
            return TEX_FORMAT_RGBA16_FLOAT;
        default:
            throw std::runtime_error("Invalid format!");
        }
    case ValueType::UINT8:
        if (format.isNormalized) {
            if (format.isLinear) {
                switch (format.channels) {
                case 1:
                    return TEX_FORMAT_R8_UNORM;
                case 2:
                    return TEX_FORMAT_RG8_UNORM;
                case 4:
                    return TEX_FORMAT_RGBA8_UNORM;
                default:
                    throw std::runtime_error("Invalid format!");
                }
            } else {
                switch (format.channels) {
                case 4:
                    return TEX_FORMAT_RGBA8_UNORM_SRGB;
                default:
                    throw std::runtime_error("Invalid format!");
                }
            }
        } else {
            switch (format.channels) {
            case 1:
                return TEX_FORMAT_R8_UINT;
            case 2:
                return TEX_FORMAT_RG8_UINT;
            case 4:
                return TEX_FORMAT_RGBA8_UINT;
            default:
                throw std::runtime_error("Invalid format!");
            }
        }
    case ValueType::UINT16:
        if (format.isNormalized) {
            if (format.isLinear) {
                switch (format.channels) {
                case 1:
                    return TEX_FORMAT_R16_UNORM;
                case 2:
                    return TEX_FORMAT_RG16_UNORM;
                case 4:
                    return TEX_FORMAT_RGBA16_UNORM;
                default:
                    throw std::runtime_error("Invalid format!");
                }
            } else {
                throw std::runtime_error("Invalid format!");
            }
        } else {
            switch (format.channels) {
            case 1:
                return TEX_FORMAT_R16_UINT;
            case 2:
                return TEX_FORMAT_RG16_UINT;
            case 4:
                return TEX_FORMAT_RGBA16_UINT;
            default:
                throw std::runtime_error("Invalid format!");
            }
        }
    case ValueType::UINT32:
        switch (format.channels) {
        case 1:
            return TEX_FORMAT_R32_UINT;
        case 2:
            return TEX_FORMAT_RG32_UINT;
        case 4:
            return TEX_FORMAT_RGBA32_UINT;
        default:
            throw std::runtime_error("Invalid format!");
        }
    case ValueType::INT8:
        if (format.isNormalized) {
            switch (format.channels) {
            case 1:
                return TEX_FORMAT_R8_SNORM;
            case 2:
                return TEX_FORMAT_RG8_SNORM;
            case 4:
                return TEX_FORMAT_RGBA8_SNORM;
            default:
                throw std::runtime_error("Invalid format!");
            }
        } else {
            switch (format.channels) {
            case 1:
                return TEX_FORMAT_R8_SINT;
            case 2:
                return TEX_FORMAT_RG8_SINT;
            case 4:
                return TEX_FORMAT_RGBA8_SINT;
            default:
                throw std::runtime_error("Invalid format!");
            }
        }
    case ValueType::INT16:
        if (format.isNormalized) {
            switch (format.channels) {
            case 1:
                return TEX_FORMAT_R16_SNORM;
            case 2:
                return TEX_FORMAT_RG16_SNORM;
            case 4:
                return TEX_FORMAT_RGBA16_SNORM;
            default:
                throw std::runtime_error("Invalid format!");
            }
        } else {
            switch (format.channels) {
            case 1:
                return TEX_FORMAT_R16_SINT;
            case 2:
                return TEX_FORMAT_RG16_SINT;
            case 4:
                return TEX_FORMAT_RGBA16_SINT;
            default:
                throw std::runtime_error("Invalid format!");
            }
        }
    case ValueType::INT32:
        switch (format.channels) {
        case 1:
            return TEX_FORMAT_R32_SINT;
        case 2:
            return TEX_FORMAT_RG32_SINT;
        case 4:
            return TEX_FORMAT_RGBA32_SINT;
        default:
            throw std::runtime_error("Invalid format!");
        }
    default:
        throw std::runtime_error("Invalid format!");
    }
}

dg::TextureDesc gfx::ToDiligent(TextureDesc const& desc) {
    dg::TextureDesc result;
    result.Type = ToDiligent(desc.type);
    result.Width = desc.width;
    result.Height = desc.height;
    if (texture::IsArray(desc.type)) {
        result.ArraySize = desc.arraySizeOrDepth;
    } else {
        result.Depth = desc.arraySizeOrDepth;
    }
    result.Format = ToDiligent(desc.format);
    result.MipLevels = desc.mipLevels;
    result.BindFlags = dg::BIND_SHADER_RESOURCE;
    result.Usage = dg::USAGE_IMMUTABLE;
    return result;
}

dg::TextureSubResData gfx::ToDiligent(TextureBuffer const& buffer, texture::SubResDataDesc const& sub) {
    dg::TextureSubResData result;
    result.DepthStride = sub.depthStride;
    result.Stride = sub.stride;
    result.pData = &buffer.data[sub.srcOffset];
    return result;
}

dg::RefCntAutoPtr<dg::ITexture> gfx::UploadToGpu(
    dg::IRenderDevice& device,
    dg::IDeviceContext& context,
    TextureBuffer const& texture) {

    auto desc = ToDiligent(texture.GetDesc());
    auto subs = texture.desc.GetSubresourceDescs();

    std::vector<dg::TextureSubResData> dgSubs;
    dgSubs.reserve(subs.size());
    std::transform(subs.begin(), subs.end(), std::back_inserter(dgSubs), 
        [&](texture::SubResDataDesc const& input) {
            return ToDiligent(texture, input);
    });

    dg::TextureData data;
    data.NumSubresources = dgSubs.size();
    data.pSubResources = dgSubs.data();
    data.pContext = &context;

    dg::RefCntAutoPtr<dg::ITexture> tex;
    device.CreateTexture(desc, &data, &tex);
    return tex;
}