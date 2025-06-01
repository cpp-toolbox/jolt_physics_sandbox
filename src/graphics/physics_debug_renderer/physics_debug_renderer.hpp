#ifndef PHYSICS_DEBUG_RENDERER_HPP
#define PHYSICS_DEBUG_RENDERER_HPP

#include <iostream>
#define JPH_DEBUG_RENDERER
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <glm/glm.hpp>
#include <vector>

#include "sbpt_generated_includes.hpp"

class PhysicsDebugRenderer : public JPH::DebugRendererSimple {
  private:
    CW_V_TransformationWithColoredVertexShaderRenderer &renderer;

    glm::vec3 colorToVec3(JPH::ColorArg color) {
        return glm::vec3(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);
    }

    glm::vec3 rvec3ToVec3(JPH::RVec3Arg vec) {
        return glm::vec3(static_cast<float>(vec.GetX()), static_cast<float>(vec.GetY()),
                         static_cast<float>(vec.GetZ()));
    }

  public:
    explicit PhysicsDebugRenderer(CW_V_TransformationWithColoredVertexShaderRenderer &renderer) : renderer(renderer) {
        std::cout << "in constructor" << std::endl;
    }

    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override {
        std::cout << "about to draw line" << std::endl;

        glm::vec3 from = rvec3ToVec3(inFrom);
        glm::vec3 to = rvec3ToVec3(inTo);

        glm::vec3 direction = glm::normalize(to - from);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        if (glm::abs(glm::dot(direction, up)) > 0.99f) {
            up = glm::vec3(1.0f, 0.0f, 0.0f);
        }

        glm::vec3 perpendicular = glm::normalize(glm::cross(direction, up));
        float line_width = 0.01f; // thin line width
        glm::vec3 offset = perpendicular * (line_width * 0.5f);

        std::vector<glm::vec3> positions = {
            from - offset, // bottom left
            from + offset, // bottom right
            to + offset,   // top right
            to - offset    // top left
        };

        glm::vec3 color = colorToVec3(inColor);
        std::vector<glm::vec3> colors = {color, color, color, color};

        // two triangles: (0,1,2) and (0,2,3)
        std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

        renderer.draw(indices, positions, colors);
    }

    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor,
                              ECastShadow inCastShadow) override {
        std::cout << "about to draw triangle" << std::endl;

        std::vector<glm::vec3> positions = {rvec3ToVec3(inV1), rvec3ToVec3(inV2), rvec3ToVec3(inV3)};

        glm::vec3 color = colorToVec3(inColor);
        std::vector<glm::vec3> colors = {color, color, color};

        std::vector<unsigned int> indices = {0, 1, 2};

        renderer.draw(indices, positions, colors);
    }

    virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString, JPH::ColorArg inColor,
                            float inHeight) override {

        std::cout << "about to draw text" << std::endl;
        // Not implemented as requested
    }
};

#endif // PHYSICS_DEBUG_RENDERER_HPP
