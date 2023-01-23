#version 450

#include "utils.glsl"

layout(location = 0) out vec4 out_color;

uniform vec3 middle;

vec3 hash(vec3 p)
{
	p = fract(p * vec3(.1031, .1030, .0973));
    p += dot(p, p.yxz + 33.33);
    p =  fract((p.xxy + p.yxx) * p.zyx);
    return vec3(abs(p.x), abs(p.y), abs(p.z));
}

void main() {
    out_color = vec4(hash(middle), 1.);
}

