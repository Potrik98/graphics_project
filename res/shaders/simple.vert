#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 normal_in;

uniform layout(location = 3) mat4 VP;
uniform layout(location = 4) mat4 M;
uniform layout(location = 5) mat4 N;

out layout(location = 0) vec3 normal_out;
out layout(location = 1) vec3 position_out;

void main()
{
    normal_out = normalize((N * vec4(normal_in, 1.0)).xyz);
    vec4 p = M * vec4(position, 1.0f);
    gl_Position = VP * p;
    position_out = p.xyz;
}
