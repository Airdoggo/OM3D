#include "BoundingTree.h"
#include <iostream>
namespace OM3D {

BoundingTree::BoundingTree() {}

BoundingTree::BoundingTree(const SceneObject &object)
    : _object(&object)
    , _children(std::vector<BoundingTree>())
{
    auto aabb = object.get_aabb();
    _min_corner = aabb.first;
    _max_corner = aabb.second;
}

BoundingTree::BoundingTree(std::vector<BoundingTree> &children)
    : _object(nullptr)
    , _children(std::move(children))
{
    _min_corner = _children.front().get_min_corner();
    _max_corner = _children.back().get_max_corner();

    // Fit new bounding box to children bounding boxes
    for (size_t i = 1; i < _children.size(); i++) {
        const glm::vec3 &min = _children[i].get_min_corner();
        const glm::vec3 &max = _children[i].get_max_corner();

        if (_min_corner.x > min.x)
            _min_corner.x = min.x;
        if (_min_corner.y > min.y)
            _min_corner.y = min.y;
        if (_min_corner.z > min.z)
            _min_corner.z = min.z;

        if (_max_corner.x < max.x)
            _max_corner.x = max.x;
        if (_max_corner.y < max.y)
            _max_corner.y = max.y;
        if (_max_corner.z < max.z)
            _max_corner.z = max.z;
    }
}

void BoundingTree::frustum_cull(std::vector<std::vector<const SceneObject *>> &objects, const Frustum &frustum) const {
    if (frustum_cull_aabb(frustum))
        return;
    
    if (_children.empty()) {
        objects[_object->id].push_back(_object);
        return;
    }

    for (auto &c : _children) {
        c.frustum_cull(objects, frustum);
    }
}

bool BoundingTree::frustum_cull_aabb(const Frustum &frustum) const {

    if (frustum_cull_aabb_plane(frustum._near_normal, frustum._position))
        return true;
    if (frustum_cull_aabb_plane(frustum._left_normal, frustum._position))
        return true;
    if (frustum_cull_aabb_plane(frustum._right_normal, frustum._position))
        return true;
    if (frustum_cull_aabb_plane(frustum._top_normal, frustum._position))
        return true;
    if (frustum_cull_aabb_plane(frustum._bottom_normal, frustum._position))
        return true;
    return false;
}

bool BoundingTree::frustum_cull_aabb_plane(const glm::vec3 &plane_normal, const glm::vec3 &plane_position) const {
    glm::vec3 far_vert(
        plane_normal.x < 0.0f ? _min_corner.x : _max_corner.x,
        plane_normal.y < 0.0f ? _min_corner.y : _max_corner.y,
        plane_normal.z < 0.0f ? _min_corner.z : _max_corner.z
    );

    return dot(plane_normal, far_vert - plane_position) <= 0;
}

const glm::vec3 &BoundingTree::get_min_corner() {
    return _min_corner;
}

const glm::vec3 &BoundingTree::get_max_corner() {
    return _max_corner;
}

}