#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/mtltexture.hpp"
#include "scenes/3D_graphics/main_scene/parts/map.hpp"
#include "scenes/3D_graphics/main_scene/parts/bridge.hpp"

#ifdef MAIN_SCENE

struct bobomb_structure
{
    bool rushing = false, exploding = false, falling = false, hide = false;
    float w, ampl;
    float radius_boulon = 0.f, height_boulon = 0.f, cote_corps = 0.f, height_pied = 0.f, height_yeux = 0.f, radius_reach = 0.f, scaling = 0.f;
    float angle = 0.f, angular_v = 0.f, max_angular_velocity = 0.f, hspeed = 0.f, vspeed = 0.f, max_speed = 0.f, time_chasing = 0.f, temps_explode = 0.f;
    vcl::vec3 center, centre_corps, rel_position, rush_speed, original_pos;
    vcl::hierarchy_mesh_drawable hierarchy;
    GLuint texture_yeux, texture_corps;
    vcl::vec3 couleur_pied, couleur_boulon;
    map_structure* map = nullptr;
    bridge_structure* bridge = nullptr;

    void init(const vcl::vec3& _center, map_structure* _map, bridge_structure* _bridge, std::string body_file);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
    void move(const vcl::vec3& char_pos, float t, float dt);
    vcl::vec3 get_position();
};

struct bobombs_structure
{
    std::vector<bobomb_structure> bobombs;

    void setup(map_structure* _map, bridge_structure* _bridge);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
    void move(const vcl::vec3& char_pos, float t, float dt);
};

struct pink_bombomb_structure : bobomb_structure
{
    void move(float t, float dt);
    void keyboard_input(bool Z, bool W, bool D, bool S, bool A, bool Q, bool SPACE);
};

#endif


