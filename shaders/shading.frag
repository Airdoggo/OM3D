#version 450

#include "utils.glsl"

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;

uniform mat4 inv_viewproj;
uniform vec2 screen_size;

layout(location = 0) out vec4 out_color;

// Pseudo-enum for debugging options
const uint NO_DEBUG = 0;
const uint ALBEDO = 1;
const uint NORMALS = 2;
const uint DEPTH = 3;

uniform uint debug;

vec3 unproject(vec2 uv, float depth) {
    const vec3 ndc = vec3(uv * 2.0 - vec2(1.0), depth);
    const vec4 p = inv_viewproj * vec4(ndc, 1.0);
    return p.xyz / p.w;
}

void main() {
    
    const ivec2 coord = ivec2(gl_FragCoord.xy);

    const vec3 albedo = texelFetch(in_albedo, coord, 0).rgb;
    const float depth = gl_FragCoord.z;
    // Convert normals back to world-space
    const vec3 normal = (texelFetch(in_normal, coord, 0).xyz - 0.5) * 2.;

    vec3 color = unproject(coord / screen_size, depth);

    if (debug == NO_DEBUG)
        out_color = vec4(linear_to_sRGB(color), 1.0);
    else if (debug == ALBEDO)
        out_color = vec4(linear_to_sRGB(albedo), 1.0);
    else if (debug == NORMALS)
        out_color = vec4(normal, 1.0);
    else if (debug == DEPTH) {
        float d = depth * 1e5;
        out_color = vec4(d, d, d, 1.0);
    }
}

