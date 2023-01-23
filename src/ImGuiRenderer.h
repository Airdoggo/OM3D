#ifndef IMGUIRENDERER_H
#define IMGUIRENDERER_H

#include <Material.h>

#include <chrono>

struct ImDrawData;
struct GLFWwindow;

namespace OM3D {

enum DebugView {
    None,
    Albedo,
    Normals,
    Depth,
    AABB,
    Tiles,

    DebugView_Size,
};

class ImGuiRenderer : NonMovable {
    public:
        ImGuiRenderer(GLFWwindow* window);

        void start();
        void finish();

        void display_debug_mode();

        const char *debug_views[6] = { "No debug", "Albedo", "Normals", "Depth", "BVH Hierarchy", "Tiles" };
        uint32_t debug_mode = 0;
        int bvh_subdivisions = 4;
        int aabb_render_level = 0;

    private:
        void render(const ImDrawData* draw_data);
        float update_delta_time();

        GLFWwindow* _window = nullptr;

        Material _material;
        std::unique_ptr<Texture> _font;
        std::chrono::time_point<std::chrono::high_resolution_clock> _last;
};

}

#endif // IMGUIRENDERER_H
