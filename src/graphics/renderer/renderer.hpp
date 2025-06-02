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

    // Batched rendering data
    std::vector<glm::vec3> batched_positions;
    std::vector<glm::vec3> batched_rgb_colors;
    std::vector<unsigned int> batched_indices;

  public:
    explicit CW_V_TransformationWithColoredVertexShaderRenderer(ShaderCache &shader_cache);
    ~CW_V_TransformationWithColoredVertexShaderRenderer();

    // Immediate draw function - uploads data and draws immediately
    void draw_immediate(const std::vector<unsigned int> &indices, const std::vector<glm::vec3> &positions,
                        const std::vector<glm::vec3> &rgb_colors);

    // Queue draw function - adds data to batched buffers with proper index offsetting
    void queue_draw(const std::vector<unsigned int> &indices, const std::vector<glm::vec3> &positions,
                    const std::vector<glm::vec3> &rgb_colors);

    // Draw everything that has been queued and clear the buffers
    void draw_everything();

    // Optional: Get stats about queued data
    size_t get_queued_vertex_count() const { return batched_positions.size(); }
    size_t get_queued_index_count() const { return batched_indices.size(); }

    // Delete copy constructor and assignment operator to prevent copying
    CW_V_TransformationWithColoredVertexShaderRenderer(const CW_V_TransformationWithColoredVertexShaderRenderer &) =
        delete;
    CW_V_TransformationWithColoredVertexShaderRenderer &
    operator=(const CW_V_TransformationWithColoredVertexShaderRenderer &) = delete;
};

#endif // RENDERER_HPP
