#version 450

#include "utils.glsl"

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;

layout(binding = 0) uniform Data {
    FrameData frame;
};

layout(location = 0) out vec4 out_color;

const vec3 ambient = vec3(0.0);

void main() {

    const ivec2 coord = ivec2(gl_FragCoord.xy);

    const vec3 albedo = texelFetch(in_albedo, coord, 0).rgb;
    // Convert normals back to world-space
    const vec3 normal = (texelFetch(in_normal, coord, 0).xyz - 0.5) * 2.;

    vec3 acc = frame.sun_color * max(0.0, dot(frame.sun_dir, normal)) + ambient;
    out_color = vec4(albedo * acc, 1.0);
}
