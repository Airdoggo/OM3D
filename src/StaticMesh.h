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

        bool operator==(const StaticMesh& other) const;

        BoundingSphere _bounding_sphere;
        MeshData _data;
        size_t _nb_triangles;

    private:
        TypedBuffer<Vertex> _vertex_buffer;
        TypedBuffer<u32> _index_buffer;
};

}

#endif // STATICMESH_H
