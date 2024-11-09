#include <nex/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace nex;

glm::mat4x4 OrthoProjection::AsMatrix(float near, float far) const {
    return glm::ortho(width / 2.0f, -width / 2.0f, -height / 2.0f, height / 2.0f, near, far);
}

glm::mat4x4 PerspectiveProjection::AsMatrix(float near, float far) const {
    return glm::perspective(fieldOfView, aspectRatio, near, far);
}

glm::mat4x4 CameraProjection::AsMatrix() const {
    return std::visit([this](auto const& impl) {
        return impl.AsMatrix(near, far);
    }, details);
}

glm::mat4x4 CameraView::AsMatrix() const {
    return glm::lookAt(position, lookAt, up);
}

hlsl::CameraAttribs Camera::AsCameraAttribs(glm::vec2 viewport) const {
    auto viewMat = view.AsMatrix();
    auto projMat = projection.AsMatrix();
    auto viewProjMat = projMat * viewMat;

    return hlsl::CameraAttribs{
        .mView = viewMat,
        .mViewProj = viewProjMat,
        .mProj = projMat,
        .mInvView = glm::inverse(viewMat),
        .mInvViewProj = glm::inverse(viewProjMat),
        .mInvProj = glm::inverse(projMat),
        .mViewport = viewport,
        .mNearZ = projection.near,
        .mFarZ = projection.far,
        .mPosition = view.position,
        .mViewDirection = glm::normalize(view.lookAt - view.position)
    };
}