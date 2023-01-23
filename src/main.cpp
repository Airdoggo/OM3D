#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <graphics.h>
#include <SceneView.h>
#include <Texture.h>
#include <Framebuffer.h>
#include <ImGuiRenderer.h>

#include <imgui/imgui.h>

#include <iomanip>
#include <sstream>

using namespace OM3D;

static float delta_time = 0.0f;
const glm::uvec2 window_size(1600, 900);

static std::vector<float> time_buffer(10, 0.02);
static size_t time_index = 0;

void glfw_check(bool cond) {
    if(!cond) {
        const char* err = nullptr;
        glfwGetError(&err);
        std::cerr << "GLFW error: " << err << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void update_delta_time() {
    static double time = 0.0;
    const double new_time = program_time();
    delta_time = float(new_time - time);
    time = new_time;

    time_buffer[time_index++] = delta_time;
    time_index = time_index % time_buffer.size();
}

void update_fps(GLFWwindow* window) {
    float total_time = 0.0;
    
    for (auto t : time_buffer)
        total_time += t;
    
    float fps = time_buffer.size() / total_time; 

    std::stringstream title;
    title << "TP window - " << std::fixed << std::setprecision(2) << fps << " FPS";

    glfwSetWindowTitle(window, title.str().c_str());
}

void process_inputs(GLFWwindow* window, Camera& camera) {
    static glm::dvec2 mouse_pos;

    glm::dvec2 new_mouse_pos;
    glfwGetCursorPos(window, &new_mouse_pos.x, &new_mouse_pos.y);

    {
        glm::vec3 movement = {};
        if(glfwGetKey(window, 'W') == GLFW_PRESS) {
            movement += camera.forward();
        }
        if(glfwGetKey(window, 'S') == GLFW_PRESS) {
            movement -= camera.forward();
        }
        if(glfwGetKey(window, 'D') == GLFW_PRESS) {
            movement += camera.right();
        }
        if(glfwGetKey(window, 'A') == GLFW_PRESS) {
            movement -= camera.right();
        }

        float speed = 10.0f;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 10.0f;
        }

        if(movement.length() > 0.0f) {
            const glm::vec3 new_pos = camera.position() + movement * delta_time * speed;
            camera.set_view(glm::lookAt(new_pos, new_pos + camera.forward(), camera.up()));
        }
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        const glm::vec2 delta = glm::vec2(mouse_pos - new_mouse_pos) * 0.01f;
        if(delta.length() > 0.0f) {
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), delta.x, glm::vec3(0.0f, 1.0f, 0.0f));
            rot = glm::rotate(rot, delta.y, camera.right());
            camera.set_view(glm::lookAt(camera.position(), camera.position() + (glm::mat3(rot) * camera.forward()), (glm::mat3(rot) * camera.up())));
        }
    }

    mouse_pos = new_mouse_pos;
}


std::unique_ptr<Scene> create_default_scene() {
    auto scene = std::make_unique<Scene>();

    // Load default cube model
    auto result = Scene::from_gltf(std::string(data_path) + "forest_huge.glb");
    ALWAYS_ASSERT(result.is_ok, "Unable to load default scene");
    scene = std::move(result.value);

    // Add lights
    {
        PointLight light;
        light.set_position(glm::vec3(1.0f, 2.0f, 4.0f));
        light.set_color(glm::vec3(0.0f, 10.0f, 0.0f));
        light.set_radius(100.0f);
        scene->add_object(std::move(light));
    }
    {
        PointLight light;
        light.set_position(glm::vec3(1.0f, 2.0f, -4.0f));
        light.set_color(glm::vec3(10.0f, 0.0f, 0.0f));
        light.set_radius(50.0f);
        scene->add_object(std::move(light));
    }
    {
        PointLight light;
        light.set_position(glm::vec3(1.0f, 2.0f, -4.0f));
        light.set_color(glm::vec3(0.0f, 0.0f, 10.0f));
        light.set_radius(50.0f);
        scene->add_object(std::move(light));
    }

    scene->init_light_buffer();

    return scene;
}

