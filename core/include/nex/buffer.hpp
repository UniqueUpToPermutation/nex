#pragma once

#include <RenderDevice.h>
#include <MapHelper.hpp>

#include <cstring>


namespace nex::gfx {
    namespace dg = Diligent;

    template <typename T>
	class StaticUniformBuffer {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> 	_buffer;

	public:
		static Expected<StaticUniformBuffer<T>> Create(
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
			NEX_EXP_RETURN_IF(buf == nullptr, RuntimeError{"Failed to create static uniform buffer!"});

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

	template <typename T>
	class DynamicUniformBuffer {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> _buffer;

	public:
		static Expected<DynamicUniformBuffer<T>> Create(
			dg::IRenderDevice& device, 
			const uint count = 1) {
			dg::BufferDesc desc;
			desc.Name           	= "Dynamic Uniform Buffer";
			desc.Size  		  	    = sizeof(T) * count;
			desc.Usage          	= dg::USAGE_DYNAMIC;
			desc.BindFlags      	= dg::BIND_UNIFORM_BUFFER;
			desc.CPUAccessFlags 	= dg::CPU_ACCESS_WRITE;

            dg::IBuffer* buf = nullptr;
			device.CreateBuffer(desc, nullptr, &buf);
			NEX_EXP_RETURN_IF(buf == nullptr, RuntimeError{"Failed to create dynamic uniform buffer!"});

			DynamicUniformBuffer<T> result;
            result._buffer.Attach(buf);
			return result;
		}

		inline size_t Count() const {
			return _buffer->GetDesc().Size / sizeof(T);
		}

		inline dg::IBuffer* Get() {
			return _buffer.RawPtr();
		}

		inline void Write(dg::IDeviceContext& context, const T& t) {
			dg::MapHelper<T> data(&context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
			*data = t;
		}

		inline void Write(dg::IDeviceContext& context, const T t[], const uint count) {
			dg::MapHelper<T> data(&context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
			std::memcpy(data, t, sizeof(T) * count);
		}
	};

	template <typename T>
	class DynamicStructuredBuffer {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> _buffer;

	public:
		Expected<DynamicStructuredBuffer<T>> Create(
			dg::IRenderDevice& device, 
			const uint count) {

			dg::BufferDesc desc;
			desc.Name           		= "Dyanmic Structured Buffer";
			desc.Size  		  		    = sizeof(T) * count;
			desc.Usage          		= dg::USAGE_DYNAMIC;
			desc.BindFlags      		= dg::BIND_FLAGS::BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags 		= dg::CPU_ACCESS_WRITE;
			desc.ElementByteStride 	    = sizeof(T);
			desc.Mode 				    = dg::BUFFER_MODE::BUFFER_MODE_STRUCTURED;

            dg::IBuffer* buf = nullptr;
			device.CreateBuffer(desc, nullptr, &buf);
			NEX_EXP_RETURN_IF(buf == nullptr, RuntimeError{"Failed to create dynamic structured buffer!"});

			DynamicStructuredBuffer<T> result;
            result._buffer.Attach(buf);
			return result;
		}

		inline size_t Count() const {
			return _buffer->GetDesc().Size / sizeof(T);
		}

		inline dg::IBuffer* Get() {
			return _buffer.RawPtr();
		}

		inline dg::IBufferView* GetView() {
			return _buffer->GetDefaultView(dg::BUFFER_VIEW_SHADER_RESOURCE);
		}

		inline void Write(dg::IDeviceContext& context, const T& t) {
			dg::MapHelper<T> data(&context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
			*data = t;
		}

		inline void Write(dg::IDeviceContext& context, const T t[], const uint count) {
			dg::MapHelper<T> data(&context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
			std::memcpy(data, t, sizeof(T) * count);
		}
	};
}