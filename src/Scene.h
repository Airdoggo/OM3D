#ifndef SCENE_H
#define SCENE_H

#include <SceneObject.h>
#include <PointLight.h>
#include <Camera.h>
#include <shader_structs.h>
#include <BoundingTree.h>

#include <vector>
#include <memory>

namespace OM3D {

struct RenderInfo {
    size_t objects = 0;
    size_t rendered = 0;
    size_t checks = 0;
};

class Scene : NonMovable {

    public:
        Scene();

        static Result<std::unique_ptr<Scene>> from_gltf(const std::string& file_name);

        void create_bounding_volume_hierarchy(size_t subdivisions = 4);
        
        void init_light_buffer();
        void update_frame(const Camera& camera);
        void bind_buffers() const;

        void render(const Camera& camera) ;

        void add_object(SceneObject obj, std::shared_ptr<SceneObject> *object = nullptr);
        void add_object(PointLight obj);

        void dynamic_add_object(SceneObject obj, size_t subdivisions = 4);
        void dynamic_remove_object(const std::shared_ptr<SceneObject> &object);

        const RenderInfo &get_render_info() const;
        const size_t get_nb_lights() const;

    private:
        std::vector<std::vector<std::shared_ptr<SceneObject>>> _objects;
        std::vector<PointLight> _point_lights;
        TypedBuffer<shader::PointLight> _light_buffer;
        glm::vec3 _sun_direction = glm::vec3(0.2f, 1.0f, 0.1f);

        BoundingTree _bounding_tree;
        size_t _nb_different_objects = 0;

        // Updated each frame
        TypedBuffer<shader::FrameData> _buffer = TypedBuffer<shader::FrameData>(nullptr, 1);
        Frustum _frustum;
        RenderInfo _render_info;
};

}

#endif // SCENE_H
