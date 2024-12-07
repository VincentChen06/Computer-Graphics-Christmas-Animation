#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include <stdlib.h>
#include "vector.h"
#include "mesh.h"

#define FPS 30
#define FRAME_TARGET_TIME (1000/FPS)

// Global Variables
SDL_Texture* textures = NULL;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
uint32_t* color_buffer = NULL;

bool is_running = false;

int window_width;
int window_height;
int rect_x = 0;
int snowman_x = 0;
int poly_y = 0;
int scaling_factor = 1000;
int previous_frame_time = 0;

triangle_t triangles_to_render[N_MESH_FACES];
triangle_t triangles2_to_render[N_MESH2_FACES];
triangle_t triangles3_to_render[N_MESH3_FACES];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };

vec3_t square_pyramid_scaling = { .x = 1, .y = 1, .z = 1 };
vec3_t square_pyramid_rotation = { .x = 0, .y = 0, .z = 0 };
vec3_t square_pyramid_translation = { .x = 0, .y = 0, .z = 0 };

vec3_t octahedron_scaling = { .x = 1, .y = 3, .z = 1 }; 
vec3_t octahedron_rotation = { .x = 0, .y = 0, .z = 0 };
vec3_t octahedron_translation = { .x = 0, .y = 0, .z = 0 };

vec3_t triangular_pyramid_scaling = { .x = 1, .y = 1, .z = 1 };
vec3_t triangular_pyramid_rotation = { .x = 0, .y = 0, .z = 0 };
vec3_t triangular_pyramid_translation = { .x = 0, .y = 0, .z = 0 };

vec3_t octahedron2_scaling = { .x = 1, .y = 1, .z = 1 };
vec3_t octahedron2_rotation = { .x = 0, .y = 0, .z = 0 };
vec3_t octahedron2_translation = { .x = 0, .y = 0, .z = 0 };

