#include "vector.h" 
#include <math.h>

vec3_t vec3_rotate_x(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
      .x = v.x,
      .y = v.y * cos(angle) - v.z * sin(angle),
      .z = v.y * sin(angle) + v.z * cos(angle),
    };
    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
      .x = v.x * cos(angle) - v.z * sin(angle),
      .y = v.y,
      .z = v.x * sin(angle) + v.z * cos(angle),
    };
    return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
      .x = v.x * cos(angle) - v.y * sin(angle),
      .y = v.x * sin(angle) + v.y * cos(angle),
      .z = v.z
    };
    return rotated_vector;
}

vec3_t vec3_translate(vec3_t v, float tx, float ty, float tz) {
    vec3_t translated_vector = {
        .x = v.x + tx,
        .y = v.y + ty,
        .z = v.z + tz
    };
    return translated_vector;
}

vec3_t vec3_scale(vec3_t v, float sx, float sy, float sz) {
    vec3_t scaled_vector = {
        .x = v.x * sx,
        .y = v.y * sy,
        .z = v.z * sz
    };
    return scaled_vector;
}


