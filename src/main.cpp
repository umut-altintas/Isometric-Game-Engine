#include "raylib.h"
#include "raymath.h"
#include "iostream"

const int FPS = 60;

Vector2 screen_size = {800, 800};
Vector2 screen_center = screen_size/2;

// Cell shape changed to w = 2*h
// TODO:
// 1. Create a function that makes texture stay on fixed position.
// 2. Revise the UpdateCamera function and make it:
//    -> if selected fixed_camera; camera can be see all the grid 
//    -> if selected moving_camera; camera will follow the player
// 3. Make a audio handler and the first song is: "Make the world go away".
// 4. Make a scrollable panel that sits bottom of the screen. The panel will be similar with sims 2 decorations panel. This panel is for the assets of the game.
// 5. Make interactable objects that plays different types of music.

class Grid {
public:
    int grid_count = 20;
    float iso_h = 30;
    float iso_w = iso_h * 2;

    Vector2 grid_size = {iso_w*grid_count, iso_h*grid_count};
    Vector2 grid_center = grid_size/2;
    
    float world_origin_x = screen_center.x - (grid_size.x/2);
    float world_origin_y = screen_center.y - (grid_size.y/2);

    float axis_i = world_origin_x + (iso_w * grid_count/2); // i = 0
    float axis_j = world_origin_y + (iso_h/2); // j = 0
    Vector2 start_place = {axis_i, axis_j};        // 0,0

    Vector2 increment_on_i = {iso_w/2, iso_h/2};   // +1,0
    Vector2 increment_on_j = {-iso_w/2, iso_h/2};  // 0,+1
    Vector2 decrement_on_i = {-iso_w/2, -iso_h/2}; // -1,0
    Vector2 decrement_on_j = {iso_w/2, -iso_h/2};  // 0,-1
    Rectangle cell_shape{};
/*
    Vector2 global_to_local_position(Vector2 mouse_position) {
        float local_i = (mouse_position.x - axis_i) / iso_w + (mouse_position.y - axis_j) / iso_h;
        float local_j = (mouse_position.y - axis_j) / iso_h - (mouse_position.x - axis_i) / iso_w;

        if(round((local_i) < 0 || round(local_j) < 0) || (round(local_i) > grid_count-1 || round(local_j) > grid_count - 1)){
            return{-1,-1};
        }
        else{
            return {abs(round(local_i)), abs(round(local_j))};
        }
    }
*/
Vector2 global_to_local_position(Vector2 mouse_position) {

    float local_i = (mouse_position.x - axis_i) / iso_w + (mouse_position.y - axis_j) / iso_h;
    float local_j = (mouse_position.y - axis_j) / iso_h - (mouse_position.x - axis_i) / iso_w;

    float i = round(local_i);
    float j = round(local_j);

    // Correct small negative values near the axes
    if (i < 0 && local_i > -0.5f) i = 0;
    if (j < 0 && local_j > -0.5f) j = 0;
    if(i == -0.0f) i=0;
    if(j == -0.0f) j=0;

    if (i < 0 || j < 0 || i > grid_count - 1 || j > grid_count - 1) {
        return {-1, -1};
    }
    else{
        return {i, j};
    }
}
    Vector2 local_to_global_position(Vector2 local_pos) {
    return {
        axis_i + local_pos.x * increment_on_i.x + local_pos.y * increment_on_j.x,
        axis_j + local_pos.x * increment_on_i.y + local_pos.y * increment_on_j.y
    };
}
    void draw(){

    // direction i
    for(int i = 0; i <= grid_count; i++){
        DrawLine(
            world_origin_x + grid_size.x/2 - (i*iso_w/2),
            world_origin_y + (i*iso_h/2),
            world_origin_x + grid_size.x - (i*iso_w/2),
            world_origin_y + grid_size.y/2 + (i*iso_h/2),
            BLACK
        );
    }

    // direction j
    for(int j = 0; j <= grid_count; j++){
        DrawLine(
            world_origin_x + grid_size.x/2 + (j*iso_w/2),
            world_origin_y + (j*iso_h/2),
            world_origin_x + (j*iso_w/2),
            world_origin_y + grid_size.y/2 + (j*iso_h/2),
            BLACK
        );
    }
        DrawCircle(grid_center.x + world_origin_x, grid_center.y + world_origin_y, 2, MAGENTA);
        DrawCircle(start_place.x, start_place.y, 3, GREEN);
}

};

