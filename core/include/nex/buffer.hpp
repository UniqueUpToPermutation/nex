#pragma once

#include <RenderDevice.h>
#include <MapHelper.hpp>

#include <cstring>
#include <span>


namespace nex::gfx {
    namespace dg = Diligent;

    template <typename T>
	class StaticUniformBuffer {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> 	_buffer;

	public:
		static StaticUniformBuffer<T> Create(
            dg::IRenderDevice& device,
            dg::IDeviceContext& context,
            const std::vector<T>& data) {

			dg::BufferDesc desc;
			desc.Name           = "Static Uniform Buffer";
			desc.Size  		    = sizeof(T) * data.size();
			desc.Usage          = dg::USAGE_IMMUTABLE;
			desc.BindFlags      = dg::BIND_UNIFORM_BUFFER;
			desc.CPUAccessFlags = dg::CPU_ACCESS_NONE;

            dg::BufferData dgData;
            dgData.DataSize        = sizeof(T) * data.size();
            dgData.pContext        = &context;
            dgData.pData           = &data[0];

            dg::IBuffer* buf = nullptr;
			device.CreateBuffer(desc, &dgData, &buf);

			StaticUniformBuffer<T> result;
            result._buffer.Attach(buf);
			return result;
		}

		inline dg::IBuffer* Get() {
			return _buffer.RawPtr();
		}

		inline size_t Count() {
			return _buffer->GetDesc().Size / sizeof(T);
		}
	};

	struct DynamicBufferCI {
		dg::BIND_FLAGS bindFlags = dg::BIND_UNIFORM_BUFFER;
		dg::BUFFER_MODE mode = dg::BUFFER_MODE_UNDEFINED;
		uint count = 1;
		const char* name = "Dynamic Buffer";

		inline static DynamicBufferCI UniformBuffer(uint count = 1) { 
			return DynamicBufferCI{
				.count = count, 
				.name = "Dynamic Uniform Buffer"
			}; 
		}
		inline static DynamicBufferCI VertexBuffer(uint count) { 
			return DynamicBufferCI{
				.bindFlags = dg::BIND_VERTEX_BUFFER,
				.count = count, 
				.name = "Dynamic Vertex Buffer"
			}; 
		}
	};

	template <typename T>
	class DynamicBuffer {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> _buffer;

	public:
		static DynamicBuffer<T> Create(
			dg::IRenderDevice& device,
			DynamicBufferCI const& ci) {
			dg::BufferDesc desc;
			desc.Name           	= "Dynamic Buffer";
			desc.Size  		  	    = sizeof(T) * ci.count;
			desc.Usage          	= dg::USAGE_DYNAMIC;
			desc.BindFlags      	= ci.bindFlags;
			desc.CPUAccessFlags 	= dg::CPU_ACCESS_WRITE;
			desc.Mode				= ci.mode;

            dg::IBuffer* buf = nullptr;
			device.CreateBuffer(desc, nullptr, &buf);

			DynamicBuffer<T> result;
            result._buffer.Attach(buf);
			return result;
		}

		inline size_t Count() const {
			return _buffer->GetDesc().Size / sizeof(T);
		}

		inline dg::IBuffer* Get() {
			return _buffer.RawPtr();
		}

		inline dg::MapHelper<T> Map(dg::IDeviceContext& context) {
			return dg::MapHelper<T>(&context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
		}

		inline void Write(dg::IDeviceContext& context, const T& t) {
			auto data = Map(context);
			*data = t;
		}

		inline void Write(dg::IDeviceContext& context, std::span<const T> t) {
			auto data = Map(context);
			std::memcpy(data, t.data(), sizeof(T) * t.size());
		}		
	};
}