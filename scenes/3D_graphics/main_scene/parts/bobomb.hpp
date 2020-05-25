#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/mtltexture.hpp"
#include "scenes/3D_graphics/main_scene/parts/map.hpp"
#include "scenes/3D_graphics/main_scene/parts/bridge.hpp"

#ifdef MAIN_SCENE

struct bobombs_structure;

struct bobomb_structure
{
    bool is_pink = false;
    bool rushing = false, exploding = false, fire = false, falling = false, hide = false, wait = false;
    float w = 0.f, ampl = 0.f;
    
    float angle = 0.f, angular_v = 0.f,  hspeed = 0.f, vspeed = 0.f, time_chasing = 0.f;
    vcl::vec3 center, rel_position, rush_speed, original_pos;

    // shared parameters, copied for each bobomb because it's cheap and the code will be clearer
    float radius_boulon = 0.f, height_boulon = 0.f, cote_corps = 0.f, height_pied = 0.f, height_yeux = 0.f, radius_reach = 0.f, scaling = 0.f;
    float max_angular_velocity = 0.f, max_speed = 0.f, temps_explode = 0.f, temps_wait = 0.f, temps_fire = 0.f;
    vcl::vec3 centre_corps;
    //vcl::hierarchy_mesh_drawable* hierarchy; // This one is expensive to copy so it's a pointer
    map_structure* map = nullptr;
    bridge_structure* bridge = nullptr;

    void init(const vcl::vec3& _center, bobombs_structure* bobombs, bool is_pink);
    vcl::vec3 get_position();

    void move_black(const vcl::vec3& char_pos, float t, float dt);
    void move_pink(float t, float dt);
    void keyboard_input(bool Z, bool W, bool D, bool S, bool A, bool Q, bool SPACE);
};

struct bobombs_structure
{
    float radius_boulon = 0.f, height_boulon = 0.f, cote_corps = 0.f, height_pied = 0.f, height_yeux = 0.f, radius_reach = 0.f, scaling = 0.f;
    float max_angular_velocity = 0.f, max_speed = 0.f, temps_explode = 0.f, temps_wait = 0.f, temps_fire = 0.f;
    int frame_explosion = 14;
    vcl::vec3 centre_corps;

    vcl::hierarchy_mesh_drawable hierarchy;
    vcl::mesh_drawable fumee, explosion;
    GLuint texture_yeux, texture_corps, texture_corps_rose, texture_fumee, *textures_explosion;
    vcl::vec3 couleur_pied, couleur_boulon;
    map_structure* map = nullptr;
    bridge_structure* bridge = nullptr;

    std::vector<bobomb_structure> bobombs;
    bobomb_structure pbobomb;
    float t;

    void setup(map_structure* _map, bridge_structure* _bridge, vcl::vec3 pink_position);

    void move(const vcl::vec3& char_pos, float t, float dt);

    void draw_part_nogl(std::string name, std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surfbb, bool wf);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
};

#endif
