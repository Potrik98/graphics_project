#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 normal_in;
in layout(location = 2) vec2 tx_cd_in;

in layout(location = 3) vec3 tangent;
in layout(location = 4) vec3 bitangent;

uniform layout(location = 3) mat4 VP;
uniform layout(location = 4) mat4 M;
uniform layout(location = 5) mat4 N;

out layout(location = 0) vec3 normal_out;
out layout(location = 1) vec3 position_out;
out layout(location = 2) vec2 tex_cd_out;
out layout(location = 3) mat3 TBN;

void main()
{
    normal_out = normalize((N * vec4(normal_in, 1.0)).xyz);
    vec4 p = M * vec4(position, 1.0f);
    gl_Position = VP * p;
    position_out = p.xyz;
    tex_cd_out = tx_cd_in;

    vec3 T = normalize(vec3(M * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(M * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(M * vec4(normal_in, 0.0)));
    TBN = mat3(T, B, N);
}
