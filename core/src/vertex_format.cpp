#include <nex/vertex_format.hpp>

using namespace nex;

int nex::GetSize(ValueType v) {
	switch (v) {
		case ValueType::FLOAT32:
			return 4;
		case ValueType::FLOAT16:
			return 2;
		case ValueType::INT8:
			return 1;
		case ValueType::INT16:
			return 2;
		case ValueType::INT32:
			return 4;
		case ValueType::UINT8:
			return 1;
		case ValueType::UINT16:
			return 2;
		case ValueType::UINT32:
			return 4;
		default:
			return -1;
	}
}

#define VERTEX_FORMAT_CASE(x) \
	case VertexFormat::x: \
		return VertexFormatInfo::x()

Expected<VertexFormatInfo> VertexFormatInfo::From(VertexFormat format) {
	switch (format) {
		VERTEX_FORMAT_CASE(Position);
		VERTEX_FORMAT_CASE(PositionColor);
		VERTEX_FORMAT_CASE(PositionUV);
		VERTEX_FORMAT_CASE(PositionUVNormal);
		VERTEX_FORMAT_CASE(PositionUVNormalTangent);
		VERTEX_FORMAT_CASE(PositionUVNormalTangentBitangent);
		default:
			Error err;
			NEX_ERR_SET(err, RuntimeError{"Could not get vertex format info!"});
			return MakeUnexpected(err);
	}
}

VertexFormatInfo VertexFormatInfo::Position() {
	VertexFormatInfo layout;
	layout.position = 0;
	layout.formatTag = VertexFormat::Position;

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX)
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormatInfo VertexFormatInfo::PositionColor() {
	VertexFormatInfo layout;
	layout.formatTag = VertexFormat::PositionColor;

	layout.position = 0;
	layout.colors = {1};

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 4, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormatInfo VertexFormatInfo::PositionUVNormalTangent() {
	VertexFormatInfo layout;
	layout.formatTag = VertexFormat::PositionUVNormalTangent;

	layout.position = 0;
	layout.uvs = {1};
	layout.normal = 2;
	layout.tangent = 3;

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 2, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(2, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(3, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormatInfo VertexFormatInfo::PositionUVNormal() {
	VertexFormatInfo layout;
	layout.formatTag = VertexFormat::PositionUVNormal;

	layout.position = 0;
	layout.uvs = {1};
	layout.normal = 2;

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 2, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(2, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormatInfo VertexFormatInfo::PositionUVNormalTangentBitangent() {
	VertexFormatInfo layout;
	layout.formatTag = VertexFormat::PositionUVNormalTangentBitangent;

	layout.position = 0;
	layout.uvs = {1};
	layout.normal = 2;
	layout.tangent = 3;
	layout.bitangent = 4;

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 2, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(2, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(3, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(4, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}

VertexFormatInfo VertexFormatInfo::PositionUV() {
	VertexFormatInfo layout;
	layout.formatTag = VertexFormat::PositionUV;

	layout.position = 0;
	layout.uvs = {1};

	std::vector<LayoutElement> layoutElements = {
		LayoutElement(0, 0, 3, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
		LayoutElement(1, 0, 2, ValueType::FLOAT32, false, InputElementFrequency::PER_VERTEX),
	};

	layout.elements = std::move(layoutElements);
	layout.AutoLayout();
	return layout;
}


Error VertexFormatInfo::CheckValid() const {
	int numElements = static_cast<int>(elements.size());

	NEX_ERR_RETURN_IF(position >= numElements, RuntimeError{"Invalid position index!"});
	NEX_ERR_RETURN_IF(normal >= numElements, RuntimeError{"Invalid normal index!"});
	NEX_ERR_RETURN_IF(tangent >= numElements, RuntimeError{"Invalid tangent index!"});
	NEX_ERR_RETURN_IF(bitangent >= numElements, RuntimeError{"Invalid bitangent index!"});

	for (auto uv : uvs) {
		NEX_ERR_RETURN_IF(uv >= numElements, RuntimeError{"Invalid UV index!"});
		NEX_ERR_RETURN_IF(elements[uv].numComponents != 2, RuntimeError{"UV does not have 2 components!"});
	}

	for (auto color : colors) {
		NEX_ERR_RETURN_IF(color >= numElements, RuntimeError{"Invalid color index!"});
		NEX_ERR_RETURN_IF(elements[color].numComponents != 4, RuntimeError{"Color does not have 4 components!"});
	}

	for (auto uvw : uvws) {
		NEX_ERR_RETURN_IF(uvw >= numElements, RuntimeError{"Invalid UVW index!"});
		NEX_ERR_RETURN_IF(elements[uvw].numComponents != 3, RuntimeError{"Color does not have 4 components!"});
	}

	return {};
}

Error VertexFormatInfo::AutoLayout() {
	bool useAutoStride = false;
    bool useAutoOffset = false;

    for (auto const& layoutElement : elements) {
        if (useAutoOffset) {
            NEX_ERR_RETURN_IF(layoutElement.relativeOffset != LAYOUT_ELEMENT_AUTO_OFFSET,
                RuntimeError{"All layout elements must have auto offset enabled if one does!"});
        } else {
            useAutoOffset = (layoutElement.relativeOffset == LAYOUT_ELEMENT_AUTO_OFFSET);
        }

        if (useAutoStride) {
            NEX_ERR_RETURN_IF(layoutElement.stride != LAYOUT_ELEMENT_AUTO_STRIDE,
                RuntimeError{"All layout elements must have auto offset enabled if one does!"});
        } else {
            useAutoStride = (layoutElement.stride == LAYOUT_ELEMENT_AUTO_STRIDE);
        }
    }

	NEX_ERR_RETURN_IF(useAutoOffset != useAutoStride, 
		RuntimeError{"Auto offset and auto stride must both be enabled if one is!"});
	
	int offset = 0;
	for (auto& layoutElement : elements) {
		if (useAutoOffset) {
			layoutElement.relativeOffset = offset;
		}
		offset += layoutElement.numComponents * GetSize(layoutElement.valueType);
	}
	if (useAutoStride) {
		for (auto& layoutElement : elements) {
			layoutElement.stride = offset;
		}
	}

	return {};
}