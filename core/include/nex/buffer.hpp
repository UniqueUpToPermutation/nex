#pragma once

#include <RenderDevice.h>
#include <MapHelper.hpp>

#include <cstring>


namespace nex {
    namespace dg = Diligent;

    template <typename T>
	class StaticUniformBuffer {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> 	_buffer;
		size_t							_count;

	public:
        StaticUniformBuffer() = default;
		StaticUniformBuffer(
            dg::IRenderDevice* device,
            dg::IDeviceContext* context,
            const std::vector<T>& data) : 
				_count(data.size()) {

			dg::BufferDesc desc;
			desc.Name           = "Static Uniform Buffer";
			desc.Size  		    = sizeof(T) * _count;
			desc.Usage          = dg::USAGE_IMMUTABLE;
			desc.BindFlags      = dg::BIND_UNIFORM_BUFFER;
			desc.CPUAccessFlags = dg::CPU_ACCESS_NONE;

            dg::BufferData dgData;
            dgData.DataSize        = sizeof(T) * _count;
            dgData.pContext        = context;
            dgData.pData           = &data[0];

            dg::IBuffer* buf = nullptr;
			device->CreateBuffer(desc, &dgData, &buf);
            _buffer.Attach(buf);
		}

		inline dg::IBuffer* Get() {
			return _buffer.RawPtr();
		}

		inline size_t Count() {
			return _count;
		}
	};

	template <typename T>
	class DynamicUniformBuffer {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> _buffer;
		size_t _count;

	public:
        DynamicUniformBuffer() = default;
		DynamicUniformBuffer(
			dg::IRenderDevice* device, 
			const uint count = 1) : 
			_count(count) {

			dg::BufferDesc desc;
			desc.Name           	= "Dynamic Uniform Buffer";
			desc.Size  		  	    = sizeof(T) * count;
			desc.Usage          	= dg::USAGE_DYNAMIC;
			desc.BindFlags      	= dg::BIND_UNIFORM_BUFFER;
			desc.CPUAccessFlags 	= dg::CPU_ACCESS_WRITE;

            dg::IBuffer* buf = nullptr;
			device->CreateBuffer(desc, nullptr, &buf);
            _buffer.Attach(buf);
		}

		inline size_t Count() const {
			return _count;
		}

		inline dg::IBuffer* Get() {
			return _buffer.RawPtr();
		}

		inline void Write(dg::IDeviceContext* context, const T& t) {
			dg::MapHelper<T> data(context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
			*data = t;
		}

		inline void Write(dg::IDeviceContext* context, const T t[], const uint count) {
			dg::MapHelper<T> data(context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
			std::memcpy(data, t, sizeof(T) * count);
		}
	};

	template <typename T>
	class DynamicStructuredBuffer {
	private:
		dg::RefCntAutoPtr<dg::IBuffer> _buffer;
		size_t _count;

	public:
		DynamicStructuredBuffer() = default;
		DynamicStructuredBuffer(
			dg::IRenderDevice* device, 
			const uint count) : 
			_count(count) {

			dg::BufferDesc desc;
			desc.Name           		= "Dyanmic Structured Buffer";
			desc.Size  		  		    = sizeof(T) * count;
			desc.Usage          		= dg::USAGE_DYNAMIC;
			desc.BindFlags      		= dg::BIND_FLAGS::BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags 		= dg::CPU_ACCESS_WRITE;
			desc.ElementByteStride 	    = sizeof(T);
			desc.Mode 				    = dg::BUFFER_MODE::BUFFER_MODE_STRUCTURED;

            dg::IBuffer* buf = nullptr;
			device->CreateBuffer(desc, nullptr, &buf);
            _buffer.Attach(buf);
		}

		inline size_t Count() const {
			return _count;
		}

		inline dg::IBuffer* Get() {
			return _buffer.RawPtr();
		}

		inline dg::IBufferView* GetView() {
			return _buffer->GetDefaultView(dg::BUFFER_VIEW_SHADER_RESOURCE);
		}

		inline void Write(dg::IDeviceContext* context, const T& t) {
			dg::MapHelper<T> data(context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
			*data = t;
		}

		inline void Write(dg::IDeviceContext* context, const T t[], const uint count) {
			dg::MapHelper<T> data(context, _buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
			std::memcpy(data, t, sizeof(T) * count);
		}
	};
}