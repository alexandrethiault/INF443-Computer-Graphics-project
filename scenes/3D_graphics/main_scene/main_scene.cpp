
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

    map.loadMTL("scenes/shared_assets/models/Bob-omb/Bob-omb Battlefield.mtl");
    map.loadOBJ("scenes/shared_assets/models/Bob-omb/Bob-omb Battlefield.obj");
    map.create_sky(); // Skybox  
    map.other_objects();

    star.create_star();

    bubbles.setup(shaders, scene);
    flight.setup_flight(shaders, scene, character);

    timer.scale = 1.0f;
    timer.t_max = 20.0f;
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    set_gui();

    timer.update();
    const float t = timer.t;
    const float dt = std::min(0.03f, timer_event.update());

    chomp.move(flight.p, t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);
    character.move(t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);
    star.move(t);
    bubbles.simulate();

    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe
    chomp.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);
    map.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe); // Including sky and 5 posts
    star.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe); // 2 stars
    bubbles.draw_bubbles(shaders, scene, gui_scene.surface, gui_scene.wireframe);
    flight.draw_path(shaders, scene, gui_scene.display_keyframe, gui_scene.display_polygon);

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

void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
    ImGui::Checkbox("Billboards", &gui_scene.billboards); ImGui::SameLine();

    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

    ImGui::SliderFloat("Bubbles time scale", &bubbles.timerevent.scale, 0.1f, 3.0f);
    if (ImGui::Button("Stop bubbles")) bubbles.timerevent.stop(); ImGui::SameLine();
    if (ImGui::Button("Restart bubbles")) bubbles.timerevent.start();

    ImGui::Text("Mario: "); ImGui::SameLine();
    ImGui::Checkbox("Keyframes", &gui_scene.display_keyframe); ImGui::SameLine();
    ImGui::Checkbox("Polygon", &gui_scene.display_polygon);

    ImGui::SliderFloat("Mario's time", &flight.timer.t, flight.timer.t_min, flight.timer.t_max);
    ImGui::SliderFloat("Mario timer scale", &flight.timer.scale, 0.1f, 3.0f);

    if (ImGui::Button("Print Keyframe")) {
        std::cout << "keyframe_position:" << std::endl;
        for (size_t k = 0; k < flight.keyframes.size(); ++k) 
            std::cout << flight.keyframes[k].p << std::endl;
    }
}

#endif
