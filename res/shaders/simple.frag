#version 430 core

in layout(location = 0) vec3 normal_in;
in layout(location = 1) vec3 position;
in layout(location = 2) vec2 tex_cd;
in layout(location = 3) mat3 TBN;

const float ambient_intensity = 1.0;
const vec4 emissive_color = vec4(0,0,0,1);
const vec4 light_color = vec4(0.73, 0.94, 0.98, 1.0);

uniform layout(location = 6) vec3 light_1;
uniform layout(location = 7) vec3 light_2;
uniform layout(location = 8) vec3 light_3;

uniform layout(location = 9) vec3 spot_light_1;
uniform layout(location = 10) vec3 spot_light_1_dir;
uniform layout(location = 11) float spot_light_1_angle_cos;

uniform layout(location = 12) vec3 ball_position;
const float radius = 3.0;

uniform layout(location = 13) bool is_textured;
uniform layout(location = 14) bool is_lighted;
uniform layout(location = 15) bool is_normal_mapped;

uniform layout(binding = 0) sampler2D tex_sampl;
uniform layout(binding = 1) sampler2D nrm_sampl;

vec3 normal;

const vec3 camera_pos = vec3(0, 0, 0);
const float k = 16; // specular constant
float spec = 1.0;

// attenuation constants
const float a = 1;
const float b = 0.000052;
const float c = 0.0000025;

out vec4 color;

vec4 calculate_light(vec3 light_pos, vec4 diffuse_color) {
    vec3 to_light = light_pos - position;
    float dist = length(to_light);
    to_light = normalize(to_light);
    float diffuse_intensity = max(0, dot(to_light, normal));
    vec3 refl_light = reflect(-to_light, normal);
    vec3 to_camera = normalize(camera_pos - position);
    float specular_intensity = pow(max(0, dot(to_camera, refl_light)), k);
    float att = 1.0 / max(1, a + b * dist + c * dist * dist);
    diffuse_intensity *= 2.4 * att;
    specular_intensity *= att * spec;
    return (light_color * specular_intensity) + (diffuse_color * diffuse_intensity);
}

vec4 calculate_spot_light(vec4 diffuse_color) {
    vec3 to_light = spot_light_1 - position;
    float dist = length(to_light);
    to_light = normalize(to_light);
    if (dot(spot_light_1_dir, -to_light) < spot_light_1_angle_cos) {
        return vec4(0);
    }
    float diffuse_intensity = max(0, dot(to_light, normal));
    vec3 refl_light = reflect(-to_light, normal);
    vec3 to_camera = normalize(camera_pos - position);
    float specular_intensity = pow(max(0, dot(to_camera, refl_light)), k);
    float att = 1.0 / max(1, a + b * dist + c * dist * dist);
    diffuse_intensity *= 2 * att;
    specular_intensity *= att * spec;
    return (light_color * specular_intensity) + (diffuse_color * diffuse_intensity);
}

bool is_shadowed() {
    vec3 light_to_pos = position - spot_light_1;
    vec3 light_ray = normalize(light_to_pos);
    vec3 light_to_ball = ball_position - spot_light_1;
    float dp = dot(light_to_ball, light_ray);
    if (dp < 0) return false;
    float d2 = dot(light_to_ball, light_to_ball) // equals |v|^2
                - dp * dp;
    if (d2 > radius*radius) return false;
    float len = length(light_to_pos);
    float intersect = sqrt(radius*radius - d2);
    float t = intersect + dp;
    if (t > len) return false; // This means we are on the ball surface
    return true;
}

void main()
{
    if (is_normal_mapped) {
        normal = texture(nrm_sampl, tex_cd).xyz;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    } else {
        normal = normalize(normal_in);
    }
    vec4 diffuse_color;
    if (is_textured) {
        diffuse_color = texture(tex_sampl, tex_cd);
        spec = 0.4;
    } else {
        diffuse_color = vec4(0.2, 0.4, 0.2, 1.0);
        spec = 1.0;
    }
    color = diffuse_color * ambient_intensity;
    if (is_lighted) {
        color += emissive_color;
        //lights
        color += calculate_light(light_1, diffuse_color);
        color += calculate_light(light_2, diffuse_color);
        color += calculate_light(light_3, diffuse_color);

        //spot light
        if (!is_shadowed()) {
            color += calculate_spot_light(diffuse_color);
        }
    }
}
