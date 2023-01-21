#include "BoundingTree.h"

#include <algorithm>

namespace OM3D {

BoundingTree::BoundingTree() {}

BoundingTree::BoundingTree(std::shared_ptr<SceneObject> object)
    : _object(object)
    , _children(std::vector<BoundingTree>())
{
    auto aabb = object->get_aabb();
    _min_corner = aabb.first;
    _max_corner = aabb.second;
}

BoundingTree::BoundingTree(std::vector<BoundingTree> &children)
    : _object(nullptr)
    , _children(std::move(children))
{
    // Fit new bounding box to children bounding boxes
    fit_children();
}

void BoundingTree::subdivise(size_t subdivisions) {
    if (_children.size() <= subdivisions)
        return;

    float x_length = _max_corner.x - _min_corner.x;
    float y_length = _max_corner.y - _min_corner.y;
    float z_length = _max_corner.z - _min_corner.z;
    int longest_axis = x_length > y_length ? (x_length > z_length ? 0 : 2)
                                           : (y_length > z_length ? 1 : 2);
    
    // Compare the objects center relative to the longest axis of the current bounding tree
    auto cmp = [&](const BoundingTree &a, const BoundingTree &b) {
        return (a._min_corner[longest_axis] + a._max_corner[longest_axis]) * 0.5
             < (b._min_corner[longest_axis] + b._max_corner[longest_axis]) * 0.5;
    };

    std::sort(_children.begin(), _children.end(), cmp);

    std::vector<BoundingTree> new_children(subdivisions);
    for (size_t i = 0; i < subdivisions; i++) {

        // New vector containing every child in the space's subdivision
        std::vector<BoundingTree> new_child(
            _children.begin() + _children.size() * i / subdivisions,
            _children.begin() + _children.size() * (i + 1) / subdivisions
        );

        if (new_child.size() == 1) {
            new_children[i] = new_child.front();
        }
        else {
            new_children[i] = BoundingTree(new_child);
        }      
    }

    _children = std::move(new_children);

    for (auto &c : _children) {
        c.subdivise(subdivisions);
    }
}

bool BoundingTree::fit_children() {
    glm::vec3 current_min = _min_corner;
    glm::vec3 current_max = _max_corner;

    _min_corner = _children.front()._min_corner;
    _max_corner = _children.front()._max_corner;

    for (size_t i = 1; i < _children.size(); i++) {
        const glm::vec3 &min = _children[i]._min_corner;
        const glm::vec3 &max = _children[i]._max_corner;

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

    return _min_corner != current_min || _max_corner != current_max;
}

void BoundingTree::insert(BoundingTree &obj, size_t subdivision) {
    
}

RemoveResult BoundingTree::remove(const std::shared_ptr<SceneObject> object) {
    // Leaf, therefore object
    if (_children.empty()) {
        if (_object == object) {
            return Delete;
        }
        return NotFound;
    }

    for (auto it = _children.begin(); it != _children.end(); it++) {
        const glm::vec3 &min = it->_min_corner;
        const glm::vec3 &max = it->_max_corner;

        auto aabb = object->get_aabb();

        // Check if children can contain object
        if (min.x <= aabb.first.x && min.y <= aabb.first.y && min.z <= aabb.first.z
            && max.x >= aabb.second.x && max.y >= aabb.second.y && max.z >= aabb.second.z) {

            RemoveResult res = it->remove(object);
            if (res == Delete) {
                _children.erase(it);

                // One child means this node is useless
                if (_children.size() == 1) {

                    auto &child = _children.front();

                    _object = child._object;
                    _min_corner = child._min_corner;
                    _max_corner = child._max_corner;
                    _children = std::vector<BoundingTree>(child._children);

                    return Resize;
                }

                if (fit_children()) {
                    return Resize;
                }

                return NotFound;
            }
            else if (res == Resize) {
                return fit_children() ? Resize : NotFound;
            }
        }
    }

    return NotFound;
}

void BoundingTree::frustum_cull(std::vector<std::vector<std::shared_ptr<SceneObject>>> &objects, const Frustum &frustum, size_t &counter) const {
    counter++;
    if (frustum_cull_aabb(frustum))
        return;
    
    if (_children.empty() && _object) {
        objects[_object->id].push_back(_object);
        return;
    }

    for (auto &c : _children) {
        c.frustum_cull(objects, frustum, counter);
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

}