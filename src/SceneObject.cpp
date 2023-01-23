#include "SceneObject.h"

#include <glm/gtc/matrix_transform.hpp>

namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh, std::shared_ptr<Material> material) :
    _mesh(std::move(mesh)),
    _material(std::move(material)) {
}

void SceneObject::render() const {
    if(!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("model"), transform());
    _material->set_uniform(HASH("instanced"), 0u);
    _material->bind();
    _mesh->draw();
}

void SceneObject::render(int nb_instances) const {
    if(!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("instanced"), 1u);
    _material->bind();
    _mesh->draw(nb_instances);
}

void SceneObject::render_light_volume(const glm::vec3 &position, float radius, const glm::vec3 &color) const {
    if (!_material || !_mesh)
    {
        return;
    }

    _material->bind();
    _material->set_uniform(HASH("light.position"), position);
    _material->set_uniform(HASH("light.radius"), radius);
    _material->set_uniform(HASH("light.color"), color);

    // Sphere is imperfect, so we increase the mesh's radius by a little to prevent culling when we shouldn't
    float r = radius * 1.1f;

    glm::mat4 transform = { r, 0., 0., 0.,
                            0., r, 0., 0.,
                            0., 0., r, 0.,
                            position.x, position.y, position.z, 1.};
    _material->set_uniform(HASH("model"), transform);
    _mesh->draw_light_volume();
}

void SceneObject::set_transform(const glm::mat4 &tr) {
    _transform = tr;
}

const glm::mat4& SceneObject::transform() const {
    return _transform;
}

const BoundingSphere& SceneObject::get_bounding_sphere() const {
    return _mesh->_bounding_sphere;
}

const std::shared_ptr<Material>& SceneObject::get_material() const {
    return _material;
}

bool SceneObject::operator==(const SceneObject& other) const {
    return _material.get() == other._material.get() && *_mesh.get() == *other._mesh.get();
}

}
