#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace hlsl {
    using float2 = glm::vec2;
    using float3 = glm::vec3;
    using float4 = glm::vec4;

    using float4x4 = glm::mat4x4;
    using float3x3 = glm::mat3x3;
    using float3x4 = glm::mat3x4;

    #include <common.hlsl>
}