//Function Declarations
bool initialize_windowing_system();
void clean_up();
void run_render_pipeline();
void process_keyboard_input(void);
void setup_memory_buffers(void);
void clear_color_buffer(uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_circle(int x, int y, int radius, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_polygon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int x5, int y5, uint32_t color);
void draw_cloud();
void draw_snow();
void draw_snowman();
void draw_tree(int x, int y, int trunk_width, int trunk_height, uint32_t color);
void draw_star(int x, int y, int size, uint32_t color, float angle);
void project_square_pyramid();
void project_octahedron();
void project_triangular_pyramid();
void project_octahedron2();
uint32_t generate_random_color();
vec2_t perspective_project_point(vec3_t point_3d);

bool initialize_windowing_system() {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL_Init() Failed\n");
        return false;
    }

    SDL_DisplayMode display_mode;
    if (SDL_GetCurrentDisplayMode(0, &display_mode) != 0) {
        fprintf(stderr, "SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError());
        return false;
    }
    window_width = display_mode.w;
    window_height = display_mode.h;
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_FULLSCREEN);

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow() Failed\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer() Failed\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void clean_up() {
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void run_render_pipeline() {

    SDL_UpdateTexture(texture, NULL, color_buffer, (int)(window_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void process_keyboard_input(void) {

    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {

    case SDL_QUIT:
        is_running = false;
        break;

    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            is_running = false;
        break;

    }
}

void setup_memory_buffers(void) {

    color_buffer = (uint32_t*)malloc(window_width * window_height * sizeof(uint32_t));

    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height);
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(y * window_width) + x] = color;
        }
    }
}

uint32_t generate_random_color() {
    uint8_t r = rand() % 256; 
    uint8_t g = rand() % 256; 
    uint8_t b = rand() % 256;

    return (r << 16) | (g << 8) | b;
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x < window_width && y < window_height) {
        color_buffer[(y * window_width) + x] = color;
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    //Top
    draw_line(x, y, x + width, y, generate_random_color());
    //Bottom
    draw_line(x, y + height, x + width, y + height, generate_random_color());
    //Left
    draw_line(x, y, x, y + height, generate_random_color());
    //Right
    draw_line(x + width, y, x + width, y + height, generate_random_color());
}

void draw_circle(int x, int y, int radius, uint32_t color) {
    int current_x = radius;
    int current_y = 0;
    int err = 0;
    
    //Octant
    while (current_x >= current_y) {
        draw_pixel(x + current_x, y + current_y, color); 
        draw_pixel(x + current_y, y + current_x, color); 
        draw_pixel(x - current_y, y + current_x, color); 
        draw_pixel(x - current_x, y + current_y, color); 
        draw_pixel(x - current_x, y - current_y, color); 
        draw_pixel(x - current_y, y - current_x, color); 
        draw_pixel(x + current_y, y - current_x, color); 
        draw_pixel(x + current_x, y - current_y, color); 

        //Error
        if (err <= 0) {
            err += 2 * current_y + 1;
            current_y++;
        }

        if (err > 0) {
            err -= 2 * current_x + 1;
            current_x--;
        }
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_polygon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int x5, int y5, uint32_t color) {
    y0 += poly_y;
    y1 += poly_y;
    y2 += poly_y;
    y3 += poly_y;
    y4 += poly_y;
    y5 += poly_y;

    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x3, y3, color);
    draw_line(x3, y3, x4, y4, color);
    draw_line(x4, y4, x5, y5, color);
    draw_line(x5, y5, x0, y0, color);

    //Vertical loop for translation
    poly_y += 5;
    if (poly_y >= window_height) {
        poly_y = -70;
    }
}

void draw_cloud() {
    //left to right
    draw_rect(rect_x, 100, 500, 200, generate_random_color());
    draw_rect(rect_x + 800, 120, 500, 200, generate_random_color());
    draw_rect(rect_x + 1500, 126, 500, 200, generate_random_color());
    draw_rect(rect_x + 2100, 100, 500, 200, generate_random_color());

    //right to left
    draw_rect(100 - rect_x, 100, 500, 200, generate_random_color());
    draw_rect(800 - rect_x, 130, 500, 200, generate_random_color());
    draw_rect(1500 - rect_x, 150, 500, 200, generate_random_color());
    draw_rect(2100 - rect_x, 100, 500, 200, generate_random_color());

    //Horizontal right loop for translation
    rect_x += 5;
    if (rect_x >= window_width) {
        rect_x = -70;
    }
}
void draw_snow() {
    //Generate random # snow
    int num_snow = rand() % 51 + 50;

    for (int i = 0; i < num_snow; i++) {
        int x = rand() % window_width;
        int y = rand() % (window_height - 350) + 350; //Below the clouds

        draw_pixel(x, y, 0xFFFFFF);
    }
}

void draw_snowman() {
    //head
    draw_circle(snowman_x, (window_height / 2) + 200, 100, 0xFFFFFF);

    //body
    draw_circle(snowman_x, (window_height / 2) + 500, 200, 0xFFFFFF);

    //eyes
    draw_pixel(snowman_x - 25, (window_height / 2) + 180, 0xFF0000); // Left eye
    draw_pixel(snowman_x + 25, (window_height / 2) + 180, 0xFF0000); // Right eye

    //nose
    draw_triangle(snowman_x - 5, (window_height / 2) + 200,
                  snowman_x + 5, (window_height / 2) + 200,
                  snowman_x, (window_height / 2) + 210, 0xFFA500);

    //mouth
    draw_triangle(snowman_x - 20, (window_height / 2) + 240,
                  snowman_x, (window_height / 2) + 250,
                  snowman_x + 20, (window_height / 2) + 240, 0xe4c1ad);

    //hat
    draw_rect(snowman_x - 70, (window_height / 2) + 70, 140, 30, 0x00FF00);
    draw_rect(snowman_x - 35, (window_height / 2) - 70, 70, 140, 0x00FF00);

    //Horizontal right loop for translation
    snowman_x += 5;
    if (snowman_x >= window_width) {
        snowman_x = -70;
    }
}

void draw_tree(int x, int y, int trunk_width, int trunk_height, uint32_t color) {
    //trunk
    draw_rect(x - trunk_width / 2, y - 100, trunk_width, trunk_height, color);

    //Leaf
    int lx = x;
    int ly = y - trunk_height;

    //Top
    draw_triangle(lx, ly, lx - 50, ly + 50, lx + 50, ly + 50, generate_random_color());

    //Middle
    draw_triangle(lx, ly + 40, lx - 70, ly + 90, lx + 70, ly + 90, generate_random_color());

    //Bottom
    draw_triangle(lx, ly + 80, lx - 90, ly + 140, lx + 90, ly + 140, generate_random_color());
}


void draw_star(int x, int y, int size, uint32_t color, float angle) {
    //triangles vertices 4 triangle by 3*2 points
    float vertices[4][6] = {
        {x, y - size, x - size / 2, y + size / 2, x + size / 2, y + size / 2}, // Top triangle
        {x, y + size, x - size / 2, y - size / 2, x + size / 2, y - size / 2}, // Bottom triangle
        {x - size, y, x + size / 2, y - size / 2, x + size / 2, y + size / 2}, // Left triangle
        {x + size, y, x - size / 2, y - size / 2, x - size / 2, y + size / 2}  // Right triangle
    };

    //rotation
    for (int i = 0; i < 4; ++i) {
        float rotated_vertices[6];
        for (int j = 0; j < 6; ++j) {
            if (j % 2 == 0) {
                rotated_vertices[j] = x + (vertices[i][j] - x) * cos(angle) - (vertices[i][j + 1] - y) * sin(angle);
            }
            else {
                rotated_vertices[j] = y + (vertices[i][j] - y) * cos(angle) + (vertices[i][j - 1] - x) * sin(angle);
            }
        }
        draw_triangle(rotated_vertices[0], rotated_vertices[1], rotated_vertices[2], rotated_vertices[3], rotated_vertices[4], rotated_vertices[5], color);
    }
}

vec2_t perspective_project_point(vec3_t point_3d) {
    vec2_t projected_point = { .x = (scaling_factor * point_3d.x) / point_3d.z, .y = (scaling_factor * point_3d.y) / point_3d.z };
    return projected_point;
}

void project_square_pyramid() {
    for (int i = 0; i < N_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];
        vec3_t face_vertices[3];

        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];

            //Rotation
            transformed_vertex = vec3_rotate_x(transformed_vertex, square_pyramid_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, square_pyramid_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, square_pyramid_rotation.z);

            //Translation
            transformed_vertex = vec3_translate(transformed_vertex, square_pyramid_translation.x, square_pyramid_translation.y, square_pyramid_translation.z);

            //Scaling
            transformed_vertex = vec3_scale(transformed_vertex, square_pyramid_scaling.x, square_pyramid_scaling.y, square_pyramid_scaling.z);

            transformed_vertex.z -= camera_position.z;

            vec2_t projected_point = perspective_project_point(transformed_vertex);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }
        triangles_to_render[i] = projected_triangle;
    }
}

