#pragma once

#include <RefCntAutoPtr.hpp>
#include <Shader.h>
#include <PipelineState.h>
#include <RenderDevice.h>
#include <DeviceContext.h>

#include <nex/embed.hpp>
#include <nex/buffer.hpp>
#include <nex/hlsl.hpp>

#include <im3d.h>

namespace dg = Diligent;

namespace nex::gfx {
    struct Im3dShaders {
		dg::RefCntAutoPtr<dg::IShader> trianglesVS;
		dg::RefCntAutoPtr<dg::IShader> pointsVS;
		dg::RefCntAutoPtr<dg::IShader> linesVS;
		dg::RefCntAutoPtr<dg::IShader> pointsGS;
		dg::RefCntAutoPtr<dg::IShader> linesGS;
		dg::RefCntAutoPtr<dg::IShader> trianglesPS;
		dg::RefCntAutoPtr<dg::IShader> linesPS;
		dg::RefCntAutoPtr<dg::IShader> pointsPS;

		static Expected<Im3dShaders> Create(
            dg::IRenderDevice& device, 
            IVirtualFileSystem& system);
	};

	struct Im3dPipelineCI {
		DynamicBuffer<hlsl::SceneGlobals>& globals;
		Im3dShaders& shaders;
		dg::TEXTURE_FORMAT backbufferColorFormat;
		dg::TEXTURE_FORMAT backbufferDepthFormat;
		uint samples = 1;
		bool enableDepth = true;
	};

	struct Im3dPipeline {
		dg::RefCntAutoPtr<dg::IPipelineState> pipelineStateVertices;
		dg::RefCntAutoPtr<dg::IPipelineState> pipelineStateLines;
		dg::RefCntAutoPtr<dg::IPipelineState> pipelineStateTriangles;
		dg::RefCntAutoPtr<dg::IShaderResourceBinding> vertexSRB;
		dg::RefCntAutoPtr<dg::IShaderResourceBinding> linesSRB;
		dg::RefCntAutoPtr<dg::IShaderResourceBinding> triangleSRB;
		Im3dShaders shaders;

        static Expected<Im3dPipeline> Create(
			dg::IRenderDevice& device,
			Im3dPipelineCI const& ci);
	};

	class Im3dModule {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> _geometryBuffer;

	public:
		static constexpr uint kDefaultBufferSize = 500;

		static Expected<Im3dModule> Create(
			dg::IRenderDevice& device, 
			uint bufferSize = kDefaultBufferSize);

		void Draw(dg::IDeviceContext& deviceContext,
			Im3dPipeline& state,
			Im3d::Context& im3dContext = Im3d::GetContext());

		uint GetBufferCount() const;
	};
}