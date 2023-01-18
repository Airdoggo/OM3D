#include <GBuffer.h>
#include <array>

namespace OM3D {
	GBuffer::GBuffer(glm::vec2 window_size)
	{
		textures[GBUFFER_TEXTURE_TYPE::ALBEDO] = Texture(
			glm::vec2(window_size), ImageFormat::RGB8_sRGB);
		textures[GBUFFER_TEXTURE_TYPE::NORMAL] = Texture(
			glm::vec2(window_size), ImageFormat::RGBA8_UNORM);
		depthTex = Texture(
			glm::vec2(window_size), ImageFormat::Depth32_FLOAT);
		frameBuffer = Framebuffer(&depthTex, std::array{&textures[ALBEDO], &textures[NORMAL]});
	}

	GBuffer::~GBuffer()
	{
		frameBuffer.~Framebuffer();
	}

	void GBuffer::Bind()
	{
		frameBuffer.bind(true);
	}

	void GBuffer::Blit()
	{
		frameBuffer.blit(true);
	}

	void GBuffer::bind_textures()
	{
		textures[0].bind(0);
		textures[1].bind(1);
		depthTex.bind(2);
	}
}
