#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

vec3_t vec3_rotate_x(vec3_t v, float angle);

vec3_t vec3_rotate_y(vec3_t v, float angle);

vec3_t vec3_rotate_z(vec3_t v, float angle);

vec3_t vec3_translate(vec3_t v, float tx, float ty, float tz);

vec3_t vec3_scale(vec3_t v, float sx, float sy, float sz);

#endif
