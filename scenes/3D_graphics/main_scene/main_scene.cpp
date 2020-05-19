
#include "main_scene.hpp"

#include <random>
#include <fstream>

#ifdef MAIN_SCENE

using namespace vcl;

void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 20.0f;
    scene.camera.apply_rotation(0,0,2,1.2f);

    chomp.init({ -2.545635f, 0.232917f, 0.782936f }); // Position of the bottom of the Chomp

    character.init({0,0,0});

    map.init(
        "scenes/shared_assets/models/Bob-omb/Bob-omb Battlefield.mtl",
        "scenes/shared_assets/models/Bob-omb/Bob-omb Battlefield.obj",
        &character
    );

    star.create_star();

    bridge.setup(vec3{ -1.736f, 3.139f, 0.728f }, vec3{ -1.28f,-1.28f,0 }, vec3{ 0.256f,-0.256f,0 }, vec3{ 0,0,0.054f });

    bubbles.setup(&map);

    bobombs.setup(&map, &bridge); // Pointers used for collisions

    flight.setup_flight(&character);

    timer.scale = 1.0f;
    timer.t_max = 20.0f;
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    set_gui(shaders, scene);

    timer.update();
    const float t = timer.t;
    const float dt = std::min(0.03f, timer_event.update());

    chomp.move(flight.p, t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);
    character.move(t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);
    bobombs.move(flight.p, t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);
    star.move(t);
    bridge.move(t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);
    bubbles.simulate();
    flight.simulate(); // Moves the character to the current flight position

    if (gui_scene.lock_on_mario) scene.camera.translation = -flight.p; // camera frame center = Mario
    if (gui_scene.auto_orientation) {
        scene.camera.scale = 1;
        scene.camera.orientation = rotation_to_vector_mat3({ -flight.dp.z,-flight.dp.x,flight.dp.y });
    }

    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe
    chomp.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);
    map.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe); // Including sky and 5 posts
    bobombs.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);
    star.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe); // 2 stars
    flight.draw_path(shaders, scene, gui_scene.display_keyframe, gui_scene.display_polygon);
    bridge.draw_bridge(shaders, scene, gui_scene.surface, gui_scene.wireframe);
    if (gui_scene.bubbles) bubbles.draw_bubbles(shaders, scene, gui_scene.surface, gui_scene.wireframe);
    if (gui_scene.mario) character.draw(shaders, scene, gui_scene.surface, gui_scene.wireframe);

    //// BILLBOARDS ALWAYS LAST ////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    // étoiles puis grille du chomp puis chomp (chaine) puis reste de la map (arbres, pièces)
    bobombs.draw_billboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);
    star.draw_billboards(shaders, scene, gui_scene.surface, gui_scene.wireframe); // Star eyes
    chomp.draw_billboards(shaders, scene, gui_scene.billboards, gui_scene.wireframe); // Eyes and chains
    map.draw_billboards(shaders, scene, gui_scene.billboards, gui_scene.wireframe, ((int)(16 * t)) % 4); // 2 types of grids and 17 trees
    glDepthMask(true);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    last_t = t;
}

// Mouse click is used to select a position of the control polygon
void scene_model::mouse_click(scene_structure& scene, GLFWwindow* window, int, int, int)
{
    flight.mouse_click(scene, window);
}

// Translate the selected object to the new pointed mouse position within the camera plane
void scene_model::mouse_move(scene_structure& scene, GLFWwindow* window)
{
    flight.mouse_move(scene, window);
}

// Press P to print the position of the center of the moving referential ("frame camera")
void scene_model::keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        std::cout << scene.frame_camera.uniform.transform.translation << std::endl;
}

void scene_model::set_gui(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
    ImGui::Checkbox("Billboards", &gui_scene.billboards); ImGui::SameLine();

    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.01f, 3.0f);

    ImGui::Checkbox("Mario", &gui_scene.mario); ImGui::SameLine();
    ImGui::Checkbox("Lock camera on Mario", &gui_scene.lock_on_mario); ImGui::SameLine();
    ImGui::Checkbox("Third person camera", &gui_scene.auto_orientation);
    ImGui::Checkbox("Keyframes", &gui_scene.display_keyframe); ImGui::SameLine();
    ImGui::Checkbox("Polygon", &gui_scene.display_polygon); ImGui::SameLine();
    ImGui::Checkbox("Bubbles", &gui_scene.bubbles);

    ImGui::SliderFloat("Mario's time", &flight.timer.t, flight.timer.t_min, flight.timer.t_max);
    ImGui::SliderFloat("Mario timer scale", &flight.timer.scale, 0.01f, 3.0f);

    ImGui::SliderFloat("Bubbles time scale", &bubbles.timerevent.scale, 0.01f, 3.0f);
    if (ImGui::Button("Stop bubbles")) bubbles.timerevent.stop(); ImGui::SameLine();
    if (ImGui::Button("Restart bubbles")) bubbles.timerevent.start(); ImGui::SameLine();
    if (ImGui::Button("Print Keyframe")) {
        std::cout << "keyframe_position:" << std::endl;
        for (size_t k = 0; k < flight.keyframes.size(); ++k) 
            std::cout << flight.keyframes[k].p << std::endl;
    }
}

#endif
