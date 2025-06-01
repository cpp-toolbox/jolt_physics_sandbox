#ifndef RENDERER_HPP
#define RENDERER_HPP

// A renderer is simply a thing that lets you draw stuff for particular shaders easily without having to do the opengl
// rigamaroll

#include <vector>
#include <glm/glm.hpp>
#include "sbpt_generated_includes.hpp"

// CW_V_TRANSFORMATION_WITH_COLORED_VERTEX
class CW_V_TransformationWithColoredVertexShaderRenderer {
  private:
    ShaderCache &shader_cache;

    // OpenGL buffer objects
    GLuint vertex_attribute_object;
    GLuint positions_buffer_object;
    GLuint rgb_colors_buffer_object;

    GLuint indices_buffer_object;

  public:
    explicit CW_V_TransformationWithColoredVertexShaderRenderer(ShaderCache &shader_cache);
    ~CW_V_TransformationWithColoredVertexShaderRenderer();

    // Immediate draw function - uploads data and draws immediately
    void draw(const std::vector<unsigned int> &indices, const std::vector<glm::vec3> &positions,
              const std::vector<glm::vec3> &rgb_colors);

    // Delete copy constructor and assignment operator to prevent copying
    CW_V_TransformationWithColoredVertexShaderRenderer(const CW_V_TransformationWithColoredVertexShaderRenderer &) =
        delete;
    CW_V_TransformationWithColoredVertexShaderRenderer &
    operator=(const CW_V_TransformationWithColoredVertexShaderRenderer &) = delete;
};

#endif // RENDERER_HPP
