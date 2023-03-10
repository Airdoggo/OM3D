#ifndef STATICMESH_H
#define STATICMESH_H

#include <graphics.h>
#include <TypedBuffer.h>
#include <Vertex.h>

#include <vector>

namespace OM3D {

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
};

struct BoundingSphere {
    glm::vec3 origin;
    float radius;
};

class StaticMesh : NonCopyable {

    public:
        StaticMesh() = default;
        StaticMesh(StaticMesh&&) = default;
        StaticMesh& operator=(StaticMesh&&) = default;

        StaticMesh(const MeshData& data);

        void draw() const;
        void draw(int nb_instances) const;
        void draw_light_volume() const;

        std::pair<glm::vec3, glm::vec3> get_aabb() const;

        bool operator==(const StaticMesh& other) const;

    private:
        TypedBuffer<Vertex> _vertex_buffer;
        TypedBuffer<u32> _index_buffer;

        glm::vec3 _min_coords;
        glm::vec3 _max_coords;
};

}

#endif // STATICMESH_H
