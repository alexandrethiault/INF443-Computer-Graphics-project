#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_BUBBLE

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
};

struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
    bool bubbles = true;
};

struct scene_model : scene_base
{
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void simulate_particles(float dt);
    void set_gui();

    float bubble_radius;
    std::list<bubble_structure> particles; // Storage of all currently active particles

    vcl::mesh_drawable sphere; // Visual representation of a bubble - a sphere
    vcl::mesh_drawable demo_ground; // Visual representation of the ground - a disc
    GLuint texture_support, texture_bridge;
    gui_scene_structure gui_scene;
    vcl::timer_event timerevent;    // Timer allowing to indicate periodic events

};

#endif
