#pragma once

#include <utils.h>
#include <Framebuffer.h>
#include <glad/glad.h>

namespace OM3D {

    class GBuffer
    {
    public:

        enum GBUFFER_TEXTURE_TYPE {
            ALBEDO,
            NORMAL,
            NUM_TEXTURES
        };

        GBuffer(glm::vec2 window_size);

        ~GBuffer();

        void Bind();

        void Blit();

        void bind_textures();

    private:
        Texture textures[NUM_TEXTURES];
        Texture depthTex;
        Framebuffer frameBuffer;
    };
}