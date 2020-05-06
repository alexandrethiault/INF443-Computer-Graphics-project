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
    vcl::mesh create_bridge(vcl::vec3& center, vcl::vec3& len, vcl::vec3& wid, vcl::vec3& hei);
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    float thetamax, theta; // l'angle theta bascule de -thetamax à thetamax
    int period; // périodicité period : angle = thetamax * sin(2*pi*t/period)
    vcl::vec3 rotation_axis;

    vcl::mesh_drawable bridge;
    GLuint texture_bridge;
    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
};

#endif
