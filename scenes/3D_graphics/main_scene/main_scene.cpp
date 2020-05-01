
#include "main_scene.hpp"

#include <random>
#include <fstream>

#ifdef MAIN_SCENE

using namespace vcl;

void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 20.0f;
    scene.camera.apply_rotation(0,0,2,1.2f);
    
    map.create_sky(); // Skybox   

    char_pos = { 100,100,100 }; // tant qu'on n'a pas un vrai Mario

    chomp.init({ -2.65f,0.2f,0.77f }); // Position of the bottom of the Chomp

    map.loadMTL("scenes/shared_assets/models/Bob-omb/Bob-omb Battlefield.mtl");
    map.loadOBJ("scenes/shared_assets/models/Bob-omb/Bob-omb Battlefield.obj");

    timer.scale = 1.0f;
    timer.t_max = 20.0f;
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    timer.update();
    const float t = timer.t;
    const float dt = std::min(0.03f, timer_event.update());
    set_gui();

    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe

    chomp.move(char_pos, t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);

    chomp.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);

    // Display sky
    glBindTexture(GL_TEXTURE_2D, map.texture_sky);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    map.sky.uniform.transform.translation = scene.camera.camera_position(); // Move sky around camera
    draw(map.sky, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(map.sky, scene.camera, shaders["wireframe"]);

    // Bob omb battlefield map
    for (int i = 2; i < (int)map.map.size(); i++) { // 0 and 1 are billboards
        glBindTexture(GL_TEXTURE_2D, map.map_textures[map.texture_indices[i]]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (gui_scene.surface) draw(map.map[i], scene.camera, shaders["mesh"]);
        if (gui_scene.wireframe) draw(map.map[i], scene.camera, shaders["wireframe"]);
    }

    //// BILLBOARDS ALWAYS LAST ////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, map.map_textures[map.texture_indices[i]]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (gui_scene.surface) draw(map.map[i], scene.camera, shaders["mesh"]);
        if (gui_scene.wireframe) draw(map.map[i], scene.camera, shaders["wireframe"]);
    }
    chomp.draw_billboards(shaders, scene, gui_scene.billboards, gui_scene.wireframe);
    glDepthMask(true);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    last_t = t;
}

////////////////////////////// USER INTERFACE //////////////////////////////

/*/ Mouse click is used to select a position of the control polygon
void scene_model::mouse_click(scene_structure& scene, GLFWwindow* window, int, int, int)
{
    const bool mouse_click_left = glfw_mouse_pressed_left(window);
    const bool key_shift = glfw_key_shift_pressed(window);

    if (mouse_click_left && key_shift)
    {
        // Create the 3D ray passing by the selected point on the screen
        const ray r = picking_ray(scene.camera, glfw_cursor_coordinates_window(window));

        // Loop over all positions and get the closest intersected sphere if any
        picked_object = -1;
        float distance_min = 0.0f;
        for (int k = 0; k < keyframes.size(); k++) {
            const vec3 c = keyframes[k].p;
            const picking_info info = ray_intersect_sphere(r, c, 0.1f);

            if (info.picking_valid) { // the ray intersects a sphere
                const float distance = norm(info.intersection - r.p); // get the closest intersection
                if (picked_object == -1 || distance < distance_min) {
                    distance_min = distance;
                    picked_object = k;
                }
            }
        }
    }
}

// Translate the selected object to the new pointed mouse position within the camera plane
void scene_model::mouse_move(scene_structure& scene, GLFWwindow* window)
{
    const bool mouse_click_left = glfw_mouse_pressed_left(window);
    const bool key_shift = glfw_key_shift_pressed(window);
    if (mouse_click_left && key_shift && picked_object != -1) {
        // Get vector orthogonal to camera orientation
        const mat4 M = scene.camera.camera_matrix();
        const vec3 n = { M(0,2),M(1,2),M(2,2) };

        // Intersection between current ray and the plane orthogonal to the view direction and passing by the object
        const vec2 cursor = glfw_cursor_coordinates_window(window);
        const ray r = picking_ray(scene.camera, cursor);
        vec3& p0 = keyframes[picked_object].p;
        const picking_info info = ray_intersect_plane(r, n, p0);

        // translate the position
        p0 = info.intersection;
        equalize_speed(keyframes, timer);
    }
}*/

void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
    ImGui::Checkbox("Billboards", &gui_scene.billboards); ImGui::SameLine();

    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);
}

#endif