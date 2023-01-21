#include "StaticMesh.h"

#include <glad/glad.h>
#include <glm/gtx/norm.hpp>

namespace OM3D {

StaticMesh::StaticMesh(const MeshData& data) :
    _vertex_buffer(data.vertices),
    _index_buffer(data.indices) {
    
    auto &vert = data.vertices;

    _min_coords = vert[0].position;
    _max_coords = vert[0].position;

    // Compute the AABB for later
    for (size_t i = 1; i < vert.size(); i++) {
        auto &pos = vert[i].position;

        if (_min_coords.x > pos.x)
            _min_coords.x = pos.x;
        else if (_max_coords.x < pos.x)
            _max_coords.x = pos.x;

        if (_min_coords.y > pos.y)
            _min_coords.y = pos.y;
        else if (_max_coords.y < pos.y)
            _max_coords.y = pos.y;

        if (_min_coords.z > pos.z)
            _min_coords.z = pos.z;
        else if (_max_coords.z < pos.z)
            _max_coords.z = pos.z;
    }
}

void StaticMesh::draw() const {
    _vertex_buffer.bind(BufferUsage::Attribute);
    _index_buffer.bind(BufferUsage::Index);

    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
    // Vertex normal
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(float)));
    // Vertex uv
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(float)));
    // Tangent / bitangent sign
    glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(8 * sizeof(float)));
    // Vertex color
    glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(12 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glDrawElements(GL_TRIANGLES, int(_index_buffer.element_count()), GL_UNSIGNED_INT, nullptr);
}

void StaticMesh::draw(int nb_instances) const {
    _vertex_buffer.bind(BufferUsage::Attribute);
    _index_buffer.bind(BufferUsage::Index);

    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
    // Vertex normal
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(float)));
    // Vertex uv
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(float)));
    // Tangent / bitangent sign
    glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(8 * sizeof(float)));
    // Vertex color
    glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(12 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    //glDrawElements(GL_TRIANGLES, int(_index_buffer.element_count()), GL_UNSIGNED_INT, nullptr);
    glDrawElementsInstanced(GL_TRIANGLES, int(_index_buffer.element_count()), GL_UNSIGNED_INT, nullptr, nb_instances);
}

bool StaticMesh::operator==(const StaticMesh& other) const {
    return _vertex_buffer.element_count() == other._vertex_buffer.element_count()
        && _index_buffer.element_count() == other._index_buffer.element_count();
}

void StaticMesh::draw_light_volume() const {
    _vertex_buffer.bind(BufferUsage::Attribute);
    _index_buffer.bind(BufferUsage::Index);

    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, int(_index_buffer.element_count()), GL_UNSIGNED_INT, nullptr);
}

std::pair<glm::vec3, glm::vec3> StaticMesh::get_aabb() const {
    return { _min_coords, _max_coords };
}

}
