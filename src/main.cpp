#include "graphics/physics_debug_renderer/physics_debug_renderer.hpp"
#include "graphics/renderer/renderer.hpp"
#include "utility/texture_packer_model_loading/texture_packer_model_loading.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "graphics/draw_info/draw_info.hpp"
#include "input/glfw_lambda_callback_manager/glfw_lambda_callback_manager.hpp"
#include "input/input_state/input_state.hpp"

#include "utility/fixed_frequency_loop/fixed_frequency_loop.hpp"

#define JPH_DEBUG_RENDERER
#include "physics/physics.hpp"

#include "graphics/vertex_geometry/vertex_geometry.hpp"
#include "graphics/shader_standard/shader_standard.hpp"
#include "graphics/texture_packer/texture_packer.hpp"
#include "graphics/batcher/generated/batcher.hpp"
#include "graphics/shader_cache/shader_cache.hpp"
#include "graphics/fps_camera/fps_camera.hpp"
#include "graphics/window/window.hpp"
#include "graphics/colors/colors.hpp"

#include "utility/resource_path/resource_path.hpp"
#include "utility/unique_id_generator/unique_id_generator.hpp"
#include "utility/jolt_glm_type_conversions/jolt_glm_type_conversions.hpp"

#include <iostream>
#include <filesystem>

