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

        // bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

        void Bind();

        void Blit();

    private:
        Texture textures[NUM_TEXTURES];
        Texture depthTex;
        Framebuffer frameBuffer;
    };
}