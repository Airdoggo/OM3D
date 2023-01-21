#ifndef BOUNDINGTREE_H
#define BOUNDINGTREE_H

#include <glm/vec3.hpp>
#include <vector>

#include "Camera.h"
#include "SceneObject.h"

namespace OM3D {

class BoundingTree {
    public:
        BoundingTree();
        BoundingTree(const SceneObject &object);
        BoundingTree(std::vector<BoundingTree> &children);

        void subdivise(size_t subdivisions);

        void frustum_cull(std::vector<std::vector<const SceneObject *>> &objects, const Frustum &frustum, size_t &counter) const;
        bool frustum_cull_aabb(const Frustum &frustum) const;
        bool frustum_cull_aabb_plane(const glm::vec3 &plane, const glm::vec3 &plane_position) const;

        const glm::vec3 &get_min_corner() const;
        const glm::vec3 &get_max_corner() const;

    private:
        const SceneObject *_object;
        std::vector<BoundingTree> _children;

        glm::vec3 _min_corner;
        glm::vec3 _max_corner;
};

}

#endif // BOUNDINGTREE_H
