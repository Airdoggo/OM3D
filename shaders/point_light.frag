#version 450

#include "utils.glsl"

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;

uniform mat4 inv_viewproj;
uniform vec2 screen_size;

layout(location = 0) out vec4 out_color;

uniform PointLight light;

vec3 unproject(vec2 uv, float depth) {
    const vec3 ndc = vec3(uv * 2.0 - vec2(1.0), depth);
    const vec4 p = inv_viewproj * vec4(ndc, 1.0);
    return p.xyz / p.w;
}

void main() {

    const ivec2 coord = ivec2(gl_FragCoord.xy);

    const vec3 albedo = texelFetch(in_albedo, coord, 0).rgb;
    const float depth = texelFetch(in_depth, coord, 0).x;
    // Convert normals back to world-space
    const vec3 normal = (texelFetch(in_normal, coord, 0).xyz - 0.5) * 2.;

    vec3 position = unproject(coord / screen_size, depth);

    const vec3 to_light = (light.position - position);
    const float dist = length(to_light);
    const vec3 light_vec = to_light / dist;

    const float NoL = max(0., dot(light_vec, normal));
    const float att = attenuation(dist, light.radius);

    vec3 acc = light.color * (NoL * att);

    out_color = vec4(linear_to_sRGB(albedo * acc), 1.0);
}