void project_octahedron() {
    for (int i = 0; i < N_MESH2_FACES; i++) {
        face_t mesh2_face = mesh2_faces[i];
        vec3_t face2_vertices[3];

        face2_vertices[0] = mesh2_vertices[mesh2_face.a];
        face2_vertices[1] = mesh2_vertices[mesh2_face.b];
        face2_vertices[2] = mesh2_vertices[mesh2_face.c];

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face2_vertices[j];

            //Rotation
            transformed_vertex = vec3_rotate_y(transformed_vertex, octahedron_rotation.y);

            //Translation
            transformed_vertex = vec3_translate(transformed_vertex, octahedron_translation.x, octahedron_translation.y, octahedron_translation.z);

            //Scaling
            transformed_vertex = vec3_scale(transformed_vertex, octahedron_scaling.x, octahedron_scaling.y, octahedron_scaling.z);

            transformed_vertex.z -= camera_position.z;

            vec2_t projected_point = perspective_project_point(transformed_vertex);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }
        triangles2_to_render[i] = projected_triangle;
    }
}

void project_triangular_pyramid() {
    for (int i = 0; i < N_MESH3_FACES; i++) {
        face_t mesh3_face = mesh3_faces[i];
        vec3_t face3_vertices[3];

        face3_vertices[0] = mesh_vertices[mesh3_face.a - 1];
        face3_vertices[1] = mesh_vertices[mesh3_face.b - 1];
        face3_vertices[2] = mesh_vertices[mesh3_face.c - 1];

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face3_vertices[j];

            //Rotation
            transformed_vertex = vec3_rotate_x(transformed_vertex, triangular_pyramid_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, triangular_pyramid_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, triangular_pyramid_rotation.z);

            //Translation
            transformed_vertex = vec3_translate(transformed_vertex, triangular_pyramid_translation.x, triangular_pyramid_translation.y, triangular_pyramid_translation.z);

            //Scaling
            transformed_vertex = vec3_scale(transformed_vertex, triangular_pyramid_scaling.x, triangular_pyramid_scaling.y, triangular_pyramid_scaling.z);

            transformed_vertex.z -= camera_position.z;

            vec2_t projected_point = perspective_project_point(transformed_vertex);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }
        triangles3_to_render[i] = projected_triangle;
    }
}