void UpdateCamera(Camera2D& camera, Grid& grid) {

    if (IsKeyPressed(KEY_UP)) {
        camera.target += grid.decrement_on_i;
    }

    if (IsKeyPressed(KEY_DOWN)) {
        camera.target += grid.increment_on_i;
    }

    if (IsKeyPressed(KEY_LEFT)) {
        camera.target += grid.increment_on_j;
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        camera.target += grid.decrement_on_j;
    }

    // Zoom
    float wheel = GetMouseWheelMove();

    if (wheel != 0) {
        camera.zoom += wheel * 0.1f;

        if (camera.zoom < 0.2f) camera.zoom = 0.2f;
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    }
}

void draw_texture_floor(Grid& grid, Camera2D& camera, Texture2D& texture, float scale) {
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 global_position = GetScreenToWorld2D(GetMousePosition(), camera);
        Vector2 local_position = grid.global_to_local_position(global_position);

        if (local_position.x >= 0 && local_position.y >= 0) {
            Vector2 texture_position = grid.local_to_global_position(local_position);
            texture_position.x -= grid.iso_w/2 ;
            texture_position.y -= grid.iso_h/2 ;
            DrawTextureEx(texture, texture_position, 0, scale, WHITE);
        }
    }
}
void draw_texture_cube(Grid& grid, Camera2D& camera, Texture2D& texture, float scale) {
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 global_position = GetScreenToWorld2D(GetMousePosition(), camera);
        Vector2 local_position = grid.global_to_local_position(global_position);

        if (local_position.x >= 0 && local_position.y >= 0) {
            Vector2 texture_position = grid.local_to_global_position(local_position);
            texture_position.x -= grid.iso_w/2 ;
            texture_position.y -= texture.height - grid.iso_h/2;
            DrawTextureEx(texture, texture_position, 0, scale, WHITE);
        }
    }
}

int main(){
    InitWindow(screen_size.x, screen_size.y, "Kitchen Game");
    SetTargetFPS(FPS);
    Texture2D texture = LoadTexture("textures/black_cube.png");
    Grid grid = Grid();
    Camera2D camera = { 0 };
    camera.target = (Vector2){screen_center};
    camera.offset = (Vector2){ GetRenderWidth()/2.0f, GetRenderHeight()/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    float scale = 1;

    while(!WindowShouldClose()){
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Vector2 global_position = GetScreenToWorld2D(GetMousePosition(), camera);
            Vector2 local_position = grid.global_to_local_position(global_position);
            std::cout<<"["<<local_position.x<<","<<local_position.y<<"]"<<std::endl;
            DrawCircle(grid.local_to_global_position(local_position).x, grid.local_to_global_position(local_position).y, 5, MAGENTA);
        }
        
        BeginDrawing();
        BeginMode2D(camera);
        grid.draw();
        ClearBackground(WHITE);
        UpdateCamera(camera, grid);
        draw_texture_cube(grid, camera, texture, scale);
        
        if(IsKeyPressed(KEY_UP)){
            grid.start_place = Vector2Add(grid.start_place, grid. decrement_on_i);
        }
        if(IsKeyPressed(KEY_DOWN)){
            grid.start_place = Vector2Add(grid.start_place, grid. increment_on_i);
        }
        if(IsKeyPressed(KEY_LEFT)){
            grid.start_place = Vector2Add(grid.start_place, grid. increment_on_j);
        }
        if(IsKeyPressed(KEY_RIGHT)){
            grid.start_place = Vector2Add(grid.start_place, grid. decrement_on_j);
        }

        EndMode2D();
        EndDrawing();
    }
    UnloadTexture(texture);
    CloseWindow();
    return 0;
}