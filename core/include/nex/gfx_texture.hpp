#pragma once

#include <nex/texture.hpp>

#include <Texture.h>
#include <RenderDevice.h>
#include <DeviceContext.h>
#include <RefCntAutoPtr.hpp>

namespace dg = Diligent;

namespace nex::gfx {
    dg::RESOURCE_DIMENSION ToDiligent(texture::Dimension dim);
    dg::VALUE_TYPE ToDiligent(ValueType valueType);
    dg::TEXTURE_FORMAT ToDiligent(const TextureFormat& format);
    dg::TextureDesc ToDiligent(TextureDesc const& desc);
    dg::TextureSubResData ToDiligent(TextureBuffer const& buffer, texture::SubResDataDesc const& sub);

    dg::RefCntAutoPtr<dg::ITexture> UploadToGpu(
        dg::IRenderDevice& device, 
        dg::IDeviceContext& context,
        TextureBuffer const& texture);
}