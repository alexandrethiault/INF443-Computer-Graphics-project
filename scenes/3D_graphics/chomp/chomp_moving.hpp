#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_CHOMP_MOVING


struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
};

struct scene_model : scene_base
{

    void setup_data(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    vcl::hierarchy_mesh_drawable hierarchy_chomp;

    vcl::mesh_drawable demo_ground;

    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
};

#endif

