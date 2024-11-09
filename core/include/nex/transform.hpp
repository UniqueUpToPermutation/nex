#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace nex {
    struct Transform {
        glm::vec3 translation;
        float scale;
        glm::quat rotation;
    };
}