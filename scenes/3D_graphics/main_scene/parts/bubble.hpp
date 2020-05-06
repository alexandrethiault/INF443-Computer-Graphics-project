#pragma once

#include "main/scene_base/base.hpp"

#ifdef MAIN_SCENE

struct bubble_structure
{
    vcl::vec3 p; // Position
    vcl::vec3 v; // Speed
    bool squishing;
    bool unsquishing;
    int squish_counter;
    float squish; // si squishing, squish peut aller de 0 à 1, et si unsquishing, de 1 à 0

    bubble_structure(vcl::vec3 p, vcl::vec3 v) {
        this->p = p; this->v = v;
        squishing = unsquishing = false;
        squish = 0.0f;
        squish_counter = 0;
    }

    void squish_tranformation(vcl::mesh_drawable& sphere, float bubble_radius) {
        sphere.uniform.transform.scaling_axis = { 1 + squish, 1 + squish, 1 - squish };
        sphere.uniform.transform.translation = p - bubble_radius * vcl::vec3{ 0,0, squish };
    }
};

struct bubbles_structure
{
    void setup();
    void draw_bubbles(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void simulate();

    float radius;
    std::list<bubble_structure> positions; // Storage of all currently active particles
    vcl::mesh_drawable sphere; // Visual representation of a bubble - a sphere
    GLuint texture_bubble;

    vcl::timer_event timerevent;    // Timer allowing to indicate periodic events
};

#endif
