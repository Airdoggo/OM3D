#ifndef SCENE_H
#define SCENE_H

#include <SceneObject.h>
#include <PointLight.h>
#include <Camera.h>
#include <OcclusionQuery.h>


#include <vector>
#pragma warning (disable : 4005)
#include <memory>

namespace OM3D {

class Scene : NonMovable {

    public:
        Scene();

        static Result<std::unique_ptr<Scene>> from_gltf(const std::string& file_name);

        void render(const Camera& camera);
        void compute_lights(const Camera& camera, const Material &sun_light_material, const Material &point_light_material) const;
        void compute_occlusion_query(const Camera& camera);

        void add_object(SceneObject obj);
        void add_object(PointLight obj);

        void sort_front_to_back(const glm::vec3& camera_pos);

    private:
        struct front_to_back
        {
        public:
            front_to_back(const glm::vec3& cam_pos) : camera_pos(cam_pos) {}

            bool operator()(const SceneObject& a, const SceneObject& b) const
            {
                return glm::distance(camera_pos, glm::vec3(a.transform()[3])) <
                    glm::distance(camera_pos, glm::vec3(b.transform()[3]));
            }
        private:
            glm::vec3 camera_pos;
        };

        std::vector<std::vector<SceneObject>> _objects;
        std::vector<PointLight> _point_lights;
        glm::vec3 _sun_direction = glm::vec3(0.2f, 1.0f, 0.1f);
        OcclusionQuery occlusion_query;

};

}

#endif // SCENE_H
