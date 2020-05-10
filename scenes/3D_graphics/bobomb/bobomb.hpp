#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_BOBOMB

struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
    bool billboards = true;
};

struct mtltexture {
    float Ns, Ni, d;
    vcl::vec3 Ka, Kd, Ks, Ke;
    int illum;
    std::string name, map_Kd;
};

struct bobomb_structure
{
    bool rushing, exploding, disappear;
    float radius_boulon = 0., height_boulon, cote_corps, height_pied, height_yeux, radius_reach, radius_eye, angle, angular_v, max_angular_velocity, speed, max_speed, time_chasing;
    vcl::vec3 center, centre_corps, rel_position, rush_speed;
    vcl::hierarchy_mesh_drawable hierarchy;
    vcl::mesh_drawable chain;
    GLuint texture_up, texture_down, texture_tongue, texture_eye, texture_tooth, texture_chain;

    void init(const vcl::vec3& _center);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
    void move(const vcl::vec3& char_pos, float t, float dt);

    GLuint texture_yeux, texture_corps;
    vcl::vec3 couleur_pied, couleur_boulon;
};

struct scene_model : scene_base
{

    void setup_data(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    bobomb_structure bobomb;
    vcl::mesh_drawable demo_ground, character;
    vcl::vec3 char_pos;

    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
    float last_t;
};

#endif


