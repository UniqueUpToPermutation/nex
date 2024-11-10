#pragma once

#include <cstdint>
#include <vector>

#include <nex/error.hpp>

namespace nex {
    enum class ValueType {
        UNDEFINED = 0,
        INT8,
        INT16,
        INT32,
        UINT8,
        UINT16,
        UINT32,
        FLOAT16,
        FLOAT32,
        NULL_T,
        NUM_TYPES
    };

    int GetSize(ValueType v);

    enum class InputElementFrequency {
        UNDEFINED = 0,
        PER_VERTEX,
        PER_INSTANCE,
        NUM_FREQUENCIES
    };

    static const uint32_t LAYOUT_ELEMENT_AUTO_OFFSET = 0xFFFFFFFF;
    static const uint32_t LAYOUT_ELEMENT_AUTO_STRIDE = 0xFFFFFFFF;

    struct LayoutElement {
        uint32_t inputIndex = 0;
        uint32_t bufferSlot = 0;
        uint32_t numComponents = 0;
        ValueType valueType = ValueType::FLOAT32;
        bool isNormalized = true;
        std::size_t relativeOffset = LAYOUT_ELEMENT_AUTO_OFFSET;
        uint32_t stride = LAYOUT_ELEMENT_AUTO_STRIDE;
        InputElementFrequency frequency = InputElementFrequency::PER_VERTEX;
        uint32_t instanceDataStepRate = 1;

		inline bool operator==(const LayoutElement& other) const {
			return inputIndex == other.inputIndex &&
				bufferSlot == other.bufferSlot &&
				numComponents == other.numComponents &&
				valueType == other.valueType &&
				isNormalized == other.isNormalized &&
				relativeOffset == other.relativeOffset &&
				stride == other.stride &&
				frequency == other.frequency &&
				instanceDataStepRate == other.instanceDataStepRate;
		}

        LayoutElement() = default;
        inline LayoutElement(
            uint32_t inputIndex,
            uint32_t blufferSlot,
            uint32_t numComponents,
            ValueType valueType,
            bool isNormalized,
            uint32_t relativeOffset,
            uint32_t stride,
            InputElementFrequency frequency,
            uint32_t instanceDataStepRate) :
            inputIndex(inputIndex),
            bufferSlot(blufferSlot),
            numComponents(numComponents),
            valueType(valueType),
            isNormalized(isNormalized),
            relativeOffset(relativeOffset),
            stride(stride),
            frequency(frequency),
            instanceDataStepRate(instanceDataStepRate) {
        }

        inline LayoutElement(
            uint32_t inputIndex,
            uint32_t blufferSlot,
            uint32_t numComponents,
            ValueType valueType) :
            inputIndex(inputIndex),
            bufferSlot(blufferSlot),
            numComponents(numComponents),
            valueType(valueType) {
        }

        inline LayoutElement(
            uint32_t inputIndex,
            uint32_t blufferSlot,
            uint32_t numComponents,
            ValueType valueType,
            bool isNormalized,
            InputElementFrequency frequency) :
            inputIndex(inputIndex),
            bufferSlot(blufferSlot),
            numComponents(numComponents),
            valueType(valueType),
            isNormalized(isNormalized),
            frequency(frequency) {
        }

        template <class Archive>
		void serialize(Archive& archive) {
            archive(inputIndex);
            archive(bufferSlot);
            archive(numComponents);
            archive(valueType);
            archive(isNormalized);
            archive(relativeOffset);
            archive(stride);
            archive(frequency);
            archive(instanceDataStepRate);
        }
    };

    enum class Topology {
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP
    };

    enum class VertexFormat {
        Position,
        PositionColor,
        PositionUV,
        PositionUVNormal,
        PositionUVNormalTangent,
        PositionUVNormalTangentBitangent,
        Custom
    };

    struct VertexFormatInfo {
	public:
		std::vector<LayoutElement> elements;

		int position = -1;
		int normal = -1;
		int tangent = -1;
		int bitangent = -1;

		std::vector<int> uvs;
        std::vector<int> uvws;
		std::vector<int> colors;

        VertexFormat formatTag;

		inline bool operator==(const VertexFormatInfo& other) const {
            if (other.formatTag != formatTag) {
                return false;
            } else if (formatTag == VertexFormat::Custom) {
                return elements == other.elements &&
                    position == other.position &&
                    normal == other.normal &&
                    bitangent == other.bitangent &&
                    uvs == other.uvs &&
                    uvws == other.uvws &&
                    colors == other.colors;
            } else {
                return true;
            }
		}
		
        static Expected<VertexFormatInfo> From(VertexFormat format);
		static VertexFormatInfo PositionUVNormalTangent();
		static VertexFormatInfo PositionUVNormal();
		static VertexFormatInfo PositionUVNormalTangentBitangent();
        static VertexFormatInfo PositionUV();
        static VertexFormatInfo Position();
        static VertexFormatInfo PositionColor();

		template <class Archive>
		void serialize(Archive& archive) {
			archive(elements);

			archive(position);
			archive(normal);
			archive(tangent);
			archive(bitangent);

			archive(uvs);
            archive(uvws);
			archive(colors);
		}

        Error AutoLayout();
        Error CheckValid() const;
	};
}