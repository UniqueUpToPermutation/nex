#pragma once

#include <variant>

#include <nex/hlsl.hpp>

#include <glm/ext/scalar_constants.hpp>

namespace nex {

    struct OrthoProjection {
        float width = 2.0f;
        float height = 2.0f;

        glm::mat4x4 AsMatrix(float near, float far) const;
    };

    struct PerspectiveProjection {
        float aspectRatio = 1.0f;
        float fieldOfView = glm::pi<float>() / 2.0f;

        glm::mat4x4 AsMatrix(float near, float far) const;
    };

    struct CameraProjection {
        std::variant<OrthoProjection, PerspectiveProjection> details = OrthoProjection{};
        float near = -1.0f;
        float far = 1.0f;

        glm::mat4x4 AsMatrix() const;
    };

    struct CameraView {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 lookAt{0.0f, 0.0f, 1.0f};
        glm::vec3 up{0.0f, 1.0f, 0.0f};

        glm::mat4x4 AsMatrix() const;
    };

    struct Camera {
        CameraView view;
        CameraProjection projection;

        hlsl::CameraAttribs AsCameraAttribs(glm::vec2 viewport) const;

        inline hlsl::CameraAttribs AsCameraAttribs(float viewportWidth, float viewportHeight) const {
            return AsCameraAttribs(glm::vec2{viewportWidth, viewportHeight});
        }
    };
}