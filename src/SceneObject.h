#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <StaticMesh.h>
#include <Material.h>

#include <memory>

#include <glm/matrix.hpp>

namespace OM3D {

class SceneObject : NonCopyable {

    public:
        SceneObject(std::shared_ptr<StaticMesh> mesh = nullptr, std::shared_ptr<Material> material = nullptr);

        void render() const;
        void render(int nb_instances) const;
        void render_light_volume(const glm::vec3 &position, float radius, const glm::vec3 &color) const;

        void set_transform(const glm::mat4& tr);
        const glm::mat4& transform() const;

        const std::shared_ptr<Material> &get_material() const;
        std::pair<glm::vec3, glm::vec3> get_aabb() const;

        bool operator==(const SceneObject& other) const;

    private:
        glm::mat4 _transform = glm::mat4(1.0f);

        std::shared_ptr<StaticMesh> _mesh;
        std::shared_ptr<Material> _material;
};

}

#endif // SCENEOBJECT_H
