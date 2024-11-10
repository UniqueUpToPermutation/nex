#pragma once

#include <RenderDevice.h>
#include <RefCntAutoPtr.hpp>

#include <nex/error.hpp>
#include <nex/buffer.hpp>
#include <nex/embed.hpp>

#include <nex/hlsl.hpp>

#include <span>

#define DEFAULT_SPRITE_BATCH_SIZE 500

namespace dg = Diligent;

namespace nex::gfx {
	struct SpriteShaders {
		dg::RefCntAutoPtr<dg::IShader> vs;
		dg::RefCntAutoPtr<dg::IShader> gs;
		dg::RefCntAutoPtr<dg::IShader> ps;

		static Expected<SpriteShaders> Create(
			dg::IRenderDevice& device, 
			IVirtualFileSystem& system);
	};

    struct SpriteBatchPipelineCI {
        SpriteShaders const& shaders;
        DynamicBuffer<hlsl::SceneGlobals>& globals;
        dg::TEXTURE_FORMAT backbufferFormat;
        dg::TEXTURE_FORMAT depthbufferFormat;
        dg::FILTER_TYPE filterType = dg::FILTER_TYPE_LINEAR;
        uint samples = 1;
    };

	struct SpriteBatchPipeline {
		dg::RefCntAutoPtr<dg::IPipelineState> pso;
	};

    Expected<SpriteBatchPipeline> CreateSpriteBatchPipeline(
        dg::IRenderDevice& device,
        SpriteBatchPipelineCI const& ci);

    struct SpriteBatchBindings {
		dg::IShaderResourceVariable* textureVariable = nullptr;
		dg::RefCntAutoPtr<dg::IShaderResourceBinding> shaderBinding;
	};

    SpriteBatchBindings CreateSpriteBatchBindings(dg::IPipelineState& pso);

    struct SpriteBatchRequirements {
        SpriteBatchPipeline pipeline;
        SpriteBatchBindings bindings;

        static Expected<SpriteBatchRequirements> Create(
            dg::IRenderDevice& device, 
            SpriteBatchPipelineCI const& ci);
    };

    struct SpriteRect {
        glm::vec2 pos;
        glm::vec2 size;
    };

    struct SpriteBatchCI {
        uint batchSize = DEFAULT_SPRITE_BATCH_SIZE;
    };

	class SpriteBatch {
	private:
		DynamicBuffer<hlsl::SpriteBatchVSInput> buffer;

		dg::IDeviceContext* currentContext = nullptr;
        dg::IPipelineState* pso = nullptr;
        dg::IShaderResourceBinding* srb = nullptr;
        dg::IShaderResourceVariable* textureVariable = nullptr;

        dg::MapHelper<hlsl::SpriteBatchVSInput> map;

        struct Group {
            dg::ITexture& texture;
            uint start;
            uint size;
        };

        std::vector<Group> groups;

        uint GetWriteIndex() const;
        void Flush(bool shouldRemap);
        void AssureGroup(dg::ITexture& texture);

        inline SpriteBatch(DynamicBuffer<hlsl::SpriteBatchVSInput> buffer) :
            buffer(buffer) {}

	public:
        static SpriteBatch Create(dg::IRenderDevice& device, SpriteBatchCI const& ci = {});

		void Begin(
            dg::IDeviceContext& context, 
            dg::IPipelineState& pso, 
            dg::IShaderResourceBinding& srb,
            dg::IShaderResourceVariable& textureVariable);
        void Begin(dg::IDeviceContext& context,
            SpriteBatchRequirements& requirements);
		void End();

		void Draw(
            dg::ITexture& texture,
            std::span<hlsl::SpriteBatchVSInput const> raw);

		void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const glm::vec2& size, 
            const SpriteRect& rect, 
			const glm::vec2& origin, 
            const float rotation, 
			const glm::vec4& color);

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const glm::vec2& size,
			const glm::vec2& origin, const float rotation) {
			auto& desc = texture.GetDesc();
			auto dimensions = glm::vec2(desc.Width, desc.Height);
			Draw(texture, pos,  size, SpriteRect{glm::vec2(0.0f, 0.0f), dimensions},
				origin, rotation, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const glm::vec2& size,
			const glm::vec2& origin, const float rotation,
			const glm::vec4& color) {
			auto& desc = texture.GetDesc();
			auto dimensions = glm::vec2(desc.Width, desc.Height);
			Draw(texture, pos,  size, SpriteRect{glm::vec2(0.0f, 0.0f), dimensions},
				origin, rotation, color); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos) {
			auto& desc = texture.GetDesc();
			auto dimensions = glm::vec2(desc.Width, desc.Height);
			Draw(texture, pos, dimensions, SpriteRect{glm::vec2(0.0, 0.0), dimensions},
				glm::vec2(0.0, 0.0), 0.0, glm::vec4(1.0, 1.0, 1.0, 1.0)); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const glm::vec4& color) {
			auto& desc = texture.GetDesc();
			auto dimensions = glm::vec2(desc.Width, desc.Height);
			Draw(texture, pos, dimensions, SpriteRect{glm::vec2(0.0, 0.0), dimensions},
				glm::vec2(0.0, 0.0), 0.0, color); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const SpriteRect& rect, 
            const glm::vec4& color) {
			Draw(texture, pos, rect.size, rect,
				glm::vec2(0.0, 0.0), 0.0, color); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const SpriteRect& rect) {
			Draw(texture, pos, rect.size, rect,
				glm::vec2(0.0, 0.0), 0.0, 
				glm::vec4(1.0, 1.0, 1.0, 1.0)); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const SpriteRect& rect, 
            const glm::vec2& origin, 
            const float rotation) {
			Draw(texture, pos, rect.size, rect,
				origin, rotation, 
				glm::vec4(1.0, 1.0, 1.0, 1.0)); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const glm::vec2& origin, 
            const float rotation, 
            const glm::vec4& color) {
			auto& desc = texture.GetDesc();
			auto dimensions = glm::vec2(desc.Width, desc.Height);
			Draw(texture, pos, dimensions, SpriteRect{glm::vec2(0.0, 0.0), dimensions},
				origin, rotation, color); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const glm::vec2& origin, 
            const float rotation) {
			auto& desc = texture.GetDesc();
			auto dimensions = glm::vec2(desc.Width, desc.Height);
			Draw(texture, pos, dimensions, SpriteRect{glm::vec2(0.0, 0.0), dimensions},
				origin, rotation, glm::vec4(1.0, 1.0, 1.0, 1.0)); 
		}

		inline void Draw(
            dg::ITexture& texture, 
            const glm::vec2& pos, 
			const SpriteRect& rect, 
            const glm::vec2& origin, 
			const float rotation, 
            const glm::vec4& color) {
			Draw(texture, pos, rect.size, rect,
				origin, rotation, color); 
		}
	};
}