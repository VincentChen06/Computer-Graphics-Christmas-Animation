#ifndef MESH_H
#define MESH_H
#include "vector.h"
#include "triangle.h"

//Square pyramid
#define N_MESH_VERTICES 5

extern vec3_t mesh_vertices[N_MESH_VERTICES];

#define N_MESH_FACES 8

extern face_t mesh_faces[N_MESH_FACES];

//Octahedron
#define N_MESH2_VERTICES 6  

extern vec3_t mesh2_vertices[N_MESH2_VERTICES];

#define N_MESH2_FACES 8      

extern face_t mesh2_faces[N_MESH2_FACES];


//Triangular pyramid
#define N_MESH3_VERTICES 4

extern vec3_t mesh3_vertices[N_MESH3_VERTICES];

#define N_MESH3_FACES 4

extern face_t mesh3_faces[N_MESH3_FACES];

#endif