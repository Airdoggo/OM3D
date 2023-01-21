#include "Scene.h"

#include <glad/glad.h>

#include <TypedBuffer.h>

#include <iostream>

namespace OM3D
{
    Scene::Scene()
    {}

    Scene::Scene(SceneObject &light_volume)
    : _light_volume(std::move(light_volume))
    {
        auto mapping = _buffer.map(AccessType::WriteOnly);
        mapping[0].sun_color = glm::vec3(1.0f, 1.0f, 1.0f);
        mapping[0].point_light_count = (glm::uint)_point_lights.size();
        mapping[0].sun_dir = glm::normalize(_sun_direction);
    }

    void Scene::add_object(SceneObject obj)
    {
        _render_info.objects++;

        for (auto &v : _objects)
        {
            if (obj == v.front())
            {
                v.emplace_back(std::move(obj));
                return;
            }
        }

        _objects.emplace_back(std::vector<SceneObject>());
        _objects.back().emplace_back(std::move(obj));
    }

    void Scene::add_object(PointLight obj)
    {
        _point_lights.emplace_back(std::move(obj));
    }

    void Scene::create_bounding_volume_hierarchy() {
        std::vector<BoundingTree> trees;

        for (auto &v : _objects)
        {
            for (auto &o : v)
            {
                trees.emplace_back(BoundingTree(o));
            }
        }

        _bounding_tree = BoundingTree(trees);
    }

    void Scene::update_frame(const Camera& camera) {
        auto mapping = _buffer.map(AccessType::WriteOnly);
        mapping[0].camera.view_proj = camera.view_proj_matrix();

        _frustum = camera.build_frustum();
    }

    bool frustum_cull(const SceneObject &obj, const Frustum &frustum)
    {
        return true;
        //const BoundingSphere &s = obj.get_bounding_sphere();

        // Get the real position of the object
        const glm::mat4 &t = obj.transform();
        const glm::vec3 pos = glm::vec3(t[3].x, t[3].y, t[3].z);// + obj.get_bounding_sphere().origin;

        // Scale the bounding sphere
        float scaleX = glm::length(t[0]);
        float scaleY = glm::length(t[1]);
        float scaleZ = glm::length(t[2]);

        float max_scale = scaleX > scaleY ? (scaleX > scaleZ ? scaleX : scaleZ) : (scaleZ > scaleY ? scaleZ : scaleY);
        float radius = 1;//s.radius * max_scale;

        // Check against every plane
        if (glm::dot(pos + frustum._left_normal * radius - frustum._position, frustum._left_normal) <= 0)
            return false;
        if (glm::dot(pos + frustum._right_normal * radius - frustum._position, frustum._right_normal) <= 0)
            return false;
        if (glm::dot(pos + frustum._near_normal * radius - frustum._position, frustum._near_normal) <= 0)
            return false;
        if (glm::dot(pos + frustum._top_normal * radius - frustum._position, frustum._top_normal) <= 0)
            return false;
        if (glm::dot(pos + frustum._bottom_normal * radius - frustum._position, frustum._bottom_normal) <= 0)
            return false;

        return true;
    }

    void Scene::render(const Camera &camera)
    {
        _buffer.bind(BufferUsage::Uniform, 0);

        _render_info.rendered = 0;

        auto objects = std::vector<std::vector<const SceneObject *>>(1);

        _bounding_tree.frustum_cull(objects, _frustum);

        // Render every object
        for (auto &v : objects)
        {
            // If there are not enough objects, the instancing overhead is too big and performances are lower
            if (v.size() < 50 || true)
            {
                for (const auto &o : v)
                {
                    o->render();
                    _render_info.rendered++;
                }

                continue;
            }

            size_t i = 0;

            // Fill and bind objects buffer
            TypedBuffer<shader::mat4> object_buffer(nullptr, std::max(v.size(), size_t(1)));
            {
                auto mapping = object_buffer.map(AccessType::WriteOnly);
                for (const SceneObject *obj : v)
                {
                    if (frustum_cull(*obj, _frustum))
                    {
                        mapping[i++] = {
                            obj->transform(),
                        };
                        _render_info.rendered++;
                    }
                }
            }
            object_buffer.bind(BufferUsage::Storage, 2);

            // Render every instance of this object
            v.front()->render(int(i));
        }
    }

    bool frustum_cull_light(const PointLight &light, const Frustum &frustum)
    {
        const glm::vec3 &position = light.position();
        float radius = light.radius();

        // Check against every plane
        if (glm::dot(position + frustum._left_normal * radius - frustum._position, frustum._left_normal) <= 0)
            return false;
        if (glm::dot(position + frustum._right_normal * radius - frustum._position, frustum._right_normal) <= 0)
            return false;
        if (glm::dot(position + frustum._near_normal * radius - frustum._position, frustum._near_normal) <= 0)
            return false;
        if (glm::dot(position + frustum._top_normal * radius - frustum._position, frustum._top_normal) <= 0)
            return false;
        if (glm::dot(position + frustum._bottom_normal * radius - frustum._position, frustum._bottom_normal) <= 0)
            return false;

        return true;
    }

    void Scene::compute_lights(const Camera& camera) const {

        _buffer.bind(BufferUsage::Uniform, 0);
        
        _sun_material.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        _light_volume.get_material()->set_uniform(HASH("inv_viewproj"), glm::inverse(camera.view_proj_matrix()));

        for (auto &pl : _point_lights) {
            if (frustum_cull_light(pl, _frustum))
                _light_volume.render_light_volume(pl.position(), pl.radius(), pl.color());
        }
    }

    void Scene::set_screen_size_uniform(glm::uvec2 window_size) {
        _light_volume.get_material()->set_uniform(HASH("screen_size"), window_size);
    }

    const RenderInfo &Scene::get_render_info() const {
        return _render_info;
    }

}
