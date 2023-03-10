#include "Material.h"

#include <glad/glad.h>

#include <algorithm>

namespace OM3D
{

    Material::Material()
    {
    }

    void Material::set_program(std::shared_ptr<Program> prog)
    {
        _program = std::move(prog);
    }

    void Material::set_blend_mode(BlendMode blend)
    {
        _blend_mode = blend;
    }

    void Material::set_depth_test_mode(DepthTestMode depth)
    {
        _depth_test_mode = depth;
    }

    void Material::set_texture(u32 slot, std::shared_ptr<Texture> tex)
    {
        if (const auto it = std::find_if(_textures.begin(), _textures.end(), [&](const auto &t)
                                         { return t.second == tex; });
            it != _textures.end())
        {
            it->second = std::move(tex);
        }
        else
        {
            _textures.emplace_back(slot, std::move(tex));
        }
    }

    void Material::set_write_z_buffer(bool val)
    {
        write_z_buffer = val;
    }

    void Material::bind() const
    {
        glDepthMask(write_z_buffer);

        switch (_blend_mode)
        {
        case BlendMode::NoBlendNoCulling:
            glDisable(GL_BLEND);
            glDisable(GL_CULL_FACE);
            break;
        case BlendMode::None:
            glDisable(GL_BLEND);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            break;

        case BlendMode::Alpha:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_CULL_FACE);
            break;

        case BlendMode::Additive:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        }

        switch (_depth_test_mode)
        {
        case DepthTestMode::None:
            glDisable(GL_DEPTH_TEST);
            break;

        case DepthTestMode::Equal:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_EQUAL);
            break;

        case DepthTestMode::Standard:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_GEQUAL);
            break;

        case DepthTestMode::Reversed:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_LEQUAL);
            break;

        case DepthTestMode::Xor:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_NOTEQUAL);
            break;

        case DepthTestMode::ReversedFrontCull:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CCW);
            break;
        }

        for (const auto &texture : _textures)
        {
            texture.second->bind(texture.first);
        }
        _program->bind();
    }

    std::shared_ptr<Material> Material::empty_material()
    {
        static std::weak_ptr<Material> weak_material;
        auto material = weak_material.lock();
        if (!material)
        {
            material = std::make_shared<Material>();
            material->_program = Program::from_files("gbuffer.frag", "basic.vert");
            weak_material = material;
        }
        return material;
    }

    Material Material::textured_material()
    {
        Material material;
        material._program = Program::from_files("gbuffer.frag", "basic.vert", {"TEXTURED 1"});
        return material;
    }

    Material Material::textured_normal_mapped_material()
    {
        Material material;
        material._program = Program::from_files("gbuffer.frag", "basic.vert", std::array<std::string, 2>{"TEXTURED 1", "NORMAL_MAPPED 1"});
        return material;
    }

    Material Material::debug_material()
    {
        Material material;
        material.set_blend_mode(BlendMode::NoBlendNoCulling);
        material.set_depth_test_mode(DepthTestMode::Xor);
        material.set_write_z_buffer(false);
        material._program = Program::from_files("debug.frag", "screen.vert");
        return material;
    }

    Material Material::aabb_material()
    {
        Material material;
        material.set_blend_mode(BlendMode::NoBlendNoCulling);
        material.set_depth_test_mode(DepthTestMode::None);
        material.set_write_z_buffer(false);
        material._program = Program::from_files("aabb.frag", "basic.vert");
        return material;
    }

}
