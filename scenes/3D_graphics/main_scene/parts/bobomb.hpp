#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/mtltexture.hpp"
#include "scenes/3D_graphics/main_scene/parts/map.hpp"

#ifdef MAIN_SCENE

struct bobomb_structure
{
    bool rushing, exploding, disappear, falling;
    float radius_boulon = 0., height_boulon, cote_corps, height_pied, height_yeux, radius_reach, angle, angular_v, max_angular_velocity, hspeed, vspeed, max_speed, time_chasing, scaling;
    vcl::vec3 center, centre_corps, rel_position, rush_speed;
    vcl::hierarchy_mesh_drawable hierarchy;
    GLuint texture_yeux, texture_corps;
    vcl::vec3 couleur_pied, couleur_boulon;
    map_structure* map;

    void init(const vcl::vec3& _center, map_structure* _map);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
    void move(const vcl::vec3& char_pos, float t, float dt);
    vcl::vec3 get_position();
};

#endif


