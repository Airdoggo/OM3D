#version 450

#include "utils.glsl"

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;

layout(location = 0) out vec4 out_color;

// Pseudo-enum for debugging options
const uint ALBEDO = 1;
const uint NORMALS = 2;
const uint DEPTH = 3;

uniform uint debug;

void main() {
    
    const ivec2 coord = ivec2(gl_FragCoord.xy);

    if (debug == ALBEDO) {
        const vec3 albedo = texelFetch(in_albedo, coord, 0).rgb;
        out_color = vec4(albedo, 1.0);
    }
    else if (debug == NORMALS) {
        const vec3 normal = (texelFetch(in_normal, coord, 0).xyz - 0.5) * 2.;
        out_color = vec4(normal, 1.0);
    }
    else if (debug == DEPTH) {
        float depth = texelFetch(in_depth, coord, 0).x;
        depth *= 1e5;
        out_color = vec4(depth, depth, depth, 1.0);
    }
}

