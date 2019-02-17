#version 430 core

in layout(location = 0) vec3 normal_in;
in layout(location = 1) vec3 position;

const vec4 ambient_color = vec4(0.4, 0.35, 0.5, 1.0);
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

const vec3 camera_pos = vec3(0, 0, 0);
const float k = 16; // specular constant

// attenuation constants
const float a = 2;
const float b = 0.004;
const float c = 0.0005;

out vec4 color;

vec4 calculate_light(vec3 light_pos) {
    vec3 normal = normalize(normal_in);
    vec3 to_light = light_pos - position;
    float dist = length(to_light);
    to_light = normalize(to_light);
    float diffuse_intensity = max(0, dot(to_light, normal));
    vec3 refl_light = reflect(-to_light, normal);
    vec3 to_camera = normalize(camera_pos - position);
    float specular_intensity = pow(max(0, dot(to_camera, refl_light)), k);
    float att = 1.0 / max(1, a + b * dist + c * dist * dist);
    diffuse_intensity *= att;
    specular_intensity *= att;
    return light_color * (diffuse_intensity + specular_intensity);
}

vec4 calculate_spot_light() {
    vec3 to_light = spot_light_1 - position;
    float dist = length(to_light);
    to_light = normalize(to_light);
    vec3 normal = normalize(normal_in);
    if (dot(spot_light_1_dir, -to_light) < spot_light_1_angle_cos) {
        return vec4(0);
    }
    float diffuse_intensity = max(0, dot(to_light, normal));
    vec3 refl_light = reflect(-to_light, normal);
    vec3 to_camera = normalize(camera_pos - position);
    float specular_intensity = pow(max(0, dot(to_camera, refl_light)), k);
    float att = 1.0 / max(1, a + b * dist + c * dist * dist);
    diffuse_intensity *= att;
    specular_intensity *= att;
    return light_color * (diffuse_intensity + specular_intensity);
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
    color = ambient_color + emissive_color;
    
    //lights
    color += calculate_light(light_1);
    color += calculate_light(light_2);
    color += calculate_light(light_3);

    //spot light
    if (!is_shadowed()) {
        color += calculate_spot_light();
    }
}
