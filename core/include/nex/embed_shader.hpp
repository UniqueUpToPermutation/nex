#pragma once

#include <nex/error.hpp>
#include <nex/embed.hpp>

#include <EngineFactory.h>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <DeviceContext.h>
#include <SwapChain.h>
#include <BasicMath.hpp>

#include <string>
#include <filesystem>

namespace nex {
	namespace dg = Diligent;

	struct EmbeddedShaderParams {
		std::filesystem::path path;
		dg::SHADER_TYPE shaderType;
		std::string name;		
		std::unordered_map<std::string, std::string> const* defines = nullptr;
        std::unordered_map<std::string, std::string> const* overrides = nullptr;
		std::string entryPoint = "main";
	};

	Expected<dg::IShader*> CompileEmbeddedShader(
        dg::IRenderDevice& device,
		const EmbeddedShaderParams& params,
		IVirtualFileSystem& fileLoader);
}