#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_ARTICULATED_HIERARCHY

struct gui_scene_structure
{
    bool wireframe   = false;
    bool surface     = true;
    bool skeleton    = false;
};

struct character_structure
{
    // prérempli avec ce que j'ai pris pour le chomp
    float radius_chomp = 0., radius_reach, angle, angular_v, max_angular_velocity, speed, max_speed;
    vcl::vec3 center, rel_position;
    vcl::hierarchy_mesh_drawable hierarchy;
    //vcl::hierarchy_mesh_drawable_display_skeleton hierarchy_visual_debug;
    GLuint texture_up, texture_down, texture_tongue, texture_eye, texture_tooth, texture_chain;

    void init(const vcl::vec3& center);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
    void move(float t, float dt);
};

struct scene_model : scene_base
{

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    character_structure character;

    vcl::mesh_drawable demo_ground;

    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
};

#endif


