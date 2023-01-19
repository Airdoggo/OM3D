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

        void render(const Camera& camera) const;
        void compute_lights(const Camera& camera, const Material &sun_light_material, const Material &point_light_material) const;

        void add_object(SceneObject obj);
        void add_object(PointLight obj);

        void sort_front_to_back(const Camera& camera);

    private:
        struct front_to_back
        {
        public:
            front_to_back(const Camera& cam) : camera(cam) {}

            bool operator()(const SceneObject& a, const SceneObject& b) const
            {
                return glm::distance(camera.position(), glm::vec3(a.transform()[3])) <
                    glm::distance(camera.position(), glm::vec3(b.transform()[3]));
            }
        private:
            Camera camera;
        };

        std::vector<std::vector<SceneObject>> _objects;
        std::vector<PointLight> _point_lights;
        glm::vec3 _sun_direction = glm::vec3(0.2f, 1.0f, 0.1f);
        OcclusionQuery occlusion_query;

};

}

#endif // SCENE_H
