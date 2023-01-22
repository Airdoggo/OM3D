#include "SceneObject.h"

#include <glm/gtc/matrix_transform.hpp>

namespace OM3D {

    SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh, std::shared_ptr<Material> material) :
        _mesh(std::move(mesh)),
        _material(std::move(material)) {
        float maxX, maxY, maxZ, minX, minY, minZ;
        auto obj_vertices = _mesh->_data.vertices;
        maxX = obj_vertices[0].position[0];
        maxY = obj_vertices[0].position[1];
        maxZ = obj_vertices[0].position[2];
        minX = obj_vertices[0].position[0];
        minY = obj_vertices[0].position[1];
        minZ = obj_vertices[0].position[2];

        for (int i = 1; i < obj_vertices.size(); i++)
        {
            Vertex current_vertex = obj_vertices[i];
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
        _bbox_material = std::make_shared<Material>(Material::bbox_material());
    }

    void SceneObject::render() const {
        if (!_material || !_mesh) {
            return;
        }

        _material->set_uniform(HASH("model"), transform());
        _material->set_uniform(HASH("instanced"), 0u);
        _material->bind();
        _mesh->draw();
    }

    void SceneObject::render_bbox() const {
        if (!_bbox_material || !_mesh || !_bb_mesh) {
            return;
        }

        _bbox_material->set_uniform(HASH("model"), transform());
        _bbox_material->set_uniform(HASH("instanced"), 0u);
        _bbox_material->bind();
        _bb_mesh->draw();
    }

    void SceneObject::render(int nb_instances) const {
        if (!_material || !_mesh) {
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