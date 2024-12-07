#include "mesh.h"

//vertices for a square pyramid
vec3_t mesh_vertices[N_MESH_VERTICES] = {
    {.x = 0, .y = 1, .z = 0},   // Top vertex 0
    {.x = -1, .y = -1, .z = 1}, // Bottom left vertex 1 
    {.x = 1, .y = -1, .z = 1},  // Bottom right vertex 2
    {.x = 1, .y = -1, .z = -1}, // Back right vertex 3
    {.x = -1, .y = -1, .z = -1} // Back left vertex 4
};

//faces for the square pyramid
face_t mesh_faces[N_MESH_FACES] = {
    //Base
    {.a = 1, .b = 2, .c = 3}, 
    {.a = 1, .b = 3, .c = 4}, 
    {.a = 1, .b = 4, .c = 5}, 
    {.a = 1, .b = 5, .c = 2}, 
    //Side
    {.a = 2, .b = 3, .c = 0}, 
    {.a = 3, .b = 4, .c = 0}, 
    {.a = 4, .b = 5, .c = 0}, 
    {.a = 5, .b = 2, .c = 0}  
};

//vertices for an octahedron
vec3_t mesh2_vertices[N_MESH2_VERTICES] = {
    {.x = 0, .y = 1, .z = 0},   // Top vertex 0
    {.x = 1, .y = 0, .z = 0},   // Right vertex 1
    {.x = 0, .y = 0, .z = 1},   // Front vertex 2
    {.x = -1, .y = 0, .z = 0},  // Left vertex 3
    {.x = 0, .y = 0, .z = -1},  // Back vertex 4
    {.x = 0, .y = -1, .z = 0}   // Bottom vertex 5
};

//faces for the octahedron
face_t mesh2_faces[N_MESH2_FACES] = {
    {.a = 0, .b = 1, .c = 2},  
    {.a = 0, .b = 2, .c = 3},  
    {.a = 0, .b = 3, .c = 4},  
    {.a = 0, .b = 4, .c = 1},  
    {.a = 1, .b = 2, .c = 5},  
    {.a = 2, .b = 3, .c = 5},  
    {.a = 3, .b = 4, .c = 5},  
    {.a = 4, .b = 1, .c = 5}   
};

// Vertices for a triangular pyramid
vec3_t mesh3_vertices[N_MESH3_VERTICES] = {
    {.x = 0, .y = 1, .z = 0},   // Top vertex 0
    {.x = -1, .y = -1, .z = 1}, // Bottom left vertex 1
    {.x = 1, .y = -1, .z = 1},  // Bottom right vertex 2
    {.x = 0, .y = -1, .z = -1}  // Bottom back vertex 3
};

// Faces for the triangular pyramid
face_t mesh3_faces[N_MESH3_FACES] = {
    {.a = 0, .b = 1, .c = 2}, 
    {.a = 0, .b = 2, .c = 3}, 
    {.a = 0, .b = 3, .c = 1}, 
    {.a = 1, .b = 2, .c = 3}  
};

