#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_ARTICULATED_HIERARCHY

struct gui_scene_structure
{
    bool wireframe   = false;
    bool surface     = true;
    bool skeleton    = false;
    bool billboards = true;
};

struct mtltexture {
    float Ns, Ni, d;
    vcl::vec3 Ka, Kd, Ks, Ke;
    int illum;
    std::string name, map_Kd;
};

struct character_structure
{
    // pr�rempli avec ce que j'ai pris pour le chomp
    float radius_chomp = 0., radius_reach, angle, angular_v, max_angular_velocity, speed, max_speed;
    vcl::vec3 center, rel_position;
    vcl::hierarchy_mesh_drawable hierarchy;
    //vcl::hierarchy_mesh_drawable_display_skeleton hierarchy_visual_debug;
    GLuint texture_up, texture_down, texture_tongue, texture_eye, texture_tooth, texture_chain;

    void init(const vcl::vec3& center, std::vector<vcl::mesh_drawable> map, std::vector<GLuint> map_textures, std::vector<int> texture_indices, std::vector<std::string> part_name);
    void draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf, std::vector<std::string> part_name, std::vector<int> texture_indices, std::vector<GLuint> map_textures);
    void move(float t, float dt);
};

struct scene_model : scene_base
{

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    void loadMTL(const char* path);
    void loadOBJ(const char* path);

    character_structure character;

    vcl::mesh_drawable demo_ground;

    std::vector<vcl::mesh_drawable> map;
    std::vector<mtltexture> map_mtl;
    std::vector<int> texture_indices;
    std::vector<std::string> part_name;
    std::vector<GLuint> map_textures;

    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
    float last_t;
};

#endif


