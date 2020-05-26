#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/map.hpp"

#ifdef MAIN_SCENE

struct bubble_structure
{
    vcl::vec3 p; // Position
    vcl::vec3 v; // Speed
    bool squishing = false, unsquishing = false;
    int squish_counter = 0; // Goes up to 3. 3rd squish = explosion
    float squish = 0.0f; // If squishing, squish will grow from 0 to 1, and if unsquishing, decrease back to 0

    bubble_structure();
    bubble_structure(vcl::vec3 p, vcl::vec3 v);

    void squish_tranformation(vcl::mesh_drawable& sphere, float bubble_radius);
};

struct bubbles_structure
{
    void setup(map_structure* map_);
    void draw_bubbles(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void simulate(vcl::vec3& target);

    bool active = false;
    float radius = 0.0f;
    bubble_structure bubble; // Storage of a particle
    vcl::mesh_drawable sphere; // Visual representation of a bubble - a sphere
    GLuint texture_bubble;
    map_structure* map; // Pointer to the map, for collisions

    vcl::timer_event timerevent;    // Timer allowing to indicate periodic events
};

#endif
