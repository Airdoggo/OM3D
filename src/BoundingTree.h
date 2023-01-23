#ifndef BOUNDINGTREE_H
#define BOUNDINGTREE_H

#include <glm/vec3.hpp>
#include <vector>

#include "Camera.h"
#include "SceneObject.h"

namespace OM3D {

enum RemoveResult {
    NotFound,
    Delete,
    Resize,
};

class BoundingTree {
    public:
        BoundingTree();
        BoundingTree(std::shared_ptr<SceneObject> object);
        BoundingTree(std::vector<BoundingTree> &children);

        void subdivise(size_t subdivisions);
        bool fit_children();

        void insert(BoundingTree &obj, size_t subdivision);
        RemoveResult remove(const std::shared_ptr<SceneObject> object);

        void frustum_cull(std::vector<std::vector<std::shared_ptr<SceneObject>>> &objects, const Frustum &frustum, size_t &counter) const;
        bool frustum_cull_aabb(const Frustum &frustum) const;
        bool frustum_cull_aabb_plane(const glm::vec3 &plane, const glm::vec3 &plane_position) const;

        void draw_recursive(SceneObject &cube, size_t level);

    private:
        std::shared_ptr<SceneObject> _object;
        std::vector<BoundingTree> _children;

        glm::vec3 _min_corner;
        glm::vec3 _max_corner;
};

}

#endif // BOUNDINGTREE_H