int main() {

    std::cout << "1" << std::endl;
    Colors colors;

    FPSCamera fps_camera;

    unsigned int window_width_px = 700, window_height_px = 700;
    bool start_in_fullscreen = false;
    bool start_with_mouse_captured = true;
    bool vsync = false;

    Window window;
    window.initialize_glfw_glad_and_return_window(window_width_px, window_height_px, "catmullrom camera interpolation",
                                                  start_in_fullscreen, start_with_mouse_captured, vsync);

    const auto textures_directory = std::filesystem::path("assets");
    std::filesystem::path output_dir = std::filesystem::path("assets") / "packed_textures";
    int container_side_length = 1024;

    InputState input_state;

    std::vector<ShaderType> requested_shaders = {ShaderType::TEXTURE_PACKER_CWL_V_TRANSFORMATION_UBOS_1024,
                                                 ShaderType::CWL_V_TRANSFORMATION_UBOS_1024_WITH_SOLID_COLOR,
                                                 ShaderType::CW_V_TRANSFORMATION_WITH_COLORED_VERTEX};

    ShaderCache shader_cache(requested_shaders);
    Batcher batcher(shader_cache);
    CW_V_TransformationWithColoredVertexShaderRenderer ctwcvsr(shader_cache);

    // https://github.com/jrouwe/JoltPhysics/discussions/964
    // PhysicsDebugRenderer physics_debug_renderer(ctwcvsr);

    TexturePacker texture_packer(textures_directory, output_dir, container_side_length);
    shader_cache.set_uniform(ShaderType::TEXTURE_PACKER_CWL_V_TRANSFORMATION_UBOS_1024,
                             ShaderUniformVariable::PACKED_TEXTURE_BOUNDING_BOXES, 1);

    Physics physics;

    ResourcePath rp(false);

    auto map = texture_packer_model_loading::parse_model_into_tig(
        rp, "assets/map.obj", texture_packer,
        batcher.texture_packer_cwl_v_transformation_ubos_1024_shader_batcher.object_id_generator,
        batcher.texture_packer_cwl_v_transformation_ubos_1024_shader_batcher.ltw_object_id_generator);

    physics.load_model_into_physics_world(draw_info::extract_indexed_vertex_positions_vector(map.ivptps));

    auto physics_character = physics.create_character(0, JPH::Vec3(0, 10, 0));

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
    shader_cache.set_uniform(ShaderType::CWL_V_TRANSFORMATION_UBOS_1024_WITH_SOLID_COLOR,
                             ShaderUniformVariable::RGBA_COLOR, glm::vec4(colors.cyan, 1));

    std::function<void(unsigned int)> char_callback = [](unsigned int codepoint) {};
    std::function<void(int, int, int, int)> key_callback = [&](int key, int scancode, int action, int mods) {
        input_state.glfw_key_callback(key, scancode, action, mods);
    };
    std::function<void(double, double)> mouse_pos_callback = [&](double xpos, double ypos) {
        fps_camera.mouse_callback(xpos, ypos);
    };
    std::function<void(int, int, int)> mouse_button_callback = [&](int button, int action, int mods) {
        input_state.glfw_mouse_button_callback(button, action, mods);
    };
    std::function<void(int, int)> frame_buffer_size_callback = [](int width, int height) {};
    GLFWLambdaCallbackManager glcm(window.glfw_window, char_callback, key_callback, mouse_pos_callback,
                                   mouse_button_callback, frame_buffer_size_callback);

    auto ball = vertex_geometry::generate_icosphere(3, 1);
    Transform ball_transform;

    glm::mat4 identity = glm::mat4(1);

    shader_cache.set_uniform(ShaderType::CWL_V_TRANSFORMATION_UBOS_1024_WITH_SOLID_COLOR,
                             ShaderUniformVariable::CAMERA_TO_CLIP,
                             fps_camera.get_projection_matrix(window_width_px, window_height_px));

    shader_cache.set_uniform(ShaderType::CW_V_TRANSFORMATION_WITH_COLORED_VERTEX, ShaderUniformVariable::CAMERA_TO_CLIP,
                             fps_camera.get_projection_matrix(window_width_px, window_height_px));

    shader_cache.set_uniform(ShaderType::TEXTURE_PACKER_CWL_V_TRANSFORMATION_UBOS_1024,
                             ShaderUniformVariable::CAMERA_TO_CLIP,
                             fps_camera.get_projection_matrix(window_width_px, window_height_px));

    std::function<void(double)> tick = [&](double dt) {
        /*glfwGetFramebufferSize(window, &width, &height);*/

        glViewport(0, 0, window_width_px, window_width_px);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fps_camera.process_input(input_state.is_pressed(EKey::LEFT_CONTROL), input_state.is_pressed(EKey::TAB),
                                 input_state.is_pressed(EKey::w), input_state.is_pressed(EKey::a),
                                 input_state.is_pressed(EKey::s), input_state.is_pressed(EKey::d),
                                 input_state.is_pressed(EKey::SPACE), input_state.is_pressed(EKey::LEFT_SHIFT), dt);

        shader_cache.set_uniform(ShaderType::CWL_V_TRANSFORMATION_UBOS_1024_WITH_SOLID_COLOR,
                                 ShaderUniformVariable::WORLD_TO_CAMERA, fps_camera.get_view_matrix());

        shader_cache.set_uniform(ShaderType::CW_V_TRANSFORMATION_WITH_COLORED_VERTEX,
                                 ShaderUniformVariable::WORLD_TO_CAMERA, fps_camera.get_view_matrix());

        shader_cache.set_uniform(ShaderType::TEXTURE_PACKER_CWL_V_TRANSFORMATION_UBOS_1024,
                                 ShaderUniformVariable::WORLD_TO_CAMERA, fps_camera.get_view_matrix());

        std::vector<unsigned int> ltw_indices(ball.xyz_positions.size(), 0);

        batcher.cwl_v_transformation_ubos_1024_with_solid_color_shader_batcher.queue_draw(
            0, ball.indices, ball.xyz_positions, ltw_indices);

        batcher.cwl_v_transformation_ubos_1024_with_solid_color_shader_batcher.draw_everything();
        batcher.cwl_v_transformation_ubos_1024_with_solid_color_shader_batcher.upload_ltw_matrices();

        physics.update_characters_only(dt);

        batcher.cw_v_transformation_with_colored_vertex_shader_batcher.draw_everything();

        // fps_camera.transform.set_translation(j2g(physics_character->GetPosition()));

        batcher.texture_packer_cwl_v_transformation_ubos_1024_shader_batcher.queue_draw(map);

        batcher.texture_packer_cwl_v_transformation_ubos_1024_shader_batcher.upload_ltw_matrices();
        batcher.texture_packer_cwl_v_transformation_ubos_1024_shader_batcher.draw_everything();

        JPH::BodyManager::DrawSettings draw_settings;
        // physics.physics_system.DrawBodies(draw_settings, &physics_debug_renderer);

        glfwSwapBuffers(window.glfw_window);
        glfwPollEvents();

        TemporalBinarySignal::process_all();
    };

    std::function<bool()> termination = [&]() { return glfwWindowShouldClose(window.glfw_window); };

    FixedFrequencyLoop ffl;
    ffl.start(120, tick, termination);

    return 0;
}
