#ifndef SCENE_H
#define SCENE_H

#include <SceneObject.h>
#include <PointLight.h>
#include <Camera.h>
#include <shader_structs.h>

#include <vector>
#include <memory>

namespace OM3D {

struct RenderInfo {
    size_t objects = 0;
    size_t rendered = 0;
};

class Scene : NonMovable {

    public:
        Scene();
        Scene(SceneObject &light_volume);

        static Result<std::unique_ptr<Scene>> from_gltf(const std::string& file_name, const std::string& light_file_name);

        void update_frame(const Camera& camera);

        void render(const Camera& camera);
        void compute_lights(const Camera& camera) const;

        void add_object(SceneObject obj);
        void add_object(PointLight obj);

        void set_screen_size_uniform(glm::uvec2 window_size);

        const RenderInfo &get_render_info() const;

    private:
        std::vector<std::vector<SceneObject>> _objects;
        std::vector<PointLight> _point_lights;
        SceneObject _light_volume;
        Material _sun_material = Material::sun_light_material();
        glm::vec3 _sun_direction = glm::vec3(0.2f, 1.0f, 0.1f);

        // Updated each frame
        TypedBuffer<shader::FrameData> _buffer = TypedBuffer<shader::FrameData>(nullptr, 1);
        Frustum _frustum;
        glm::vec3 _camera_position;
        RenderInfo _render_info;
};

}

#endif // SCENE_H
