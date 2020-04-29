#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/chomp.hpp"
#include "scenes/3D_graphics/main_scene/parts/map.hpp"
#include "scenes/3D_graphics/main_scene/parts/star.hpp"

#ifdef MAIN_SCENE

struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
    bool billboards = true;
};

struct scene_model : scene_base
{
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    //void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    //void mouse_move(scene_structure& scene, GLFWwindow* window);
    void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);
    void set_gui();
    
    vcl::vec3 char_pos; // Position du Mario, utilisé par le Chomp

    chomp_structure chomp;

    map_structure map;

    star_structure star;

    gui_scene_structure gui_scene;

    vcl::timer_interval timer;
    vcl::timer_event timer_event;
    float last_t;
};

#endif