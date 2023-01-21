#include "SceneObject.h"

#include <glm/gtc/matrix_transform.hpp>

namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh, std::shared_ptr<Material> material) :
    _mesh(std::move(mesh)),
    _material(std::move(material)) {
    // compute the max/min positions for the model mesh
    float maxX, maxY, maxZ, minX, minY, minZ;
    auto oVertices = _mesh->_data.vertices;
    maxX = oVertices[0].position[0];
    maxY = oVertices[0].position[1];
    maxZ = oVertices[0].position[2];
    minX = oVertices[0].position[0];
    minY = oVertices[0].position[1];
    minZ = oVertices[0].position[2];

    for (int i = 1; i < oVertices.size(); i++)
    {
        Vertex current_vertex = oVertices[i];
        maxX = current_vertex.position[0] > maxX ? current_vertex.position[0] : maxX;
        maxY = current_vertex.position[1] > maxY ? current_vertex.position[1] : maxY;
        maxZ = current_vertex.position[2] > maxZ ? current_vertex.position[2] : maxZ;

        minX = current_vertex.position[0] < minX ? current_vertex.position[0] : minX;
        minY = current_vertex.position[1] < minY ? current_vertex.position[1] : minY;
        minZ = current_vertex.position[2] < minZ ? current_vertex.position[2] : minZ;
    }

    // create the bounding box for occlusion culling
    std::vector<Vertex> vertices = {
        { glm::vec3(minX, minY, minZ), glm::vec3(-0.57735, -0.57735, -0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(maxX, minY, minZ), glm::vec3(0.57735, -0.57735, -0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(maxX, maxY, minZ), glm::vec3(0.57735,  0.57735, -0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(minX, maxY, minZ), glm::vec3(-0.57735,  0.57735, -0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(minX, minY, maxZ), glm::vec3(-0.57735, -0.57735,  0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(maxX, minY, maxZ), glm::vec3(0.57735, -0.57735,  0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(maxX, maxY, maxZ), glm::vec3(0.57735,  0.57735,  0.57735), glm::vec2(0.f, 0.f)},
        { glm::vec3(minX, maxY, maxZ), glm::vec3(-0.57735,  0.57735,  0.57735), glm::vec2(0.f, 0.f)},
    };

    std::vector<u32> indices = {
        0, 2, 1,
        0, 3, 2,
        0, 1, 5,
        0, 5, 4,
        1, 2, 6,
        1, 6, 5,
        4, 5, 6,
        4, 6, 7,
        3, 0, 4,
        3, 4, 7,
        2, 3, 7,
        2, 7, 6,
    };

    MeshData bb_mesh_data = { std::move(vertices), std::move(indices) };
    _bb_mesh = std::make_shared<StaticMesh>(StaticMesh(bb_mesh_data));
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

void SceneObject::render_bbox() const {
    if (!_material || !_mesh || !_bb_mesh) {
        return;
    }

    _material->set_uniform(HASH("model"), transform());
    _material->set_uniform(HASH("instanced"), 0u);
    _material->bind();
    _bb_mesh->draw();
}

void SceneObject::render(int nb_instances) const {
    if(!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("instanced"), 1u);
    _material->bind();
    _mesh->draw(nb_instances);
}

void SceneObject::set_transform(const glm::mat4& tr) {
    _transform = tr;
}

const glm::mat4& SceneObject::transform() const {
    return _transform;
}

const BoundingSphere &SceneObject::get_bounding_sphere() const {
    return _mesh->_bounding_sphere;
}

const std::shared_ptr<Material> &SceneObject::get_material() const {
    return _material;
}

bool SceneObject::operator==(const SceneObject& other) const {
    return _material.get() == other._material.get() && *_mesh.get() == *other._mesh.get();
}

}
