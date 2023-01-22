#include "Scene.h"

#include <glad/glad.h>

#include <TypedBuffer.h>

#include <shader_structs.h>

#include <iostream>

#include <algorithm>

namespace OM3D
{

    Scene::Scene()
    {
        occlusion_query = OcclusionQuery();
    }

    void Scene::add_object(SceneObject obj)
    {
        _objects.emplace_back(std::move(obj));
    }


    void Scene::add_object(PointLight obj)
    {
        _point_lights.emplace_back(std::move(obj));
    }

    void Scene::sort_front_to_back(const glm::vec3& camera_pos)
    {
        std::sort(_objects.begin(), _objects.end(), front_to_back(camera_pos));
    }

    bool frustum_cull(const SceneObject& obj, const Frustum& frustum, const glm::vec3& camera)
    {
        const BoundingSphere& s = obj.get_bounding_sphere();

        // Get the real position of the object
        const glm::mat4& t = obj.transform();
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

    void Scene::compute_occlusion_query(const Camera& camera) {
        TypedBuffer<shader::FrameData> buffer(nullptr, 1);
        {
            auto mapping = buffer.map(AccessType::WriteOnly);
            mapping[0].camera.view_proj = camera.view_proj_matrix();
            mapping[0].point_light_count = u32(_point_lights.size());
            mapping[0].sun_color = glm::vec3(1.0f, 1.0f, 1.0f);
            mapping[0].sun_dir = glm::normalize(_sun_direction);
        }
        buffer.bind(BufferUsage::Uniform, 0);
        // Disable writing to frame and depth buffer
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);

        for (SceneObject& obj : _objects) {
            occlusion_query.begin_query();
            obj.render_bbox();
            occlusion_query.end_query();
            obj.is_visible = occlusion_query.has_samples_passed();
        }

        // Enable writing to frame and depth buffer
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);
    }

    void Scene::render(const Camera& camera)
    {
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
                const auto& light = _point_lights[i];
                mapping[i] = {
                    light.position(),
                    light.radius(),
                    light.color(),
                    0.0f };
            }
        }
        light_buffer.bind(BufferUsage::Storage, 1);

        Frustum frustum = camera.build_frustum();
        glm::vec3 camera_pos = camera.position();

        // Render every object
        for (auto& o : _objects)
        {
            if (!frustum_cull(o, frustum, camera_pos)) {
                o.is_visible = false;
                continue;
            }
            if (o.get_nb_triangles() < 200) {
                o.render();
                continue;
            }
            if (o.is_visible)
            {
                occlusion_query.begin_query();
                o.render();
                occlusion_query.end_query();
                o.is_visible = occlusion_query.has_samples_passed();
            }
            else {
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                glDepthMask(GL_FALSE);

                occlusion_query.begin_query();
                o.render_bbox();
                occlusion_query.end_query();
                o.is_visible = occlusion_query.has_samples_passed();

                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                glDepthMask(GL_TRUE);
            }
        }
    }
}