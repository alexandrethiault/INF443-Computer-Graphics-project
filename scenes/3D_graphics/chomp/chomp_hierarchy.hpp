#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_CHOMP_HIERARCHY

struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
    bool billboards = true;
};

struct chomp_structure
{
    float radius_chomp = 0.;
    vcl::hierarchy_mesh_drawable hierarchy;
    GLuint texture_up, texture_down, texture_tongue, texture_eye, texture_tooth, texture_chain;

    void init();
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
};

struct scene_model : scene_base
{

    void setup_data(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    chomp_structure chomp;
    vcl::mesh_drawable demo_ground;

    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
};

#endif


