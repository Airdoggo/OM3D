#include "Scene.h"

#include <glad/glad.h>

#include <TypedBuffer.h>

#include <iostream>

namespace OM3D
{
    Scene::Scene()
    {
        auto mapping = _buffer.map(AccessType::WriteOnly);
        mapping[0].sun_color = glm::vec3(1.0f, 1.0f, 1.0f);
        mapping[0].point_light_count = (glm::uint)_point_lights.size();
        mapping[0].sun_dir = glm::normalize(_sun_direction);

        std::vector<Vertex> cube_vertices = {
            {{-0.5, -0.5, 0.5}},
            {{0.5, -0.5, 0.5}},
            {{-0.5, 0.5, 0.5}},
            {{0.5, 0.5, 0.5}},
            {{-0.5, -0.5, -0.5}},
            {{0.5, -0.5, -0.5}},
            {{-0.5, 0.5, -0.5}},
            {{0.5, 0.5, -0.5,}}
        };
        std::vector<u32> cube_indices = {
            2, 7, 6,
            2, 3, 7,
            0, 4, 5,
            0, 5, 1,
            0, 2, 6,
            0, 6, 4,
            1, 7, 3,
            1, 5, 7,
            0, 3, 2,
            0, 1, 3,
            4, 6, 7,
            4, 7, 5};

        _cube = SceneObject(std::make_shared<StaticMesh>(MeshData{cube_vertices, cube_indices}),
                            std::make_shared<Material>(Material::aabb_material()));
    }

    void Scene::add_object(SceneObject obj, std::shared_ptr<SceneObject> *new_address)
    {
        _render_info.objects++;

        for (size_t i = 0; i < _objects.size(); i++)
        {
            if (obj == *_objects[i].front())
            {
                obj.id = i;
                _objects[i].emplace_back(std::make_shared<SceneObject>(std::move(obj)));
                if (new_address)
                    *new_address = _objects[i].back();
                return;
            }
        }

        _nb_different_objects++;

        obj.id = _objects.size();
        _objects.emplace_back(std::vector<std::shared_ptr<SceneObject>>());
        _objects.back().emplace_back(std::make_shared<SceneObject>(std::move(obj)));
        if (new_address)
            *new_address = _objects.back().back();
    }

    void Scene::add_object(PointLight obj)
    {
        _point_lights.emplace_back(std::move(obj));

        auto mapping = _buffer.map(AccessType::WriteOnly);
        mapping[0].point_light_count = (glm::uint)_point_lights.size();
    }

    void Scene::init_light_buffer()
    {
        _light_buffer = TypedBuffer<shader::PointLight>(nullptr, std::max(_point_lights.size(), size_t(1)));
        {
            auto mapping = _light_buffer.map(AccessType::WriteOnly);
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
    }

    void Scene::dynamic_add_object(SceneObject obj, size_t subdivisions)
    {
        std::shared_ptr<SceneObject> new_object;
        add_object(std::move(obj), &new_object);

        BoundingTree new_leaf(new_object);
        _bounding_tree.insert(new_leaf, subdivisions);
    }

    void Scene::dynamic_remove_object(const std::shared_ptr<SceneObject> &object)
    {
        if (_bounding_tree.remove(object) == Delete)
        {
            _bounding_tree = BoundingTree();
        }

        auto &v = _objects[object->id];
        v.erase(std::find(v.begin(), v.end(), object));
        _render_info.objects--;
    }

    void Scene::create_bounding_volume_hierarchy(size_t subdivisions)
    {
        std::vector<BoundingTree> trees;

        for (auto &v : _objects)
        {
            for (auto &o : v)
            {
                trees.emplace_back(BoundingTree(o));
            }
        }

        _bounding_tree = BoundingTree(trees);
        if (subdivisions > 1)
            _bounding_tree.subdivise(subdivisions);
    }

    void Scene::update_frame(const Camera &camera)
    {
        auto mapping = _buffer.map(AccessType::WriteOnly);
        mapping[0].camera.view_proj = camera.view_proj_matrix();

        _frustum = camera.build_frustum();
    }

    void Scene::render(const Camera &camera)
    {
        _buffer.bind(BufferUsage::Uniform, 0);

        _render_info.rendered = 0;
        _render_info.checks = 0;

        auto objects = std::vector<std::vector<std::shared_ptr<SceneObject>>>(_nb_different_objects);

        _bounding_tree.frustum_cull(objects, _frustum, _render_info.checks);

        // Render every object
        for (auto &v : objects)
        {
            // If there are not enough objects, the instancing overhead is too big and performances are lower
            if (v.size() < 50)
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
                for (const std::shared_ptr<SceneObject> &obj : v)
                {
                    mapping[i++] = {
                        obj->transform(),
                    };
                    _render_info.rendered++;
                }
            }
            object_buffer.bind(BufferUsage::Storage, 2);

            // Render every instance of this object
            v.front()->render(int(i));
        }
    }

    void Scene::render_aabb(size_t level) {
        _bounding_tree.draw_recursive(_cube, level);
    }

    void Scene::bind_buffers() const
    {
        _buffer.bind(BufferUsage::Uniform, 0);
        _light_buffer.bind(BufferUsage::Storage, 1);
    }

    const RenderInfo &Scene::get_render_info() const
    {
        return _render_info;
    }

    const size_t Scene::get_nb_lights() const
    {
        return _point_lights.size();
    }

}
