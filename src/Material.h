#ifndef MATERIAL_H
#define MATERIAL_H

#include <Program.h>
#include <Texture.h>

#include <memory>
#include <vector>

namespace OM3D {

enum class BlendMode {
    NoBlendNoCulling,
    None,
    Alpha,
};

enum class DepthTestMode {
    Standard,
    Reversed,
    Equal,
    None,
    Xor,
};

class Material {

    public:
        Material();

        void set_program(std::shared_ptr<Program> prog);
        void set_blend_mode(BlendMode blend);
        void set_depth_test_mode(DepthTestMode depth);
        void set_texture(u32 slot, std::shared_ptr<Texture> tex);
        void set_write_z_buffer(bool val);

        template<typename... Args>
        void set_uniform(Args&&... args) {
            _program->set_uniform(FWD(args)...);
        }

        void bind() const;

        static std::shared_ptr<Material> empty_material();
        static Material textured_material();
        static Material textured_normal_mapped_material();
        static Material debug_material();
        static Material sun_light_material();
        static Material point_light_material();

    private:
        std::shared_ptr<Program> _program;
        std::vector<std::pair<u32, std::shared_ptr<Texture>>> _textures;

        BlendMode _blend_mode = BlendMode::None;
        DepthTestMode _depth_test_mode = DepthTestMode::Standard;
        bool write_z_buffer = true;

};

}

#endif // MATERIAL_H
