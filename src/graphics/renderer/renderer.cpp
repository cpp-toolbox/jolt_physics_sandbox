#include "renderer.hpp"
#include <iostream>

CW_V_TransformationWithColoredVertexShaderRenderer::CW_V_TransformationWithColoredVertexShaderRenderer(
    ShaderCache &shader_cache)
    : shader_cache(shader_cache) {
    std::cout << "renderer constructor" << std::endl;
    glGenVertexArrays(1, &vertex_attribute_object);
    glBindVertexArray(vertex_attribute_object);
    // Create buffer objects
    glGenBuffers(1, &indices_buffer_object);
    glGenBuffers(1, &positions_buffer_object);
    glGenBuffers(1, &rgb_colors_buffer_object);
    // Reserve space for reasonable batch sizes
    const size_t initial_buffer_size = 100000;
    // Setup positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, positions_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, initial_buffer_size * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    shader_cache.configure_vertex_attributes_for_drawables_vao(vertex_attribute_object, positions_buffer_object,
                                                               ShaderType::CW_V_TRANSFORMATION_WITH_COLORED_VERTEX,
                                                               ShaderVertexAttributeVariable::XYZ_POSITION);
    // Setup colors buffer
    glBindBuffer(GL_ARRAY_BUFFER, rgb_colors_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, initial_buffer_size * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    shader_cache.configure_vertex_attributes_for_drawables_vao(vertex_attribute_object, rgb_colors_buffer_object,
                                                               ShaderType::CW_V_TRANSFORMATION_WITH_COLORED_VERTEX,
                                                               ShaderVertexAttributeVariable::PASSTHROUGH_RGB_COLOR);
    // Setup indices buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, initial_buffer_size * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}

CW_V_TransformationWithColoredVertexShaderRenderer::~CW_V_TransformationWithColoredVertexShaderRenderer() {
    glDeleteVertexArrays(1, &vertex_attribute_object);
    glDeleteBuffers(1, &indices_buffer_object);
    glDeleteBuffers(1, &positions_buffer_object);
    glDeleteBuffers(1, &rgb_colors_buffer_object);
}

void CW_V_TransformationWithColoredVertexShaderRenderer::draw_immediate(const std::vector<unsigned int> &indices,
                                                                        const std::vector<glm::vec3> &positions,
                                                                        const std::vector<glm::vec3> &rgb_colors) {
    // Validate input data
    if (positions.empty() || indices.empty()) {
        return;
    }
    if (positions.size() != rgb_colors.size()) {
        throw std::runtime_error("Vertex attribute arrays must have the same size");
    }
    // Use the shader program
    shader_cache.use_shader_program(ShaderType::CW_V_TRANSFORMATION_WITH_COLORED_VERTEX);
    glBindVertexArray(vertex_attribute_object);
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, positions_buffer_object);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), positions.data());
    glBindBuffer(GL_ARRAY_BUFFER, rgb_colors_buffer_object);
    glBufferSubData(GL_ARRAY_BUFFER, 0, rgb_colors.size() * sizeof(glm::vec3), rgb_colors.data());
    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_object);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());
    // Draw
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Cleanup
    glBindVertexArray(0);
    shader_cache.stop_using_shader_program();
}

void CW_V_TransformationWithColoredVertexShaderRenderer::queue_draw(const std::vector<unsigned int> &indices,
                                                                    const std::vector<glm::vec3> &positions,
                                                                    const std::vector<glm::vec3> &rgb_colors) {
    // Validate input data
    if (positions.empty() || indices.empty()) {
        return;
    }
    if (positions.size() != rgb_colors.size()) {
        throw std::runtime_error("Vertex attribute arrays must have the same size");
    }

    // Calculate the current vertex offset
    unsigned int vertex_offset = static_cast<unsigned int>(batched_positions.size());

    // Add positions and colors to batched data
    batched_positions.insert(batched_positions.end(), positions.begin(), positions.end());
    batched_rgb_colors.insert(batched_rgb_colors.end(), rgb_colors.begin(), rgb_colors.end());

    // Add indices with offset applied
    for (unsigned int index : indices) {
        batched_indices.push_back(index + vertex_offset);
    }
}

void CW_V_TransformationWithColoredVertexShaderRenderer::draw_everything() {
    if (!batched_indices.empty()) {
        // Draw all batched data at once
        draw_immediate(batched_indices, batched_positions, batched_rgb_colors);

        // Clear all batched data for next iteration
        batched_positions.clear();
        batched_rgb_colors.clear();
        batched_indices.clear();
    }
}
