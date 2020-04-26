#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_BRIDGE

struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
};

struct scene_model : scene_base
{
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    float thetamax, theta; // l'angle theta bascule de -thetamax à thetamax
    int period; // périodicité period : angle = thetamax * sin(2*pi*t/period)

    vcl::mesh_drawable bridge_support, bridge;
    GLuint texture_support, texture_bridge;
    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
};

#endif
