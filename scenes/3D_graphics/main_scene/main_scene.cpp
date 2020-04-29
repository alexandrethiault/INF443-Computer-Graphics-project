
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

    char_pos = { 100,100,100 }; // tant qu'on n'a pas un vrai Mario

    chomp.init({ -2.545635f, 0.232917f, 0.782936f }); // Position of the bottom of the Chomp

    map.loadMTL("scenes/shared_assets/models/Bob-omb/Bob-omb Battlefield.mtl");
    map.loadOBJ("scenes/shared_assets/models/Bob-omb/Bob-omb Battlefield.obj");
    map.create_sky(); // Skybox  
    map.other_objects();

    star.create_star();

    timer.scale = 1.0f;
    timer.t_max = 20.0f;
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    timer.update();
    const float t = timer.t;
    const float dt = std::min(0.03f, timer_event.update());
    set_gui();

    chomp.move(char_pos, t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);
    star.move(t);

    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe

    chomp.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);
    map.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe); // Including sky and 5 posts
    star.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe); // 2 stars

    //// BILLBOARDS ALWAYS LAST ////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    // étoiles puis grille du chomp puis chomp (chaine) puis reste de la map (arbres, pièces)
    star.draw_billboards(shaders, scene, gui_scene.surface, gui_scene.wireframe); // Star eyes
    chomp.draw_billboards(shaders, scene, gui_scene.billboards, gui_scene.wireframe); // Eyes and chains
    map.draw_billboards(shaders, scene, gui_scene.billboards, gui_scene.wireframe); // 2 types of grids and 17 trees
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

void scene_model::keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        vec3& pos = scene.frame_camera.uniform.transform.translation;
        std::cout << "vec3{ " << pos.x << "f," << pos.y << "f," << pos.z << "f } " << pos << std::endl;
    }
}

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