void project_octahedron2() {
    for (int i = 0; i < N_MESH2_FACES; i++) {
        face_t mesh2_face = mesh2_faces[i];
        vec3_t face2_vertices[3];

        face2_vertices[0] = mesh2_vertices[mesh2_face.a];
        face2_vertices[1] = mesh2_vertices[mesh2_face.b];
        face2_vertices[2] = mesh2_vertices[mesh2_face.c];

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face2_vertices[j];

            //Rotation
            transformed_vertex = vec3_rotate_y(transformed_vertex, octahedron2_rotation.y);

            //Translation
            transformed_vertex = vec3_translate(transformed_vertex, octahedron2_translation.x, octahedron2_translation.y, octahedron2_translation.z);

            //Scaling
            transformed_vertex = vec3_scale(transformed_vertex, octahedron2_scaling.x, octahedron2_scaling.y, octahedron2_scaling.z);

            transformed_vertex.z -= camera_position.z;

            vec2_t projected_point = perspective_project_point(transformed_vertex);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }
        triangles2_to_render[i] = projected_triangle;
    }
}

void update_state() {
    static bool square_pyramid_appeared = false;
    static bool octahedron_appeared = false;
    static bool triangular_pyramid_appeared = false;
    static bool octahedron2_appeared = false;
    static bool cloud_appeared = false;
    static bool snow_appeared = false;
    static bool snowman_appeared = false;
    static bool polygon_appeared = false;
    static bool tree_appeared = false;
    static bool star_appeared = false;
    static uint32_t start_time = 0;   
    uint32_t current_time = SDL_GetTicks();

    //Timer
    if (start_time == 0) {
        start_time = current_time;
    }

    uint32_t elapsed_time = current_time - start_time;
    clear_color_buffer(0xFF000000);

    //Cloud
    if (elapsed_time >= 0) {
        cloud_appeared = true;
    }
    if (cloud_appeared) {
        draw_cloud();
    }
    
    //Snow appear
    if (elapsed_time >= 10000) {
        snow_appeared = true;
    }
    if (snow_appeared) {
        draw_snow();
    }

    //Snowman
    if (elapsed_time >= 30000) {
        snowman_appeared = true;
    }
    if (snowman_appeared) {
        draw_snowman();
    }

    //Star
    if (elapsed_time >= 48000) {
        clear_color_buffer(0xFF000000);
        cloud_appeared = false;
        snow_appeared = false;
        snowman_appeared = false;
        star_appeared = true;
    }
    if (star_appeared) {
        float speed = 0.01;
        static float angle = 0;
        angle += speed;
        if (elapsed_time >= 48000) {
            draw_star(window_width / 2, window_height / 2, 100, 0xFFFF00, angle);
        }
        if (elapsed_time >= 48300) {
            draw_star(window_width / 2 - 300, window_height / 2 - 400, 100, 0xFFFF00, angle);
        }
        if (elapsed_time >= 48600) {
            draw_star(window_width / 2 + 200, window_height / 2 + 500, 100, 0xFFFF00, angle);
        }
    }

    //Square pyramid
    if (elapsed_time >= 49000) {
        clear_color_buffer(0xFF000000);
        star_appeared = false;
        square_pyramid_appeared = true;
    }
    
    if (square_pyramid_appeared) {
        square_pyramid_rotation.x += 0.01;
        square_pyramid_rotation.y += 0.01;
        square_pyramid_rotation.z += 0.01;

        //Breathing effects
        float breathing_factor = sin(elapsed_time * 0.001); 
        square_pyramid_scaling.x = 0.8 + breathing_factor * 0.1; 
        float translation_factor = breathing_factor * 0.1; 

        square_pyramid_translation.x += translation_factor;
        
        project_square_pyramid();
        for (int i = 0; i < N_MESH_FACES; i++) {
            triangle_t triangle = triangles_to_render[i];
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFF0000);
        }
    }
    
    //Star
    if (elapsed_time >= 56000) {
        clear_color_buffer(0xFF000000);
        square_pyramid_appeared = false;
        star_appeared = true;

        float speed = 0.01;
        static float angle = 0;
        angle += speed;
        if (elapsed_time >= 56000) {
            draw_star(window_width / 2, window_height / 2, 100, 0xFFFF00, angle);
        }
        if (elapsed_time >= 56300) {
            draw_star(window_width / 2 - 100, window_height / 2 - 300, 100, 0xFFFF00, angle);
        }
        if (elapsed_time >= 56600) {
            draw_star(window_width / 2 + 400, window_height / 2 + 400, 100, 0xFFFF00, angle);
        }
    }

    //Octahedron
    if (elapsed_time >= 57000) {
        clear_color_buffer(0xFF000000);
        star_appeared = false;
        octahedron_appeared = true;
    }

    if (octahedron_appeared) {
        octahedron_rotation.x += 0.01;
        octahedron_rotation.y += 0.01;
        octahedron_rotation.z += 0.01;

        project_octahedron();
        for (int i = 0; i < N_MESH2_FACES; i++) {
            triangle_t triangle = triangles2_to_render[i];
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                generate_random_color());
        }
    }
    
    //Star
    if (elapsed_time >= 63500) {
        clear_color_buffer(0xFF000000);
        octahedron_appeared = false;
        star_appeared = true;

        float speed = 0.01;
        static float angle = 0;
        angle += speed;
        if (elapsed_time >= 63500) {
            draw_star(window_width / 2, window_height / 2, 100, 0xFFFF00, angle);
        }
        if (elapsed_time >= 63800) {
            draw_star(window_width / 2 - 550, window_height / 2 - 150, 100, 0xFFFF00, angle);
        }
        if (elapsed_time >= 64100) {
            draw_star(window_width / 2 + 430, window_height / 2 + 250, 100, 0xFFFF00, angle);
        }
    }
    
    //Triangular pyramid
    if (elapsed_time >= 65000) {
        clear_color_buffer(0xFF000000);
        star_appeared = false;
        triangular_pyramid_appeared = true;
    }

    if (triangular_pyramid_appeared) {
        triangular_pyramid_rotation.x += 0.01;

        //Breathing effects
        float breathing_factor = sin(elapsed_time * 0.001); 
        triangular_pyramid_scaling.y = 0.8 + breathing_factor * 0.01; 
        float translation_factor = breathing_factor * 0.05; 

        triangular_pyramid_translation.y += translation_factor;

        project_triangular_pyramid();
        for (int i = 0; i < N_MESH_FACES; i++) {
            triangle_t triangle = triangles3_to_render[i];
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0x00FF00);
        }
    }

    //Star
    if (elapsed_time >= 71500) {
        clear_color_buffer(0xFF000000);
        triangular_pyramid_appeared = false;
        star_appeared = true;
        float speed = 0.01;
        static float angle = 0;
        angle += speed;

        if (elapsed_time >= 71500) {
            draw_star(window_width / 2, window_height / 2, 100, 0xFFFF00, angle);
        }
        if (elapsed_time >= 71800) {
            draw_star(window_width / 2 - 300, window_height / 2 - 140, 100, 0xFFFF00, angle);
        }
        if (elapsed_time >= 72100) {
            draw_star(window_width / 2 + 130, window_height / 2 + 150, 100, 0xFFFF00, angle);
        }
    }

    //All 3d
    if (elapsed_time >= 73000) {
        clear_color_buffer(0xFF000000);
        star_appeared = false;
        square_pyramid_appeared = true;
        octahedron_appeared = true;
        triangular_pyramid_appeared = true;
        
        project_square_pyramid();
        for (int i = 0; i < N_MESH_FACES; i++) {
            triangle_t triangle = triangles_to_render[i];
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFF0000);
        }

        project_octahedron();
        for (int i = 0; i < N_MESH2_FACES; i++) {
            triangle_t triangle = triangles2_to_render[i];
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                generate_random_color());
        }

        project_triangular_pyramid();
        for (int i = 0; i < N_MESH_FACES; i++) {
            triangle_t triangle = triangles3_to_render[i];
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0x00FF00);
        }
    }

    //Tree
    if (elapsed_time >= 80000) {
        clear_color_buffer(0xFF000000);
        square_pyramid_appeared = false;
        octahedron_appeared = false;
        triangular_pyramid_appeared = false;
        star_appeared = false;
        tree_appeared = true;
        cloud_appeared = true;
        snow_appeared = true;
    }
    if (tree_appeared) {
        draw_tree(window_width / 2, window_height - 70, 50, 245, generate_random_color());
        draw_cloud();
        draw_snow();
    }

    //Polygon
    if (elapsed_time >= 85000) {
        polygon_appeared = true;
    }
    if (polygon_appeared) {
        draw_polygon(100, 100, 150, 150, 200, 100, 200, 200, 150, 250, 100, 200, generate_random_color());
        draw_polygon(300, 200, 350, 250, 400, 200, 400, 300, 350, 350, 300, 300, generate_random_color());
        draw_polygon(500, 300, 550, 350, 600, 300, 600, 400, 550, 450, 500, 400, generate_random_color());
        draw_polygon(700, 400, 750, 450, 800, 400, 800, 500, 750, 550, 700, 500, generate_random_color());
        draw_polygon(900, 500, 950, 550, 1000, 500, 1000, 600, 950, 650, 900, 600, generate_random_color());
        draw_polygon(window_width - 100, 100, window_width - 150, 150, window_width - 200, 100, window_width - 200, 200, window_width - 150, 250, window_width - 100, 200, generate_random_color());
        draw_polygon(window_width - 300, 200, window_width - 350, 250, window_width - 400, 200, window_width - 400, 300, window_width - 350, 350, window_width - 300, 300, generate_random_color());
        draw_polygon(window_width - 500, 300, window_width - 550, 350, window_width - 600, 300, window_width - 600, 400, window_width - 550, 450, window_width - 500, 400, generate_random_color());
        draw_polygon(window_width - 700, 400, window_width - 750, 450, window_width - 800, 400, window_width - 800, 500, window_width - 750, 550, window_width - 700, 500, generate_random_color());
        draw_polygon(window_width - 900, 500, window_width - 950, 550, window_width - 1000, 500, window_width - 1000, 600, window_width - 950, 650, window_width - 900, 600, generate_random_color());
    }

    //Octahedron2
    if (elapsed_time >= 90000) {
        octahedron2_appeared = true;
    }

    if (octahedron2_appeared) {
        octahedron2_rotation.x += 0.01;
        octahedron2_rotation.y += 0.01;
        octahedron2_rotation.z += 0.01;

        if (octahedron2_translation.x > window_width) {
            octahedron2_translation.x += 0.1;
            octahedron2_scaling.z += 0.1;
        }
        else if (octahedron2_translation.x < window_width) {
            octahedron2_translation.x -= 0.1;
            octahedron2_scaling.z -= 0.1;
        }

        project_octahedron2();
        for (int i = 0; i < N_MESH2_FACES; i++) {
            triangle_t triangle = triangles2_to_render[i];
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFFEA00);
        }
    }
    
    //Clear all
    if (elapsed_time >= 103000) {
        clear_color_buffer(0xFF000000);
        cloud_appeared = false;
        snow_appeared = false;
        tree_appeared = false;
        polygon_appeared = false;
        octahedron2_appeared = false;
    }

    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    previous_frame_time = SDL_GetTicks();
}

int main(void) {
    is_running = initialize_windowing_system(); 
    setup_memory_buffers();

    //Game loop
    while (is_running) {
        process_keyboard_input(); 
        update_state();
        run_render_pipeline();
    }
    clean_up();
    return 0;
}