int main(int, char**) {
    DEBUG_ASSERT([] { std::cout << "Debug asserts enabled" << std::endl; return true; }());

    glfw_check(glfwInit());
    DEFER(glfwTerminate());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(window_size.x, window_size.y, "TP window", nullptr, nullptr);
    glfw_check(window);
    DEFER(glfwDestroyWindow(window));

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    init_graphics();

    ImGuiRenderer imgui(window);

    std::unique_ptr<Scene> scene = create_default_scene();
    SceneView scene_view(scene.get());

    auto shading_program = Program::from_file("shading.comp", {"NB_LIGHTS " + std::to_string(scene->get_nb_lights())});
    auto tonemap_program = Program::from_file("tonemap.comp");

    Texture albedo(window_size, ImageFormat::RGBA8_UNORM);
	Texture normals(window_size, ImageFormat::RGBA8_UNORM);
	Texture depth(window_size, ImageFormat::Depth32_FLOAT);
    Texture lit(window_size, ImageFormat::RGBA16_FLOAT);
    Texture color(window_size, ImageFormat::RGBA8_UNORM);

    Framebuffer tonemap_framebuffer(nullptr, std::array{&color});
    Framebuffer g_buffer(&depth, std::array{&albedo, &normals});
    Framebuffer shading_buffer(&depth, std::array{&lit});

    Material debug_material = Material::debug_material();

    shading_program->set_uniform(HASH("screen_size"), window_size);

    for(;;) {
        glfwPollEvents();
        if(glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            break;
        }

        update_delta_time();
        update_fps(window);

        if(const auto& io = ImGui::GetIO(); !io.WantCaptureMouse && !io.WantCaptureKeyboard) {
            process_inputs(window, scene_view.camera());
        }

        // Update the frame data
        scene_view.update_frame();

        // Render the scene
        {
            g_buffer.bind();
            scene_view.render();
        }

        // Set the textures as input
        shading_buffer.bind(true, false); // Clear color but not Z buffer
        albedo.bind(0);
        normals.bind(1);
        depth.bind(2);

        if (imgui.debug_mode) // Debug view
        {
            debug_material.bind();

            if (imgui.debug_mode == 3) // Allow background fragments to be modified for Depth view
                glDepthFunc(GL_LEQUAL);

            debug_material.set_uniform(HASH("debug"), imgui.debug_mode);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        else { // Screen-space light calculations
            shading_program->bind();
            lit.bind_as_image(3, AccessType::WriteOnly);
            shading_program->set_uniform(HASH("inv_viewproj"), glm::inverse(scene_view.camera().view_proj_matrix()));
            shading_program->set_uniform(HASH("proj_mat"), scene_view.camera().projection_matrix());
            shading_program->set_uniform(HASH("view_mat"), scene_view.camera().view_matrix());
            scene->bind_buffers();
            
            glDispatchCompute(align_up_to(window_size.x, 16) / 16, align_up_to(window_size.y, 16) / 16, 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
        }

        // Apply a tonemap in compute shader
        {
            tonemap_program->bind();
            lit.bind(0);
            color.bind_as_image(1, AccessType::WriteOnly);
            glDispatchCompute(align_up_to(window_size.x, 8) / 8, align_up_to(window_size.y, 8) / 8, 1);
        }
        // Blit tonemap result to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        tonemap_framebuffer.blit();

        // GUI
        imgui.start();
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

            char buffer[1024] = {};
            if(ImGui::InputText("Load scene", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                auto result = Scene::from_gltf(buffer);
                if(!result.is_ok) {
                    std::cerr << "Unable to load scene (" << buffer << ")" << std::endl;
                } else {
                    scene = std::move(result.value);
                    scene_view = SceneView(scene.get());
                    shading_program = Program::from_file("shading.comp", {"NB_LIGHTS " + std::to_string(scene->get_nb_lights())});
                }
            }

            imgui.display_debug_mode();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::SliderInt("BVH Subdivisions", &imgui.bvh_subdivisions, 1, 10)) {
                scene->create_bounding_volume_hierarchy(imgui.bvh_subdivisions);
            }

            const RenderInfo &info = scene->get_render_info();
            ImGui::Text("Number of objects: %i\nNumber of culled objects: %i\nNumber of checks: %i",
                        info.objects, info.objects - info.rendered, info.checks);
        }
        imgui.finish();

        glfwSwapBuffers(window);
    }

    scene = nullptr; // destroy scene and child OpenGL objects
}
