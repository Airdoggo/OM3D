#include "Scene.h"

#include <TypedBuffer.h>

#include <shader_structs.h>

#include <iostream>

namespace OM3D
{

    Scene::Scene()
    {
    }

    void Scene::add_object(SceneObject obj)
    {
        for (auto &v : _objects)
        {
            if (obj.get_material().get() == v.front().get_material().get())
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

    bool frustum_cull(const SceneObject &obj, const Frustum &frustum, const glm::vec3 &camera)
    {
        const BoundingSphere &s = obj.get_bounding_sphere();

        // Get the real position of the object
        const glm::mat4 &t = obj.transform();
        const glm::vec3 pos = glm::vec3(t[3].x, t[3].y, t[3].z) + obj.get_bounding_sphere().origin;

        // Scale the bounding sphere
        float scaleX = glm::length(t[0]);
        float scaleY = glm::length(t[1]);
        float scaleZ = glm::length(t[2]);

        float max_scale = scaleX > scaleY ? (scaleX > scaleZ ? scaleX : scaleZ) : (scaleZ > scaleY ? scaleZ : scaleY);
        float radius = s.radius * max_scale;

        // Check against every plane
        if (glm::dot(pos + frustum._left_normal * radius - camera, frustum._left_normal) <= 0)
            return false;
        if (glm::dot(pos + frustum._right_normal * radius - camera, frustum._right_normal) <= 0)
            return false;
        if (glm::dot(pos + frustum._near_normal * radius - camera, frustum._near_normal) <= 0)
            return false;
        if (glm::dot(pos + frustum._top_normal * radius - camera, frustum._top_normal) <= 0)
            return false;
        if (glm::dot(pos + frustum._bottom_normal * radius - camera, frustum._bottom_normal) <= 0)
            return false;

        return true;
    }

    void Scene::render(const Camera &camera) const
    {
        // Fill and bind frame data buffer
        TypedBuffer<shader::FrameData> buffer(nullptr, 1);
        {
            auto mapping = buffer.map(AccessType::WriteOnly);
            mapping[0].camera.view_proj = camera.view_proj_matrix();
            mapping[0].point_light_count = u32(_point_lights.size());
            mapping[0].sun_color = glm::vec3(1.0f, 1.0f, 1.0f);
            mapping[0].sun_dir = glm::normalize(_sun_direction);
        }
        buffer.bind(BufferUsage::Uniform, 0);

        // Fill and bind lights buffer
        TypedBuffer<shader::PointLight> light_buffer(nullptr, std::max(_point_lights.size(), size_t(1)));
        {
            auto mapping = light_buffer.map(AccessType::WriteOnly);
            for (size_t i = 0; i != _point_lights.size(); ++i)
            {
                const auto &light = _point_lights[i];
                mapping[i] = {
                    light.position(),
                    light.radius(),
                    light.color(),
                    0.0f};
            }
        }
        light_buffer.bind(BufferUsage::Storage, 1);

        Frustum frustum = camera.build_frustum();
        glm::vec3 camera_pos = camera.position();

        // Render every object
        /*for (const auto &p : _objects)
        {
            for (const auto &o : p)
            {
                if (frustum_cull(o, frustum, camera_pos))
                    o.render();
            }
        }
        return;*/

        for (auto &v : _objects)
        {
            size_t i = 0;

            // Fill and bind objects buffer
            TypedBuffer<shader::mat4> object_buffer(nullptr, std::max(v.size(), size_t(1)));
            {
                auto mapping = object_buffer.map(AccessType::WriteOnly);
                for (const SceneObject &obj : v)
                {
                    if (frustum_cull(obj, frustum, camera_pos))
                    {
                        mapping[i++] = {
                            obj.transform(),
                        };
                    }
                }
            }
            object_buffer.bind(BufferUsage::Storage, 2);

            // Render every instance of this object
            v.front().render(int(i));
        }
    }

}
